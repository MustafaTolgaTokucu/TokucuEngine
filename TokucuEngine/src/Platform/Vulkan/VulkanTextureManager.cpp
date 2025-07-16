#include "tkcpch.h"
#include "VulkanTextureManager.h"
#include "VulkanCore.h"

#include "stb_image/stb_image.h"

#include <thread>

namespace Tokucu {

VulkanTextureManager::VulkanTextureManager(VkDevice device, VkPhysicalDevice physicalDevice, VulkanCreateImage* createImage, VulkanBuffer* buffer)
	: m_Device(device), m_PhysicalDevice(physicalDevice), m_CreateImage(createImage), m_Buffer(buffer)
{
	m_ThreadPool = std::make_unique<ThreadPool>(std::thread::hardware_concurrency());
}

VulkanTextureManager::~VulkanTextureManager()
{
	// Wait for all threads to finish
	m_ThreadPool->stop();

	// Cleanup all cached textures
	for (auto const& [path, texture] : m_TextureCache) {
		vkDestroyImageView(m_Device, texture.imageView, nullptr);
		vkDestroyImage(m_Device, texture.image, nullptr);
		vkFreeMemory(m_Device, texture.memory, nullptr);
	}
	vkDestroySampler(m_Device, m_DefaultSampler, nullptr);
}

void VulkanTextureManager::Initialize()
{
	// Create a default sampler for all textures
	m_DefaultSampler = m_CreateImage->createSampler(VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, true, 16.0f, VK_BORDER_COLOR_INT_OPAQUE_BLACK, false, VK_COMPARE_OP_ALWAYS, 1);
	if (m_DefaultSampler == VK_NULL_HANDLE) {
		throw std::runtime_error("Failed to create default sampler!");
	}

	// Pre-load default textures
	std::vector<std::pair<std::string, std::string>> defaultTextures = {
		{"default_white", "assets/textures/default.jpg"},
		{"default_normal", "assets/textures/defaultNormal.jpg"},
		{"default_specular", "assets/textures/defaultSpecular.png"}
	};
	auto futures = LoadTexturesBatch(defaultTextures);
	for (auto& future : futures) {
		future.get(); // Wait for defaults to load
	}
	TKC_CORE_INFO("VulkanTextureManager initialized with default textures.");
}

std::vector<std::shared_future<DescriptorTextureInfo>> VulkanTextureManager::LoadTexturesBatch(const std::vector<std::pair<std::string, std::string>>& requests)
{
	std::vector<std::shared_future<DescriptorTextureInfo>> futures;
	futures.reserve(requests.size());

	for (const auto& request : requests) {
		const auto& type = request.first;
		const auto& path = request.second;

		// Lock to ensure exclusive access to shared resources
		std::lock_guard<std::mutex> lock(m_LoadingMutex);

		// 1. Check if the texture is already in the cache
		if (m_TextureCache.count(path)) {
			auto& cachedTex = m_TextureCache.at(path);
			cachedTex.refCount++;

			std::promise<DescriptorTextureInfo> promise;
			promise.set_value({ VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, cachedTex.sampler, cachedTex.imageView });
			futures.push_back(promise.get_future().share());
			continue;
		}

		// 2. Check if the texture is currently being loaded
		if (m_LoadingTextures.count(path)) {
			futures.push_back(m_LoadingTextures.at(path));
			continue;
		}

		// 3. If not loaded or loading, enqueue a new loading task
		auto promise = std::make_shared<std::promise<DescriptorTextureInfo>>();
		std::shared_future<DescriptorTextureInfo> future = promise->get_future();

		m_LoadingTextures[path] = future;
		futures.push_back(future);

		m_ThreadPool->enqueue(&VulkanTextureManager::LoadAndUploadTexture, this, path, type, std::move(promise));
	}

	TKC_CORE_INFO("Creating {} futures", futures.size());
	return futures;
}

void VulkanTextureManager::CleanupUnusedTextures()
{
	std::lock_guard<std::mutex> lock(m_CacheMutex);
	if (m_TextureCache.empty()) {
		return;
	}

	for (auto it = m_TextureCache.begin(); it != m_TextureCache.end();) {
		if (it->second.refCount == 0) {
			vkDestroyImageView(m_Device, it->second.imageView, nullptr);
			vkDestroyImage(m_Device, it->second.image, nullptr);
			vkFreeMemory(m_Device, it->second.memory, nullptr);
			it = m_TextureCache.erase(it);
		}
		else {
			++it;
		}
	}
}

void VulkanTextureManager::LoadAndUploadTexture(std::string path, std::string type, std::shared_ptr<std::promise<DescriptorTextureInfo>> promise)
{
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	bool success = (pixels != nullptr);

	if (!success) {
		TKC_CORE_WARN("Failed to load texture image: {}. Using default.", path);
		std::string defaultPath;
		if (type == "normal") {
			defaultPath = "assets/textures/defaultNormal.jpg";
		}
		else if (type == "specular") {
			defaultPath = "assets/textures/defaultSpecular.png";
		}
		else {
			defaultPath = "assets/textures/default.jpg";
		}
		pixels = stbi_load(defaultPath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		if (!pixels) {
			promise->set_exception(std::make_exception_ptr(std::runtime_error("Failed to load default texture!")));
			return;
		}
	}

	VkDeviceSize imageSize = static_cast<VkDeviceSize>(texWidth) * texHeight * 4;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	m_Buffer->createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(m_Device, stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(m_Device, stagingBufferMemory);
	stbi_image_free(pixels);

	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
	m_CreateImage->createImage(texWidth, texHeight, 1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory, 1, 0);

	m_Buffer->transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, 0);
	m_Buffer->copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 0);
	m_Buffer->transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, 0);

	vkDestroyBuffer(m_Device, stagingBuffer, nullptr);
	vkFreeMemory(m_Device, stagingBufferMemory, nullptr);

	VkImageView textureImageView = m_CreateImage->createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, 1);
	
	TKC_CORE_INFO("Texture loaded: {} - Success: {}", path, success);

	{
		std::lock_guard<std::mutex> lock(m_CacheMutex);
		
		if (m_TextureCache.count(path)) {
			vkDestroyImageView(m_Device, textureImageView, nullptr);
			vkDestroyImage(m_Device, textureImage, nullptr);
			vkFreeMemory(m_Device, textureImageMemory, nullptr);

			auto& cachedTex = m_TextureCache.at(path);
			cachedTex.refCount++;
			promise->set_value({ VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, cachedTex.sampler, cachedTex.imageView });
		}
		else {
			CachedTexture newCachedTexture;
			newCachedTexture.image = textureImage;
			newCachedTexture.memory = textureImageMemory;
			newCachedTexture.imageView = textureImageView;
			newCachedTexture.sampler = m_DefaultSampler;
			newCachedTexture.refCount = 1;
			m_TextureCache[path] = newCachedTexture;
			promise->set_value({ VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_DefaultSampler, textureImageView });
		}
	}
	
	{
		std::lock_guard<std::mutex> lock(m_LoadingMutex);
		m_LoadingTextures.erase(path);
	}
}

} 