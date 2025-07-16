#pragma once

#include "tkcpch.h"

#include <vulkan/vulkan.h>

#include <string>
#include <vector>
#include <unordered_map>
#include <future>
#include <mutex>

#include "VulkanBuffer.h"
#include "VulkanCreateImage.h"
#include "Tokucu/Core/ThreadPool.h"

namespace Tokucu {
	
	struct DescriptorTextureInfo; // Forward-declaration

	struct CachedTexture {
		VkImage        image = VK_NULL_HANDLE;
		VkDeviceMemory memory = VK_NULL_HANDLE;
		VkImageView    imageView = VK_NULL_HANDLE;
		VkSampler      sampler = VK_NULL_HANDLE;
		uint32_t       refCount = 0;
	};

	class VulkanTextureManager {
	public:
		VulkanTextureManager(VkDevice device, VkPhysicalDevice physicalDevice, VulkanCreateImage* createImage, VulkanBuffer* buffer);
		~VulkanTextureManager();

		void Initialize();
		std::vector<std::shared_future<DescriptorTextureInfo>> LoadTexturesBatch(const std::vector<std::pair<std::string, std::string>>& requests);
		void CleanupUnusedTextures();

		VkSampler GetTextureSampler() const { return m_DefaultSampler; }

	private:
		void LoadAndUploadTexture(std::string path, std::string type, std::shared_ptr<std::promise<DescriptorTextureInfo>> promise);

		VkDevice         m_Device;
		VkPhysicalDevice m_PhysicalDevice;
		VulkanCreateImage* m_CreateImage;
		VulkanBuffer* m_Buffer;

		std::unique_ptr<ThreadPool> m_ThreadPool;
		std::unordered_map<std::string, CachedTexture> m_TextureCache;
		std::unordered_map<std::string, std::shared_future<DescriptorTextureInfo>> m_LoadingTextures;
		std::mutex m_CacheMutex;
		std::mutex m_LoadingMutex;

		VkSampler m_DefaultSampler = VK_NULL_HANDLE;
	};
} 