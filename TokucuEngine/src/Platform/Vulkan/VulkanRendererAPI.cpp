#include "tkcpch.h"
#include <cstddef>

#include "VulkanRendererAPI.h"

#include <set>
#include <filesystem>

#include "Tokucu/Renderer/Camera.h"
#include "Tokucu/Renderer/FBXLoader.h"

#include "VulkanSwapChain.h"
#include "VulkanCreateImage.h"
#include "VulkanFramebuffer.h"
#include "VulkanRenderPass.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanBuffer.h"
#include "VulkanTextureManager.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

namespace Tokucu {
	VulkanRendererAPI::VulkanRendererAPI()
	{
		
	}

	void VulkanRendererAPI::Init(const std::shared_ptr<Window>& window)
	{
		glfwWindow = static_cast<GLFWwindow*>(window->GetNativeWindow());
		if (!glfwWindow) {
			throw std::runtime_error("GLFW window is not initialized!");
		}
		m_VulkanCore = std::make_unique<VulkanCore>(glfwWindow);
		if (!m_VulkanCore) {
			throw std::runtime_error("Failed to create VulkanCore instance!");
		}
		//CORE abstraction
		instance = m_VulkanCore->getInstance();
		device = m_VulkanCore->getDevice();
		physicalDevice = m_VulkanCore->getPhysicalDevice();
		graphicsQueue = m_VulkanCore->getGraphicsQueue();
		presentQueue = m_VulkanCore->getPresentQueue();
		surface = m_VulkanCore->getSurface();
		msaaSamples = m_VulkanCore->getMaxUsableSampleCount();
		MAX_FRAMES_IN_FLIGHT = m_VulkanCore->getMaxFramesInFlight();
		//CORE abstraction done


		m_VulkanCreateImage = std::make_unique<VulkanCreateImage>(device, physicalDevice);
		m_VulkanGraphicsPipeline = std::make_unique<VulkanGraphicsPipeline>(m_VulkanCore.get(), device, physicalDevice);
		m_VulkanBuffer = std::make_unique<VulkanBuffer>(m_VulkanCore.get(), physicalDevice, device);
		m_VulkanFramebuffer = std::make_unique<VulkanFramebuffer>(device);
		m_VulkanSwapChain = std::make_unique<VulkanSwapChain>(m_VulkanCore.get(), m_VulkanFramebuffer.get(), physicalDevice, device, surface);
		m_VulkanRenderPass = std::make_unique<VulkanRenderPass>(device, physicalDevice);
		m_VulkanTextureManager = std::make_unique<VulkanTextureManager>(device, physicalDevice, m_VulkanCreateImage.get(), m_VulkanBuffer.get());

		m_VulkanBuffer->createCommandPool(); // <-- Move this up before any image transitions
		m_VulkanTextureManager->Initialize(); // Initialize texture manager after command pool

		createRenderPass();
		registerPipeline();
		createGraphicsPipeline();
		createShadowFramebuffer();
		
		createObject();

		createTextureSampler();
		createTextureImage();

		registerModel();

		createUniformBuffers();

		createDescriptorPool();
		createDescriptorSets();

		m_VulkanBuffer->createCommandBuffers();
		m_VulkanCore->createSyncObjects();
		
		// Cache object lists for performance optimization
		CacheObjectLists();
		
		// Offscreen resources are now created inside createShadowFramebuffer()
	}
	void VulkanRendererAPI::SetClearColor(const glm::vec4& color)
	{
	}
	void VulkanRendererAPI::Clear()
	{
		// Ensure all operations are complete before cleanup
		vkDeviceWaitIdle(device);

		// Wait for all fences to be signaled before destroying command buffers
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			if (m_VulkanCore && m_VulkanCore->getInFlightFences().size() > i) {
				vkWaitForFences(device, 1, &m_VulkanCore->getInFlightFences()[i], VK_TRUE, UINT64_MAX);
			}
		}

		// Destroy images, image views, and free memory
		auto destroyImageSafe = [&](VkImage& img, VkDeviceMemory& mem) {
			if (img != VK_NULL_HANDLE) {
				vkDestroyImage(device, img, nullptr);
				img = VK_NULL_HANDLE;
			}
			if (mem != VK_NULL_HANDLE) {
				vkFreeMemory(device, mem, nullptr);
				mem = VK_NULL_HANDLE;
			}
			};
		auto destroyImageViewSafe = [&](VkImageView& view) {
			if (view != VK_NULL_HANDLE) {
				vkDestroyImageView(device, view, nullptr);
				view = VK_NULL_HANDLE;
			}
			};
		auto destroySamplerSafe = [&](VkSampler& sampler) {
			if (sampler != VK_NULL_HANDLE) {
				vkDestroySampler(device, sampler, nullptr);
				sampler = VK_NULL_HANDLE;
			}
			};

		destroyImageViewSafe(shadowImageView);
		destroyImageSafe(shadowImage, shadowImageMemory);
		destroyImageViewSafe(HDRCubeView);
		destroyImageSafe(HDRCubeImage, HDRCubeImageMemory);
		for (auto& view : prefilterMapTempViews) destroyImageViewSafe(view);
		prefilterMapTempViews.clear();

		for (auto& view : prefilterMapTempViews) {
			if (view != VK_NULL_HANDLE) {
				vkDestroyImageView(device, view, nullptr);
				view = VK_NULL_HANDLE;
			}
		}
		prefilterMapTempViews.clear();

		destroyImageViewSafe(prefilterMapView);
		destroyImageSafe(prefilterMapImage, prefilterMapImageMemory);
		destroyImageViewSafe(BRDFImageView);
		destroyImageSafe(BRDFImage, BRDFImageMemory);
		destroyImageViewSafe(skyboxImageView);
		destroyImageSafe(skyboxImage, skyboxImageMemory);
		destroyImageViewSafe(skyboxHDRImageView);
		destroyImageSafe(skyboxHDRImage, skyboxHDRImageMemory);
		destroyImageViewSafe(CubeConvolutionImageView);
		destroyImageSafe(CubeConvolutionImage, CubeConvolutionImageMemory);

		destroySamplerSafe(textureSampler);
		destroySamplerSafe(shadowSampler);
		destroySamplerSafe(prefilterTextureSampler);

		for (auto& texture : textureImages) {
			if (texture != VK_NULL_HANDLE) {
				vkDestroyImage(device, texture, nullptr);
				texture = VK_NULL_HANDLE;
			}
		}
		textureImages.clear();
		for (auto& textureMemory : textureImagesMemory) {
			if (textureMemory != VK_NULL_HANDLE) {
				vkFreeMemory(device, textureMemory, nullptr);
				textureMemory = VK_NULL_HANDLE;
			}
		}
		textureImagesMemory.clear();
		for (auto& textureView : textureImagesView) {
			destroyImageViewSafe(textureView);
		}
		textureImagesView.clear();

		// --- NEW: Off-screen / ImGui viewport resources ---
		destroyImageViewSafe(m_OffscreenColorImageView);
		destroyImageSafe(m_OffscreenColorImage, m_OffscreenColorImageMemory);
		destroyImageViewSafe(m_OffscreenResolveImageView);
		destroyImageSafe(m_OffscreenResolveImage, m_OffscreenResolveImageMemory);
		destroyImageViewSafe(m_OffscreenDepthImageView);
		destroyImageSafe(m_OffscreenDepthImage, m_OffscreenDepthImageMemory);

		if (m_OffscreenFramebuffer != VK_NULL_HANDLE) {
			vkDestroyFramebuffer(device, m_OffscreenFramebuffer, nullptr);
			m_OffscreenFramebuffer = VK_NULL_HANDLE;
		}
		if (m_OffscreenRenderPass != VK_NULL_HANDLE) {
			vkDestroyRenderPass(device, m_OffscreenRenderPass, nullptr);
			m_OffscreenRenderPass = VK_NULL_HANDLE;
		}
		if (m_OffscreenSampler != VK_NULL_HANDLE) {
			vkDestroySampler(device, m_OffscreenSampler, nullptr);
			m_OffscreenSampler = VK_NULL_HANDLE;
		}

		// Compatibility with legacy offscreen variables (if ever initialised)
		destroyImageViewSafe(m_OffscreenImageView);
		destroyImageSafe(m_OffscreenImage, m_OffscreenImageMemory);
		// --- END NEW ---

		// Framebuffers
		vkDestroyFramebuffer(device, shadowFrameBuffer, nullptr); shadowFrameBuffer = VK_NULL_HANDLE;
		vkDestroyFramebuffer(device, HDRCubeFrameBuffer, nullptr); HDRCubeFrameBuffer = VK_NULL_HANDLE;
		vkDestroyFramebuffer(device, CubeConvolutionFrameBuffer, nullptr); CubeConvolutionFrameBuffer = VK_NULL_HANDLE;
		vkDestroyFramebuffer(device, BRDFFrameBuffer, nullptr); BRDFFrameBuffer = VK_NULL_HANDLE;
		for (auto& fb : prefilterMapFramebuffers) {
			vkDestroyFramebuffer(device, fb, nullptr);
		}
		prefilterMapFramebuffers.clear();


		// Per-object cleanup (shared-buffer safe)
		std::set<VkBuffer> destroyedBuffers;
		std::set<VkDeviceMemory> destroyedMemories;
		for (auto& obj : Objects) {
			for (size_t j = 0; j < MAX_FRAMES_IN_FLIGHT; j++) {
				for (size_t u = 0; u < obj.uniformBuffers.size(); u++) {
					if (obj.uniformBuffers[u][j] != VK_NULL_HANDLE && destroyedBuffers.find(obj.uniformBuffers[u][j]) == destroyedBuffers.end()) {
						vkDestroyBuffer(device, obj.uniformBuffers[u][j], nullptr);
						destroyedBuffers.insert(obj.uniformBuffers[u][j]);
						obj.uniformBuffers[u][j] = VK_NULL_HANDLE;
					}
				}
				for (size_t u = 0; u < obj.uniformBuffersMemory.size(); u++) {
					if (obj.uniformBuffersMemory[u][j] != VK_NULL_HANDLE && destroyedMemories.find(obj.uniformBuffersMemory[u][j]) == destroyedMemories.end()) {
						vkFreeMemory(device, obj.uniformBuffersMemory[u][j], nullptr);
						destroyedMemories.insert(obj.uniformBuffersMemory[u][j]);
						obj.uniformBuffersMemory[u][j] = VK_NULL_HANDLE;
					}
				}
			}
			// Vertex and index buffers (shared-buffer safe)
			if (obj.vertexBuffer != VK_NULL_HANDLE && destroyedBuffers.find(obj.vertexBuffer) == destroyedBuffers.end()) {
				vkDestroyBuffer(device, obj.vertexBuffer, nullptr);
				destroyedBuffers.insert(obj.vertexBuffer);
				obj.vertexBuffer = VK_NULL_HANDLE;
			}
			if (obj.vertexBufferMemory != VK_NULL_HANDLE && destroyedMemories.find(obj.vertexBufferMemory) == destroyedMemories.end()) {
				vkFreeMemory(device, obj.vertexBufferMemory, nullptr);
				destroyedMemories.insert(obj.vertexBufferMemory);
				obj.vertexBufferMemory = VK_NULL_HANDLE;
			}
			if (obj.indexBuffer != VK_NULL_HANDLE && destroyedBuffers.find(obj.indexBuffer) == destroyedBuffers.end()) {
				vkDestroyBuffer(device, obj.indexBuffer, nullptr);
				destroyedBuffers.insert(obj.indexBuffer);
				obj.indexBuffer = VK_NULL_HANDLE;
			}
			if (obj.indexBufferMemory != VK_NULL_HANDLE && destroyedMemories.find(obj.indexBufferMemory) == destroyedMemories.end()) {
				vkFreeMemory(device, obj.indexBufferMemory, nullptr);
				destroyedMemories.insert(obj.indexBufferMemory);
				obj.indexBufferMemory = VK_NULL_HANDLE;
			}
			// Descriptor sets are freed with the pool
			obj.uniformBuffers.clear();
			obj.uniformBuffersMemory.clear();
			obj.uniformBuffersMapped.clear();
			obj.descriptorSets.clear();
		}
		Objects.clear();

		// Pipelines
		for (auto& pipeline : Pipelines) {
			if (pipeline->pipeline != VK_NULL_HANDLE)
				vkDestroyPipeline(device, pipeline->pipeline, nullptr);
			if (pipeline->pipelineLayout != VK_NULL_HANDLE)
				vkDestroyPipelineLayout(device, pipeline->pipelineLayout, nullptr);
			if (pipeline->descriptorSetLayout != VK_NULL_HANDLE)
				vkDestroyDescriptorSetLayout(device, pipeline->descriptorSetLayout, nullptr);
			if (pipeline->descriptorPool != VK_NULL_HANDLE)
				vkDestroyDescriptorPool(device, pipeline->descriptorPool, nullptr);
		}
		Pipelines.clear();

		// Render passes
		vkDestroyRenderPass(device, shadowRenderPass, nullptr); shadowRenderPass = VK_NULL_HANDLE;
		vkDestroyRenderPass(device, renderPassHDR, nullptr); renderPassHDR = VK_NULL_HANDLE;
		vkDestroyRenderPass(device, BRDFRenderPass, nullptr); BRDFRenderPass = VK_NULL_HANDLE;

		// Cleanup ImGui Vulkan resources
		if (imGuiInitialized) {
			ImGui_ImplVulkan_Shutdown();
			imGuiInitialized = false;
		}
		if (imGuiDescriptorPool != VK_NULL_HANDLE) {
			vkDestroyDescriptorPool(device, imGuiDescriptorPool, nullptr);
			imGuiDescriptorPool = VK_NULL_HANDLE;
		}



		// Reset smart pointers for Vulkan objects (calls destructors)
		// (Smart pointer resets were moved earlier to ensure dependent resources are cleaned up before the device is destroyed.)

		// Clear all device references after all Vulkan objects are destroyed
		device = VK_NULL_HANDLE;
		physicalDevice = VK_NULL_HANDLE;
		graphicsQueue = VK_NULL_HANDLE;
		presentQueue = VK_NULL_HANDLE;
		surface = VK_NULL_HANDLE;

		TKC_CORE_INFO("Cleanup Completed");
	}
	void VulkanRendererAPI::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
	{
	}
	void VulkanRendererAPI::Render()
	{
		// Offscreen resources are now created during initialization, no need to create them here

		vkWaitForFences(device, 1, &m_VulkanCore->getInFlightFences()[currentFrame], VK_TRUE, UINT64_MAX);

		uint32_t imageIndex;

		VkResult result = vkAcquireNextImageKHR(device, m_VulkanSwapChain->getSwapChain(), UINT64_MAX, m_VulkanCore->getImageAvailableSemaphores()[currentFrame], VK_NULL_HANDLE, &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			m_VulkanSwapChain->recreateSwapChain();
			createShadowFramebuffer();
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}
		updateUniformBuffer(currentFrame);

		vkResetFences(device, 1, &m_VulkanCore->getInFlightFences()[currentFrame]);

		std::vector<VkCommandBuffer> commandBuffers = m_VulkanBuffer->getCommandBuffers();
		vkResetCommandBuffer(commandBuffers[currentFrame], 0);
		recordCommandBuffer(commandBuffers[currentFrame], imageIndex, currentFrame);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { m_VulkanCore->getImageAvailableSemaphores()[currentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

		VkSemaphore signalSemaphores[] = { m_VulkanCore->getRenderFinishedSemaphores()[currentFrame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, m_VulkanCore->getInFlightFences()[currentFrame]) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit draw command buffer!");
		}

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { m_VulkanSwapChain->getSwapChain() };

		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr; // Optional

		result = vkQueuePresentKHR(presentQueue, &presentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
			framebufferResized = false;
			m_VulkanSwapChain->recreateSwapChain();
			createShadowFramebuffer();
		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image!");
		}
		
		// Mark offscreen resources as created after first successful frame
		if (!m_OffscreenResourcesCreated) {
			m_OffscreenResourcesCreated = true;
		}
		
		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}
	
	void VulkanRendererAPI::initImGui()
	{
		TKC_CORE_INFO("Initializing ImGui Vulkan backend...");
		
		// Verify offscreen resources are valid (they should already be created in Init())
		if (m_OffscreenResolveImageView == VK_NULL_HANDLE) {
			TKC_CORE_ERROR("Offscreen resolve image view is null! Cannot initialize ImGui.");
			return;
		}
		
		TKC_CORE_INFO("Offscreen resolve image view: {}", (ImU64)m_OffscreenResolveImageView);
		TKC_CORE_INFO("Offscreen extent: {}x{}", m_OffscreenExtent.width, m_OffscreenExtent.height);
		
		// Create descriptor set layout for ImGui texture
		VkDescriptorSetLayoutBinding binding = {};
		binding.binding = 0;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		binding.descriptorCount = 1;
		binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		binding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &binding;

		if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &m_ImGuiDescriptorSetLayout) != VK_SUCCESS) {
			TKC_CORE_ERROR("Failed to create ImGui descriptor set layout!");
			return;
		}
		TKC_CORE_INFO("ImGui descriptor set layout created: {}", (ImU64)m_ImGuiDescriptorSetLayout);

		// Create descriptor pool for ImGui with more comprehensive sizes
		VkDescriptorPoolSize pool_sizes[] = {
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 100 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 100 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 100 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 100 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 100 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 100 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 100 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 100 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 100 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 100 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 100 }
		};

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		poolInfo.maxSets = 100 * 11;  // 11 is the size of pool_sizes array
		poolInfo.poolSizeCount = 11; // 11 is the size of pool_sizes array
		poolInfo.pPoolSizes = pool_sizes;

		if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &imGuiDescriptorPool) != VK_SUCCESS) {
			TKC_CORE_ERROR("Failed to create ImGui descriptor pool!");
			return;
		}
		TKC_CORE_INFO("ImGui descriptor pool created: {}", (ImU64)imGuiDescriptorPool);

		// Allocate descriptor set
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = imGuiDescriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &m_ImGuiDescriptorSetLayout;

		if (vkAllocateDescriptorSets(device, &allocInfo, &m_ImGuiDescriptorSet) != VK_SUCCESS) {
			TKC_CORE_ERROR("Failed to allocate ImGui descriptor set!");
			return;
		}
		TKC_CORE_INFO("ImGui descriptor set allocated: {}", (ImU64)m_ImGuiDescriptorSet);
		
		// Verify descriptor set was allocated
		if (m_ImGuiDescriptorSet == VK_NULL_HANDLE) {
			TKC_CORE_ERROR("ImGui descriptor set is null after allocation!");
			return;
		}

		// Create sampler for ImGui texture
		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.maxAnisotropy = 1.0f;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		if (vkCreateSampler(device, &samplerInfo, nullptr, &m_ImGuiSampler) != VK_SUCCESS) {
			TKC_CORE_ERROR("Failed to create ImGui sampler!");
			return;
		}
		TKC_CORE_INFO("ImGui sampler created: {}", (ImU64)m_ImGuiSampler);

		// Update descriptor set with the offscreen resolve image view
		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = m_OffscreenResolveImageView;
		imageInfo.sampler = m_ImGuiSampler;

		VkWriteDescriptorSet descriptorWrite = {};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = m_ImGuiDescriptorSet;
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pImageInfo = &imageInfo;

		// Verify all required handles are valid before updating descriptor set
		if (m_ImGuiDescriptorSet == VK_NULL_HANDLE) {
			TKC_CORE_ERROR("Cannot update descriptor set - descriptor set is null!");
			return;
		}
		if (m_OffscreenResolveImageView == VK_NULL_HANDLE) {
			TKC_CORE_ERROR("Cannot update descriptor set - offscreen resolve image view is null!");
			return;
		}
		if (m_ImGuiSampler == VK_NULL_HANDLE) {
			TKC_CORE_ERROR("Cannot update descriptor set - ImGui sampler is null!");
			return;
		}
		
		vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
		TKC_CORE_INFO("ImGui descriptor set updated with offscreen resolve image view");

		// Initialize ImGui Vulkan backend
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = instance;
		init_info.PhysicalDevice = physicalDevice;
		init_info.Device = device;
		init_info.QueueFamily = m_VulkanCore->getGraphicsQueueFamily();
		init_info.Queue = graphicsQueue;
		init_info.PipelineCache = VK_NULL_HANDLE;
		init_info.DescriptorPool = imGuiDescriptorPool;
		init_info.RenderPass = m_VulkanSwapChain->getRenderPass();
		init_info.Subpass = 0;
		init_info.MinImageCount = m_VulkanSwapChain->getSwapChainImages().size();
		init_info.ImageCount = m_VulkanSwapChain->getSwapChainImages().size();
		init_info.MSAASamples = msaaSamples;
		init_info.Allocator = nullptr;
		init_info.CheckVkResultFn = nullptr;

		TKC_CORE_INFO("Initializing ImGui Vulkan backend with {} swapchain images", init_info.ImageCount);
		if (!ImGui_ImplVulkan_Init(&init_info)) {
			TKC_CORE_ERROR("Failed to initialize ImGui Vulkan backend!");
			return;
		}
		TKC_CORE_INFO("ImGui Vulkan backend initialized successfully");

		// Register the texture with ImGui now that it's initialized
		TKC_CORE_INFO("Registering offscreen resolve image view as ImGui texture...");
		VkDescriptorSet textureId = ImGui_ImplVulkan_AddTexture(m_ImGuiSampler, m_OffscreenResolveImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		if (textureId != VK_NULL_HANDLE) {
			m_ImGuiTextureId = (ImTextureID)textureId;
			TKC_CORE_INFO("ImGui texture registered successfully: {}", (ImU64)textureId);
		} else {
			TKC_CORE_ERROR("Failed to register ImGui texture! Using fallback descriptor set.");
			m_ImGuiTextureId = (ImTextureID)m_ImGuiDescriptorSet;
		}
		
		// Mark ImGui as initialized
		imGuiInitialized = true;
	}
	
	void VulkanRendererAPI::Resize(const std::shared_ptr<Window>& window)
	{
		glfwWindow = static_cast<GLFWwindow*>(window->GetNativeWindow());

		//CORE TEST
		m_VulkanCore->setGLFWWindow(glfwWindow);
		framebufferResized = true;
	}

	void VulkanRendererAPI::createRenderPass() {

		// Create the shadow render pass
		//VulkanRenderPass RenderPass(device, physicalDevice, VK_SAMPLE_COUNT_1_BIT, m_VulkanSwapChain->findDepthFormat());
		//swapRenderPass = m_VulkanSwapChain->getRenderPass();
		swapRenderPass = m_VulkanSwapChain->getRenderPass();

		//shadowRenderPass = RenderPass.createShadowRenderPass();
		shadowRenderPass = m_VulkanRenderPass->createShadowRenderPass();

		// Create the HDR render pass
		//renderPassHDR = RenderPass.createHDRRenderPass();
		renderPassHDR = m_VulkanRenderPass->createHDRRenderPass();

		// Create the BRDF LUT render pass
		//BRDFRenderPass = RenderPass.createBRDFRenderPass();
		BRDFRenderPass = m_VulkanRenderPass->createBRDFRenderPass();

	}

	void VulkanRendererAPI::registerPipeline()
	{
		///////////////////////////////////////// 
		// Pipeline creation
		/////////////////////////////////////////
		 //Default pipeline for PBR objects
		m_Pipeline = { "defaultPipeline","assets/shaders/vert.spv", "assets/shaders/frag.spv",{},
			{{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT},
			{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,VK_SHADER_STAGE_FRAGMENT_BIT},
			{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,VK_SHADER_STAGE_GEOMETRY_BIT},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT}
			},
			{{&transformBufferInfo,VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT},{&pointLightsInfo,VK_BUFFER_USAGE_STORAGE_BUFFER_BIT},{&shadowUBOInfo,VK_BUFFER_USAGE_STORAGE_BUFFER_BIT}},
			//{{&ambientInfo},{&diffuseInfo},{&specularInfo},{&normalInfo},{&shadowInfo},{&irradianceInfo},{&prefilterMapInfo},{&BRDFInfo}},
			8,
			{{sizeof(UniformBufferObject)},{sizeof(LightAttributes) * pointLights.size()},{sizeof(shadowUBO) * 2}},swapRenderPass,VK_CULL_MODE_BACK_BIT ,msaaSamples, true };
		Pipelines.push_back(&m_Pipeline);
		//Pipeline for point light object representives
		m_Pipeline2 = { "lightPipeline" ,"assets/shaders/vertPL.spv", "assets/shaders/fragPL.spv",{},
			{{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT},
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT}},
			{{&transformBufferInfo,VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT},{&colorBufferInfo,VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT}},
			//{},
			0,
			{{sizeof(UniformBufferObject)},{sizeof(ColorUniform)}},swapRenderPass ,VK_CULL_MODE_BACK_BIT,msaaSamples, true };
		Pipelines.push_back(&m_Pipeline2);
		//Pipeline for skybox rendering using 6 layered cubemap
		m_PipelineSkybox = { "skyboxPipeline" ,"assets/shaders/skyboxVert.spv", "assets/shaders/skyboxFrag.spv",{},
			{{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT}},
			{{&transformBufferInfo,VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT}},
			//{{&skyboxInfo} },
			1,
			{ {sizeof(UniformBufferObject)} },swapRenderPass,VK_CULL_MODE_FRONT_BIT,msaaSamples, true
		};
		Pipelines.push_back(&m_PipelineSkybox);
		//For Cubemap convulation will be usen in diffuse IBL
		m_PipelineCubeConv = { "CubeConvPipeline" ,"assets/shaders/cubemapConvVert.spv", "assets/shaders/cubemapConvFrag.spv","assets/shaders/cubemapConvGeom.spv",
			{{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT},
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_GEOMETRY_BIT},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT}},
			{{&transformBufferInfo,VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT},{&cubemapPosMatInfo,VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT}},
			//{{&cubeConvInfo} },
			1,
			{ {sizeof(UniformBufferObject)},{sizeof(glm::mat4) * 6} },renderPassHDR,VK_CULL_MODE_BACK_BIT, VK_SAMPLE_COUNT_1_BIT, true
		};
		Pipelines.push_back(&m_PipelineCubeConv);
		//HDR Image to cubemap conversion (equirectengular 2D to cube)
		m_PipelineSkyboxHDR = { "skyboxPipelineHDR" ,"assets/shaders/skyboxHDRVert.spv", "assets/shaders/skyboxHDRFrag.spv","assets/shaders/skyboxHDRGeom.spv",
			{{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT},
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_GEOMETRY_BIT},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT}},
			{{&transformBufferInfo,VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT},{&cubemapPosInfo,VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT}},
			//{{&skyboxHDRInfo}}, 
			1,
			{{sizeof(UniformBufferObject)},{sizeof(glm::mat4) * 6}},renderPassHDR,VK_CULL_MODE_BACK_BIT, VK_SAMPLE_COUNT_1_BIT, true
		};
		Pipelines.push_back(&m_PipelineSkyboxHDR);
		//Prefiltering the cubemap for specular IBL
		m_PipelinePrefilter = { "prefilterPipeline" ,"assets/shaders/prefilterShaderVert.spv", "assets/shaders/prefilterShaderFrag.spv","assets/shaders/prefilterShaderGeom.spv",
			{{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT},
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_GEOMETRY_BIT},
			//{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_FRAGMENT_BIT},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT}},
			{{&transformBufferInfo,VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT},{&prefilterPosInfo,VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT}},
			//{{&prefilterInfo}}, 
			1,
			{{sizeof(UniformBufferObject)},{sizeof(glm::mat4) * 6}},renderPassHDR,VK_CULL_MODE_BACK_BIT, VK_SAMPLE_COUNT_1_BIT, true
		};
		Pipelines.push_back(&m_PipelinePrefilter);
		//BRDF LUT for specular IBL
		m_PipelineBRDF = { "BRDFPipeline" ,"assets/shaders/BRDFVert.spv", "assets/shaders/BRDFFrag.spv",{},
			{{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT}},
			{{&transformBufferInfo,VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT}},
			//{},
			0,
			{{sizeof(UniformBufferObject)}},BRDFRenderPass,VK_CULL_MODE_NONE, VK_SAMPLE_COUNT_1_BIT, true
		};
		Pipelines.push_back(&m_PipelineBRDF);
		//Shadow mapping pipeline
		m_PipelineShadow = { "shadowPipeline","assets/shaders/shadowVert.spv", "assets/shaders/shadowFrag.spv","assets/shaders/shadowGeom.spv",
		{ { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,VK_SHADER_STAGE_FRAGMENT_BIT },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,VK_SHADER_STAGE_GEOMETRY_BIT },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT },
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT}
		},
		{ {&transformBufferInfo,VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT},{&pointLightsInfo,VK_BUFFER_USAGE_STORAGE_BUFFER_BIT},{&shadowUBOInfo,VK_BUFFER_USAGE_STORAGE_BUFFER_BIT} },
			//{{&ambientInfo},{&diffuseInfo},{&specularInfo},{&normalInfo},{&shadowInfo},{&irradianceInfo},{&prefilterMapInfo},{&BRDFInfo}},
			8,
			{ {sizeof(UniformBufferObject)},{sizeof(LightAttributes) * pointLights.size()},{sizeof(shadowUBO) * 2} },shadowRenderPass, VK_CULL_MODE_BACK_BIT,VK_SAMPLE_COUNT_1_BIT, false };
		Pipelines.push_back(&m_PipelineShadow);
	}

	void VulkanRendererAPI::createGraphicsPipeline() {
		for (auto& pipeline : Pipelines) {
			m_VulkanGraphicsPipeline->createDescriptorSetLayout(pipeline);
			m_VulkanGraphicsPipeline->createGraphicsPipeline(pipeline);
		}
	}

	void VulkanRendererAPI::createShadowFramebuffer() {
		// Framebuffer for Shadow Mapping
		m_VulkanCreateImage->createImage(shadowMapSize, shadowMapSize, 1, VK_SAMPLE_COUNT_1_BIT,
			VK_FORMAT_D32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			shadowImage, shadowImageMemory, 12, 16);
		
		
		shadowImageView = m_VulkanCreateImage->createImageView(shadowImage, VK_FORMAT_D32_SFLOAT, VK_IMAGE_VIEW_TYPE_CUBE_ARRAY, VK_IMAGE_ASPECT_DEPTH_BIT, 1, 0, 12);

		shadowFrameBuffer = m_VulkanFramebuffer->createFramebuffers(shadowRenderPass, { shadowImageView }, shadowMapSize, shadowMapSize, 12);

		// Framebuffer for HDR Cubemap
		m_VulkanCreateImage->createImage(1024, 1024, 1, VK_SAMPLE_COUNT_1_BIT,
			VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			HDRCubeImage, HDRCubeImageMemory, 6, 16);
		HDRCubeView = m_VulkanCreateImage->createImageView(HDRCubeImage, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_VIEW_TYPE_CUBE, VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, 6);

		HDRCubeFrameBuffer = m_VulkanFramebuffer->createFramebuffers(renderPassHDR, { HDRCubeView }, 1024, 1024, 6);

		// Framebuffer for Cube Convolution
		m_VulkanCreateImage->createImage(32, 32, 1, VK_SAMPLE_COUNT_1_BIT,
			VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			CubeConvolutionImage, CubeConvolutionImageMemory, 6, 16);
		CubeConvolutionImageView = m_VulkanCreateImage->createImageView(CubeConvolutionImage, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_VIEW_TYPE_CUBE, VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, 6);

		CubeConvolutionFrameBuffer = m_VulkanFramebuffer->createFramebuffers(renderPassHDR, { CubeConvolutionImageView }, 32, 32, 6);

		// Framebuffer for Prefiltered Cubemap
		m_VulkanCreateImage->createImage(512, 512, 1, VK_SAMPLE_COUNT_1_BIT,
			VK_FORMAT_R32G32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			BRDFImage, BRDFImageMemory, 1, 0);
		BRDFImageView = m_VulkanCreateImage->createImageView(BRDFImage, VK_FORMAT_R32G32_SFLOAT, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, 1);

		BRDFFrameBuffer = m_VulkanFramebuffer->createFramebuffers(BRDFRenderPass, { BRDFImageView }, 512, 512, 1);

		/*******************************
		 * Off-screen framebuffer (ImGui)
		 ******************************/
		// Clean up any previous off-screen resources first
		auto safeDestroyImage = [&](VkImage& img, VkDeviceMemory& mem) {
			if (img != VK_NULL_HANDLE) {
				vkDestroyImage(device, img, nullptr);
				img = VK_NULL_HANDLE;
			}
			if (mem != VK_NULL_HANDLE) {
				vkFreeMemory(device, mem, nullptr);
				mem = VK_NULL_HANDLE;
			}
		};
		auto safeDestroyImageView = [&](VkImageView& view) {
			if (view != VK_NULL_HANDLE) {
				vkDestroyImageView(device, view, nullptr);
				view = VK_NULL_HANDLE;
			}
		};
		if (m_OffscreenFramebuffer != VK_NULL_HANDLE) vkDestroyFramebuffer(device, m_OffscreenFramebuffer, nullptr);
		if (m_OffscreenRenderPass != VK_NULL_HANDLE) vkDestroyRenderPass(device, m_OffscreenRenderPass, nullptr);
		safeDestroyImageView(m_OffscreenColorImageView);
		safeDestroyImageView(m_OffscreenResolveImageView);
		safeDestroyImageView(m_OffscreenDepthImageView);
		safeDestroyImage(m_OffscreenColorImage, m_OffscreenColorImageMemory);
		safeDestroyImage(m_OffscreenResolveImage, m_OffscreenResolveImageMemory);
		safeDestroyImage(m_OffscreenDepthImage, m_OffscreenDepthImageMemory);

		// Obtain format / extent from current swap chain
		m_OffscreenFormat = m_VulkanSwapChain->getSwapChainImageFormat();
		m_OffscreenExtent = m_VulkanSwapChain->getSwapChainExtent();
		VkFormat depthFormat = m_VulkanSwapChain->findDepthFormat();

		if (m_OffscreenExtent.width != 0 && m_OffscreenExtent.height != 0) {
			// Color (MSAA)
			m_VulkanCreateImage->createImage(
				m_OffscreenExtent.width, m_OffscreenExtent.height, 1, msaaSamples, m_OffscreenFormat,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				m_OffscreenColorImage, m_OffscreenColorImageMemory, 1, 0);
			m_OffscreenColorImageView = m_VulkanCreateImage->createImageView(m_OffscreenColorImage, m_OffscreenFormat, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, 1);

			// Resolve (single-sampled, shader-read)
			m_VulkanCreateImage->createImage(
				m_OffscreenExtent.width, m_OffscreenExtent.height, 1, VK_SAMPLE_COUNT_1_BIT, m_OffscreenFormat,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				m_OffscreenResolveImage, m_OffscreenResolveImageMemory, 1, 0);
			m_OffscreenResolveImageView = m_VulkanCreateImage->createImageView(m_OffscreenResolveImage, m_OffscreenFormat, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, 1);

			// Transition resolve image so first use is valid for shader read when ImGui registers it
			m_VulkanBuffer->transitionImageLayout(m_OffscreenResolveImage, m_OffscreenFormat,
				VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, 0);

			// Depth (MSAA)
			m_VulkanCreateImage->createImage(
				m_OffscreenExtent.width, m_OffscreenExtent.height, 1, msaaSamples, depthFormat,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				m_OffscreenDepthImage, m_OffscreenDepthImageMemory, 1, 0);
			m_OffscreenDepthImageView = m_VulkanCreateImage->createImageView(m_OffscreenDepthImage, depthFormat, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_DEPTH_BIT, 1, 0, 1);

			// Render pass + framebuffer
			m_OffscreenRenderPass = m_VulkanRenderPass->createOffscreenRenderPass(msaaSamples, m_OffscreenFormat);
			std::vector<VkImageView> offscreenAttachments = { m_OffscreenColorImageView, m_OffscreenDepthImageView, m_OffscreenResolveImageView };
			m_OffscreenFramebuffer = m_VulkanFramebuffer->createFramebuffers(m_OffscreenRenderPass, offscreenAttachments, m_OffscreenExtent.width, m_OffscreenExtent.height, 1);

			m_OffscreenResourcesCreated = true;
		}

		// If ImGui was already initialized, refresh its descriptor to use the new resolve image
		if (imGuiInitialized)
		{
			RefreshImGuiOffscreenTexture();
		}
	}

	void VulkanRendererAPI::createObject() {
		BufferData vertexData;
		BufferData indexData;

		vertexData = m_VulkanBuffer->createVertexBuffer(cubeVertices);
		indexData = m_VulkanBuffer->createIndexBuffer(cubeIndices);
		BufferData planeVertexData = m_VulkanBuffer->createVertexBuffer(secondVertices);
		BufferData planeIndexData = m_VulkanBuffer->createIndexBuffer(secondIndices);

		VulkanObject base = { "base", cubeVertices,cubeIndices, vertexData.buffer, vertexData.memory,indexData.buffer, indexData.memory, &m_Pipeline,true,std::nullopt };
		Objects.push_back(base);
		
		//VulkanObject top = { "top", cubeVertices,cubeIndices, vertexData.buffer, vertexData.memory,indexData.buffer, indexData.memory, getBindingDescription() , getAttributeDescriptions(),&m_Pipeline,true,std::nullopt };
		//Objects.push_back(top);
		//
		//VulkanObject right = { "right", cubeVertices,cubeIndices, vertexData.buffer, vertexData.memory,indexData.buffer, indexData.memory, getBindingDescription() , getAttributeDescriptions(),&m_Pipeline,true,std::nullopt };
		//Objects.push_back(right);
		//
		//VulkanObject left = { "left", cubeVertices,cubeIndices, vertexData.buffer, vertexData.memory,indexData.buffer, indexData.memory, getBindingDescription() , getAttributeDescriptions(),&m_Pipeline,true,std::nullopt };
		//Objects.push_back(left);
		//
		//VulkanObject front = { "front", cubeVertices,cubeIndices, vertexData.buffer, vertexData.memory,indexData.buffer, indexData.memory, getBindingDescription() , getAttributeDescriptions(),&m_Pipeline,true,std::nullopt };
		//Objects.push_back(front);
		//
		//VulkanObject back = { "back", cubeVertices,cubeIndices, vertexData.buffer, vertexData.memory,indexData.buffer, indexData.memory, getBindingDescription() , getAttributeDescriptions(),&m_Pipeline,true,std::nullopt };
		//Objects.push_back(back);

		VulkanObject pointLight1 = { "pointLight1", cubeVertices,cubeIndices, vertexData.buffer, vertexData.memory,indexData.buffer, indexData.memory//, getBindingDescription() , getAttributeDescriptions()
			,&m_Pipeline2,false,std::nullopt };
		Objects.push_back(pointLight1);

		VulkanObject pointLight2 = { "pointLight2", cubeVertices,cubeIndices, vertexData.buffer, vertexData.memory,indexData.buffer, indexData.memory//, getBindingDescription() , getAttributeDescriptions()
			,&m_Pipeline2,false,std::nullopt };
		Objects.push_back(pointLight2);

		VulkanObject skybox = { "skyBox", cubeVertices,cubeIndices, vertexData.buffer, vertexData.memory,indexData.buffer, indexData.memory//, getBindingDescription() , getAttributeDescriptions()
			,&m_PipelineSkybox,false,std::nullopt };
		Objects.push_back(skybox);

		VulkanObject skyboxHDR = { "skyBoxHDR", cubeVertices,cubeIndices, vertexData.buffer, vertexData.memory,indexData.buffer, indexData.memory//, getBindingDescription() , getAttributeDescriptions()
			,&m_PipelineSkyboxHDR,false,std::nullopt,
		{{"skyboxHDR","assets/textures/overcast_soil_puresky_4k.hdr"}} };
		Objects.push_back(skyboxHDR);

		VulkanObject cubeConv = { "cubeConv", cubeVertices,cubeIndices, vertexData.buffer, vertexData.memory,indexData.buffer, indexData.memory//, getBindingDescription() , getAttributeDescriptions()
			,&m_PipelineCubeConv,false,std::nullopt };
		Objects.push_back(cubeConv);

		VulkanObject prefilterCube = { "prefilterCube", cubeVertices,cubeIndices, vertexData.buffer, vertexData.memory,indexData.buffer, indexData.memory//, getBindingDescription() , getAttributeDescriptions()
			,&m_PipelinePrefilter,false,std::nullopt };
		Objects.push_back(prefilterCube);

		VulkanObject BRDFLud = { "BRDFLud", secondVertices,secondIndices, planeVertexData.buffer, planeVertexData.memory,planeIndexData.buffer, planeIndexData.memory//,getBindingDescription() , getAttributeDescriptions()
			,&m_PipelineBRDF,false,std::nullopt };
		Objects.push_back(BRDFLud);
	}

	void VulkanRendererAPI::registerModel()
	{
		createModel("Shotgun", "assets/models/shotgun/ShotgunTri.fbx", "assets/textures/Shotgun/");
		//createModel("Helmet", "assets/models/Helmet/sci_fi_space_helmet_by_aliashasim.fbx", "assets/textures/Helmet/");
		createModel("Glock", "assets/models/Glock/MDL_Glock.fbx", "assets/textures/Glock/");
	}

	void VulkanRendererAPI::createModel(std::string modelName, std::string modelLocation, std::string textureLocation) {
		std::unique_ptr<FBXLoader> loader = std::make_unique<FBXLoader>(modelLocation.c_str());
		FBXModelData model = loader->getModelData();

		const bool useDefaultTextures = textureLocation.empty();

		// Containers for optional asynchronous texture loading
		std::vector<std::pair<std::string, std::string>> allTextureRequests;
		std::vector<std::vector<std::pair<std::string, std::string>>> meshTextureRequests;

		TKC_CORE_INFO("Processing model: {} with {} meshes", modelName, model.meshes.size());

		// If we are NOT using default textures, prepare texture request vectors
		if (!useDefaultTextures) {
			for (int i = 0; i < model.meshes.size(); i++) {
				auto& mesh = model.meshes[i];
				std::string materialName = mesh.materialName;

				std::vector<std::pair<std::string, std::string>> materialTextureLocations = {
					{"ambient", textureLocation + materialName + "Base.png"},
					{"diffuse", textureLocation + materialName + "Base.png"},
					{"specular", textureLocation + materialName + "Specular.png"},
					{"normal", textureLocation + materialName + "Normal.png"}
				};

				for (const auto& [type, path] : materialTextureLocations) {
					TKC_CORE_INFO("Texture request - Type: {}, Path: {}", type, path);
				}

				allTextureRequests.insert(allTextureRequests.end(), materialTextureLocations.begin(), materialTextureLocations.end());
				meshTextureRequests.push_back(materialTextureLocations);
			}
		}

		// Start asynchronous loading if needed
		std::vector<std::shared_future<DescriptorTextureInfo>> textureFutures;
		if (!useDefaultTextures && !allTextureRequests.empty()) {
			TKC_CORE_INFO("Total texture requests: {}", allTextureRequests.size());
			textureFutures = m_VulkanTextureManager->LoadTexturesBatch(allTextureRequests);
			TKC_CORE_INFO("Total texture futures: {}", textureFutures.size());
		}

		// Create mesh objects
		size_t futureIndex = 0;
		for (int i = 0; i < model.meshes.size(); i++) {
			auto& mesh = model.meshes[i];
			BufferData vertexData = m_VulkanBuffer->createVertexBuffer(mesh.vertices);
			BufferData indexData = m_VulkanBuffer->createIndexBuffer(mesh.indices);

			VulkanObject obj{}; // use default initializers (includes default textures)
			obj.name = modelName;
			obj.vertexData = mesh.vertices;
			obj.indexData = mesh.indices;
			obj.vertexBuffer = vertexData.buffer;
			obj.vertexBufferMemory = vertexData.memory;
			obj.indexBuffer = indexData.buffer;
			obj.indexBufferMemory = indexData.memory;
			obj.pipeline = &m_Pipeline;
			obj.b_PBR = true;
			obj.modelLocation = modelLocation;

			if (!useDefaultTextures) {
				obj.textureLocations = meshTextureRequests[i];
			}

			Objects.push_back(obj);
		}

		// If we loaded custom textures, wait for futures and assign DescriptorTextureInfo
		if (!useDefaultTextures) {
			TKC_CORE_INFO("Assigning textures to meshes using loaded images...");

			for (int i = 0; i < model.meshes.size(); i++) {
				auto& obj = Objects[Objects.size() - model.meshes.size() + i];

				for (const auto& [type, path] : meshTextureRequests[i]) {
					if (futureIndex >= textureFutures.size()) {
						TKC_CORE_ERROR("Texture future index out of bounds: {} >= {}", futureIndex, textureFutures.size());
						break;
					}

					DescriptorTextureInfo textureInfo = textureFutures[futureIndex++].get();
					obj.texturesInfo.push_back(textureInfo);

					if (type == "normal") {
						obj.texturesInfo.push_back({ VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, shadowSampler, shadowImageView });
						obj.texturesInfo.push_back({ VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_VulkanTextureManager->GetTextureSampler(), CubeConvolutionImageView });
						obj.texturesInfo.push_back({ VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, prefilterTextureSampler, prefilterMapView });
						obj.texturesInfo.push_back({ VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_VulkanTextureManager->GetTextureSampler(), BRDFImageView });
					}
				}
			}
		}

		TKC_CORE_INFO("Model {} loaded (defaultTextures={}): {} objects now in scene", modelName, useDefaultTextures, Objects.size());

		// Periodic cleanup of unused textures (every 10 models)
		static int modelCount = 0;
		if (++modelCount % 10 == 0) {
			m_VulkanTextureManager->CleanupUnusedTextures();
		}
	}

	void VulkanRendererAPI::createTextureImage() {
		for (auto& object : Objects) {
			if (object.textureLocations.empty()) continue;
			if (object.name == "cubeConv") {

				DescriptorTextureInfo cubeConvInfo = { VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, textureSampler, HDRCubeView };
				object.texturesInfo.push_back(cubeConvInfo);
			}
			if (object.name == "prefilterCube")
			{
				//not using generate mipmap function , mipmapping manually with several FB and imageviews
				prefilterMipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(prefilterMapResolution, prefilterMapResolution)))) + 1;

				m_VulkanCreateImage->createImage(prefilterMapResolution, prefilterMapResolution, prefilterMipLevels, VK_SAMPLE_COUNT_1_BIT,
					VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
					VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
					prefilterMapImage, prefilterMapImageMemory, 6, 16);
				
			
				
				prefilterMapView = m_VulkanCreateImage->createImageView(prefilterMapImage, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_VIEW_TYPE_CUBE, VK_IMAGE_ASPECT_COLOR_BIT, prefilterMipLevels, 0, 6);

				// Patch any previously created objects that still hold a null prefilter map
				for (auto& prevObj : Objects) {
					for (auto& texInfo : prevObj.texturesInfo) {
						if (texInfo.imageView == VK_NULL_HANDLE && texInfo.sampler == prefilterTextureSampler) {
							texInfo.imageView = prefilterMapView;
						}
					}
				}

				DescriptorTextureInfo prefilterInfo = { VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ,textureSampler,HDRCubeView };
				object.texturesInfo.push_back(prefilterInfo);

			}
			

			// Create texture image for HDR skybox having one layer (equirectangular)
			if (object.name == "skyBoxHDR")
			{
				int texWidth, texHeight, texChannels;
				float* pixels = stbi_loadf(object.textureLocations[0].second.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
				if (!pixels) {
					throw std::runtime_error("Failed to load HDR equirectangular image!");
				}
				VkDeviceSize imageSize = texWidth * texHeight * sizeof(float) * 4;
				VkBuffer stagingBuffer;
				VkDeviceMemory stagingBufferMemory;
				m_VulkanBuffer->createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					stagingBuffer, stagingBufferMemory);
				void* data;
				vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
				memcpy(data, pixels, static_cast<size_t>(imageSize));
				vkUnmapMemory(device, stagingBufferMemory);
				stbi_image_free(pixels);

				m_VulkanCreateImage->createImage(texWidth, texHeight, 1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R32G32B32A32_SFLOAT,
					VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
					VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, skyboxHDRImage,
					skyboxHDRImageMemory, 1, 0);
				m_VulkanBuffer->transitionImageLayout(skyboxHDRImage, VK_FORMAT_R32G32B32A32_SFLOAT,
					VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, 0);
				m_VulkanBuffer->copyBufferToImage(stagingBuffer, skyboxHDRImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 0);
				m_VulkanBuffer->transitionImageLayout(skyboxHDRImage, VK_FORMAT_R32G32B32A32_SFLOAT,
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
					VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, 0);
				vkDestroyBuffer(device, stagingBuffer, nullptr);
				vkFreeMemory(device, stagingBufferMemory, nullptr);

				skyboxHDRImageView = m_VulkanCreateImage->createImageView(skyboxHDRImage, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, 1);
				DescriptorTextureInfo skyboxHDRInfo = { VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, textureSampler, skyboxHDRImageView };
				object.texturesInfo.push_back(skyboxHDRInfo);
				continue;
			}
			// Create texture image for skybox having 6 layers (cubemap)
			if (object.name == "skyBox")
			{
				int texWidth = 2048;  // Use consistent size for all faces
				int texHeight = 2048;

				m_VulkanCreateImage->createImage(texWidth, texHeight, 1, VK_SAMPLE_COUNT_1_BIT,
					VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
					VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
					skyboxImage, skyboxImageMemory, 6, 16);
				// Transition all layers to transfer destination
				m_VulkanBuffer->transitionImageLayout(skyboxImage, VK_FORMAT_R8G8B8A8_SRGB,
					VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					VK_IMAGE_ASPECT_COLOR_BIT, 6, 1, 0);
				// Modified loading loop for skybox 
				// Load all 6 faces of the skybox
				std::vector<std::string> skyboxFaces = {
					"assets/textures/skybox/right.jpg", "assets/textures/skybox/left.jpg", "assets/textures/skybox/top.jpg", "assets/textures/skybox/bottom.jpg", "assets/textures/skybox/front.jpg", "assets/textures/skybox/back.jpg"
				};
				// Allocate one reusable staging buffer (largest face)
				VkDeviceSize faceSize = texWidth * texHeight * 4; // 4 bytes per pixel (RGBA8)
				VkBuffer stagingBuffer;
				VkDeviceMemory stagingBufferMemory;
				m_VulkanBuffer->createBuffer(faceSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					stagingBuffer, stagingBufferMemory);

				for (int i = 0; i < 6; i++) {
					TKC_CORE_INFO("Loading skybox face {}: {}", i, skyboxFaces[i]);

					int w, h, ch;
					stbi_uc* pixels = stbi_load(skyboxFaces[i].c_str(), &w, &h, &ch, STBI_rgb_alpha);
					if (!pixels) {
						throw std::runtime_error("Failed to load skybox face: " + skyboxFaces[i]);
					}

					// Copy into the _same_ staging buffer (overwriting previous face)
					void* data;
					vkMapMemory(device, stagingBufferMemory, 0, faceSize, 0, &data);
					memcpy(data, pixels, static_cast<size_t>(faceSize));
					vkUnmapMemory(device, stagingBufferMemory);
					stbi_image_free(pixels);

					m_VulkanBuffer->copyBufferToImage(stagingBuffer, skyboxImage,
						static_cast<uint32_t>(w), static_cast<uint32_t>(h), i);
				}

				// Clean up staging buffer once after all faces are uploaded
				vkDestroyBuffer(device, stagingBuffer, nullptr);
				vkFreeMemory(device, stagingBufferMemory, nullptr);
				// After all faces are copied, transition to shader read layout
				m_VulkanBuffer->transitionImageLayout(skyboxImage, VK_FORMAT_R8G8B8A8_SRGB,
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
					VK_IMAGE_ASPECT_COLOR_BIT, 6, 1, 0);
				// Create the image view and sampler
				skyboxImageView = m_VulkanCreateImage->createImageView(skyboxImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_VIEW_TYPE_CUBE, VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, 6);
				DescriptorTextureInfo skyboxInfo = { VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, textureSampler, HDRCubeView };
				object.texturesInfo.push_back(skyboxInfo);
				// Skip the rest of the texture processing
				continue;
			}
			// Create texture image for regular maps
			// This part is now handled by VulkanTextureManager inside createModel
			if (object.texturesInfo.empty() && !object.textureLocations.empty()) {
				auto futures = m_VulkanTextureManager->LoadTexturesBatch(object.textureLocations);
				for (size_t idx = 0; idx < futures.size(); ++idx) {
					DescriptorTextureInfo texInfo = futures[idx].get();
					object.texturesInfo.push_back(texInfo);

					// Append extra PBR helper maps right after the normal map so the
					// descriptor bindings match the expected layout (total 8 samplers).
					if (object.textureLocations[idx].first == "normal") {
						object.texturesInfo.push_back({ VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, shadowSampler, shadowImageView });
						object.texturesInfo.push_back({ VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_VulkanTextureManager->GetTextureSampler(), CubeConvolutionImageView });
						object.texturesInfo.push_back({ VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, prefilterTextureSampler, prefilterMapView });
						object.texturesInfo.push_back({ VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_VulkanTextureManager->GetTextureSampler(), BRDFImageView });
					}
				}
			}
		}
	}

	void VulkanRendererAPI::createTextureSampler() {

		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(physicalDevice, &properties);

		// standard color sampler
		textureSampler = m_VulkanCreateImage->createSampler(
			VK_FILTER_LINEAR, VK_FILTER_LINEAR,
			VK_SAMPLER_ADDRESS_MODE_REPEAT,
			true,                                    // enable anisotropy
			properties.limits.maxSamplerAnisotropy, // use max available
			VK_BORDER_COLOR_INT_OPAQUE_BLACK,
			false, VK_COMPARE_OP_ALWAYS,
			mipLevels
		);

		// prefilter (environment) sampler
		prefilterTextureSampler = m_VulkanCreateImage->createSampler(
			VK_FILTER_LINEAR, VK_FILTER_LINEAR,
			VK_SAMPLER_ADDRESS_MODE_REPEAT,
			true,
			properties.limits.maxSamplerAnisotropy,
			VK_BORDER_COLOR_INT_OPAQUE_BLACK,
			false, VK_COMPARE_OP_ALWAYS,
			prefilterMipLevels
		);

		// shadow map PCF sampler
		shadowSampler = m_VulkanCreateImage->createSampler(
			VK_FILTER_LINEAR, VK_FILTER_LINEAR,
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
			false,  // no anisotropy needed
			1.0f,
			VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
			true, VK_COMPARE_OP_LESS_OR_EQUAL,
			1       // just base level
		);

	}

	void VulkanRendererAPI::createUniformBuffers() {
		for (auto& object : Objects) {
			m_VulkanBuffer->createUniformBuffers(object);

		}
	}

	void VulkanRendererAPI::createDescriptorPool() {
		for (auto& pipeline : Pipelines) {
			m_VulkanGraphicsPipeline->createDescriptorPool(pipeline, Objects.size());
		}
	}

	void VulkanRendererAPI::createDescriptorSets() {
		for (auto& object : Objects) {
			m_VulkanGraphicsPipeline->createDescriptorSets(&object);
		}
		TKC_CORE_INFO("Descriptor sets created for {} objects", Objects.size());
	}

	void VulkanRendererAPI::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, uint32_t currentImage) {

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		// Pre-compute clear values once
		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { {0.0f, 0.4f, 0.6f, 1.0f} };
		clearValues[1].depthStencil = { 1.0f, 0 };

		VkClearValue clearDepth{};
		clearDepth.depthStencil = { 1.0f, 0 };

		// === SHADOW PASS (Every frame - necessary for dynamic lighting) ===
		// Only render shadows if we have objects that cast shadows
		bool hasShadowCasters = false;
		for (const auto& object : Objects) {
			if (object.pipeline->name == "defaultPipeline") {
				hasShadowCasters = true;
				break;
			}
		}

		if (hasShadowCasters && m_PipelineShadow.pipeline != VK_NULL_HANDLE) {
			RenderPassInfo shadowRenderPassInfo = m_VulkanRenderPass->createRenderPassInfo(shadowRenderPass, shadowFrameBuffer, clearDepth, shadowMapSize);
			vkCmdBeginRenderPass(commandBuffer, &shadowRenderPassInfo.renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
			
			// Render shadow maps for both lights
			for (int i = 0; i < 2; i++) {
				lightIndexUBO indexUBO = {};
				indexUBO.lightIndex = i;
				
				for (auto& object : Objects) {
					if (object.pipeline->name == "defaultPipeline") {
						vkCmdPushConstants(commandBuffer, m_PipelineShadow.pipelineLayout, 
							VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 
							0, sizeof(lightIndexUBO), &indexUBO);
						m_VulkanGraphicsPipeline->renderObject(commandBuffer, &m_PipelineShadow, shadowRenderPassInfo, object, currentImage);
					}
				}
			}
			vkCmdEndRenderPass(commandBuffer);
		}

		// === ONE-TIME IBL PREPROCESSING (Only on first frame) ===
		if (!b_cubeconvulation) {
			// HDR to cubemap conversion
			RenderPassInfo eqToCubeRenderPassInfo = m_VulkanRenderPass->createRenderPassInfo(renderPassHDR, HDRCubeFrameBuffer, clearDepth, 1024);
			vkCmdBeginRenderPass(commandBuffer, &eqToCubeRenderPassInfo.renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
			
			for (auto& object : Objects) {
				if (object.pipeline->name == "skyboxPipelineHDR" && object.pipeline->pipeline != VK_NULL_HANDLE) {
					m_VulkanGraphicsPipeline->renderObject(commandBuffer, object.pipeline, eqToCubeRenderPassInfo, object, currentImage);
				}
			}
			vkCmdEndRenderPass(commandBuffer);

			// Cube convolution for diffuse IBL
			RenderPassInfo CubeConvRenderPassInfo = m_VulkanRenderPass->createRenderPassInfo(renderPassHDR, CubeConvolutionFrameBuffer, clearDepth, 32);
			vkCmdBeginRenderPass(commandBuffer, &CubeConvRenderPassInfo.renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
			
			for (auto& object : Objects) {
				if (object.pipeline->name == "CubeConvPipeline" && object.pipeline->pipeline != VK_NULL_HANDLE) {
					m_VulkanGraphicsPipeline->renderObject(commandBuffer, object.pipeline, CubeConvRenderPassInfo, object, currentImage);
				}
			}
			vkCmdEndRenderPass(commandBuffer);

			// BRDF LUT generation
			RenderPassInfo BRDFRenderPassInfo = m_VulkanRenderPass->createRenderPassInfo(BRDFRenderPass, BRDFFrameBuffer, clearDepth, 512);
			vkCmdBeginRenderPass(commandBuffer, &BRDFRenderPassInfo.renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
			
			for (auto& object : Objects) {
				if (object.pipeline->name == "BRDFPipeline" && object.pipeline->pipeline != VK_NULL_HANDLE) {
					m_VulkanGraphicsPipeline->renderObject(commandBuffer, object.pipeline, BRDFRenderPassInfo, object, currentImage);
				}
			}
			vkCmdEndRenderPass(commandBuffer);

			// Prefilter map generation (specular IBL)
			prefilterMipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(prefilterMapResolution, prefilterMapResolution)))) + 1;
			
			for (size_t mip = 0; mip < prefilterMipLevels; mip++) {
				uint32_t mipWidth = std::max(1u, static_cast<uint32_t>(prefilterMapResolution * std::pow(0.5f, mip)));
				uint32_t mipHeight = std::max(1u, static_cast<uint32_t>(prefilterMapResolution * std::pow(0.5f, mip)));

				// No manual barrier needed. The render pass will transition
				// UNDEFINED → COLOR_ATTACHMENT_OPTIMAL at subpass start and to
				// SHADER_READ_ONLY_OPTIMAL at the end (see renderPassHDR).
				VkImageView prefilterMapTempView = m_VulkanCreateImage->createImageView(prefilterMapImage, VK_FORMAT_R32G32B32A32_SFLOAT, 
					VK_IMAGE_VIEW_TYPE_CUBE, VK_IMAGE_ASPECT_COLOR_BIT, 1, mip, 6);
				
				VkFramebuffer prefilterMapFramebuffer = m_VulkanFramebuffer->createFramebuffers(renderPassHDR, { prefilterMapTempView }, mipWidth, mipHeight, 6);
				prefilterMapFramebuffers.push_back(prefilterMapFramebuffer);
				prefilterMapTempViews.push_back(prefilterMapTempView);

				RenderPassInfo prefilterRenderPassInfo = m_VulkanRenderPass->createRenderPassInfo(renderPassHDR, prefilterMapFramebuffer, clearDepth, mipWidth);
				vkCmdBeginRenderPass(commandBuffer, &prefilterRenderPassInfo.renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
				
				float roughness = static_cast<float>(mip) / static_cast<float>(prefilterMipLevels - 1);
				
				for (auto& object : Objects) {
					if (object.pipeline->name == "prefilterPipeline" && object.pipeline->pipeline != VK_NULL_HANDLE) {
						vkCmdPushConstants(commandBuffer, object.pipeline->pipelineLayout, 
							VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 
							0, sizeof(float), &roughness);
						m_VulkanGraphicsPipeline->renderObject(commandBuffer, object.pipeline, prefilterRenderPassInfo, object, currentImage);
					}
				}
				vkCmdEndRenderPass(commandBuffer);
				
				// No post-barrier needed; finalLayout in the render pass already
				// transitions the sub-resource to SHADER_READ_ONLY_OPTIMAL.
			}
			
			prefilterMapView = m_VulkanCreateImage->createImageView(prefilterMapImage, VK_FORMAT_R32G32B32A32_SFLOAT, 
				VK_IMAGE_VIEW_TYPE_CUBE, VK_IMAGE_ASPECT_COLOR_BIT, prefilterMipLevels, 0, 6);
			b_cubeconvulation = true;
		}

		// === MAIN SCENE RENDERING (Choose ONE render pass) ===
		// Since you have both swapchain and offscreen rendering, choose the appropriate one
		// For ImGui viewport, use offscreen rendering; for direct display, use swapchain
		
		// OPTION 1: Render to offscreen texture (for ImGui viewport)
		if (m_OffscreenRenderPass != VK_NULL_HANDLE && m_OffscreenFramebuffer != VK_NULL_HANDLE && 
			m_OffscreenColorImageView != VK_NULL_HANDLE && m_OffscreenDepthImageView != VK_NULL_HANDLE && 
			m_OffscreenResolveImageView != VK_NULL_HANDLE) {
			
			// No per-frame layout transition needed; attachment remains SHADER_READ_ONLY_OPTIMAL
			// and the render-pass handles the temporary COLOR_ATTACHMENT_OPTIMAL transition
			// internally. Removing this barrier avoids validation errors and CPU overhead.
			
			RenderPassInfo offscreenPassInfo = m_VulkanRenderPass->createSwapChainRenderPassInfo(
				m_OffscreenRenderPass, m_OffscreenFramebuffer, clearValues, m_OffscreenExtent);
			
			vkCmdBeginRenderPass(commandBuffer, &offscreenPassInfo.renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
			
			// Render scene objects to offscreen framebuffer
			for (auto& object : Objects) {
				if ((object.pipeline->name == "defaultPipeline" || object.pipeline->name == "lightPipeline" || object.pipeline->name == "skyboxPipeline") 
					&& object.pipeline->pipeline != VK_NULL_HANDLE) {
					m_VulkanGraphicsPipeline->renderObject(commandBuffer, object.pipeline, offscreenPassInfo, object, currentImage);
				}
			}
			vkCmdEndRenderPass(commandBuffer);
		
			// No explicit barrier needed here: Render pass finalLayout already transitions the resolve image
			// to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL.
		}

		// === IMGUI RENDERING (Always render to swapchain) ===
		std::vector<VkFramebuffer> swapChainFramebuffers = m_VulkanSwapChain->getSwapChainFramebuffers();
		RenderPassInfo renderPassInfo = m_VulkanRenderPass->createSwapChainRenderPassInfo(
			m_VulkanSwapChain->getRenderPass(), swapChainFramebuffers[imageIndex], clearValues, m_VulkanSwapChain->getSwapChainExtent());
		
		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo.renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		
		// Render ImGui interface to main window
		if (imGuiInitialized) {
			ImGuiContext* current_context = ImGui::GetCurrentContext();
			if (current_context != nullptr) {
				ImDrawData* draw_data = ImGui::GetDrawData();
				if (draw_data != nullptr) {
					ImGui_ImplVulkan_RenderDrawData(draw_data, commandBuffer, VK_NULL_HANDLE);
				}
			}
		}
		
		vkCmdEndRenderPass(commandBuffer);

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}

	void VulkanRendererAPI::updateUniformBuffer(uint32_t currentImage) {
		auto camera = Camera::GetInstance();
		static auto startTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		glm::vec3 ambientColor = glm::vec3(0.05f);
		glm::vec3 diffuseColor = glm::vec3(0.8f);
		glm::vec3 LightSourcePosition = glm::vec3(0.0f, 0.0f, 2.0f);
		glm::vec3 specular = glm::vec3(1.0f);
		glm::vec3 viewpos = camera->GetPosition();
		// Pre-compute expensive matrices once per frame ---------------------
		const glm::mat4 lightProjection = glm::perspective(glm::radians(90.0f), 1.f, 0.1f, 100.f);

		const float aspect = m_VulkanSwapChain->getSwapChainExtent().width /
							 static_cast<float>(m_VulkanSwapChain->getSwapChainExtent().height);
		const glm::mat4 mainProjection = glm::perspective(glm::radians(90.0f), aspect, 0.1f, 100.0f);

		// Capture views for IBL once
		glm::mat4 cachedCaptureViews[6] = {
			lightProjection * glm::lookAt(glm::vec3(0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f,-1.0f,  0.0f)),
			lightProjection * glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f,-1.0f,  0.0f)),
			lightProjection * glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			lightProjection * glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			lightProjection * glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f,-1.0f,  0.0f)),
			lightProjection * glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f,-1.0f,  0.0f))
		};

		glm::vec3 LightSourcePosition2(5 * cos(time), 0.0f, 5 * sin(time));

		// Only set default transformations if they haven't been modified by SceneEditor
		if (m_ModifiedObjects.find("Shotgun") == m_ModifiedObjects.end() || !m_ModifiedObjects["Shotgun"]) {
			objectTransformations["Shotgun"] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -3.0f, 5.0f));
		}
		if (m_ModifiedObjects.find("Helmet") == m_ModifiedObjects.end() || !m_ModifiedObjects["Helmet"]) {
			objectTransformations["Helmet"] = glm::translate(glm::mat4(1.0f), glm::vec3(7.0f, -20.0f, 0.0f));
			objectTransformations["Helmet"] = glm::scale(objectTransformations["Helmet"], glm::vec3(0.1));
		}
		if (m_ModifiedObjects.find("Glock") == m_ModifiedObjects.end() || !m_ModifiedObjects["Glock"]) {
			objectTransformations["Glock"] = glm::translate(glm::mat4(1.0f), glm::vec3(-7, -6.0f, 0));
		}
		if (m_ModifiedObjects.find("target") == m_ModifiedObjects.end() || !m_ModifiedObjects["target"]) {
			objectTransformations["target"] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, 0.f));
		}
		if (m_ModifiedObjects.find("realCube") == m_ModifiedObjects.end() || !m_ModifiedObjects["realCube"]) {
			objectTransformations["realCube"] = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		}

		// Room walls - only set if not modified
		if (m_ModifiedObjects.find("base") == m_ModifiedObjects.end() || !m_ModifiedObjects["base"]) {
			objectTransformations["base"] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -7, 0.f));
			objectTransformations["base"] = glm::scale(objectTransformations["base"], glm::vec3(50.f, 0.3f, 50.f));
		}
		if (m_ModifiedObjects.find("top") == m_ModifiedObjects.end() || !m_ModifiedObjects["top"]) {
			objectTransformations["top"] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 25, 0.f));
			objectTransformations["top"] = glm::scale(objectTransformations["top"], glm::vec3(50.f, 0.3f, 50.f));
		}
		if (m_ModifiedObjects.find("right") == m_ModifiedObjects.end() || !m_ModifiedObjects["right"]) {
			objectTransformations["right"] = glm::translate(glm::mat4(1.0f), glm::vec3(25, 0.0f, 0.f));
			objectTransformations["right"] = glm::scale(objectTransformations["right"], glm::vec3(0.3f, 50.f, 50.f));
		}
		if (m_ModifiedObjects.find("left") == m_ModifiedObjects.end() || !m_ModifiedObjects["left"]) {
			objectTransformations["left"] = glm::translate(glm::mat4(1.0f), glm::vec3(-25, 0.0f, 0.f));
			objectTransformations["left"] = glm::scale(objectTransformations["left"], glm::vec3(0.3f, 50.f, 50.f));
		}
		if (m_ModifiedObjects.find("front") == m_ModifiedObjects.end() || !m_ModifiedObjects["front"]) {
			objectTransformations["front"] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 25));
			objectTransformations["front"] = glm::scale(objectTransformations["front"], glm::vec3(50.f, 50.f, 0.3f));
		}
		if (m_ModifiedObjects.find("back") == m_ModifiedObjects.end() || !m_ModifiedObjects["back"]) {
			objectTransformations["back"] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -25));
			objectTransformations["back"] = glm::scale(objectTransformations["back"], glm::vec3(50.f, 50.f, 0.3f));
		}

		// Light objects - only update if not modified by SceneEditor
		if (m_ModifiedObjects.find("pointLight1") == m_ModifiedObjects.end() || !m_ModifiedObjects["pointLight1"]) {
			objectTransformations["pointLight1"] = glm::translate(glm::mat4(1.0f), LightSourcePosition);
			objectTransformations["pointLight1"] = glm::scale(objectTransformations["pointLight1"], glm::vec3(0.2f));
		}
		if (m_ModifiedObjects.find("pointLight2") == m_ModifiedObjects.end() || !m_ModifiedObjects["pointLight2"]) {
			objectTransformations["pointLight2"] = glm::translate(glm::mat4(1.0f), LightSourcePosition2);
			objectTransformations["pointLight2"] = glm::scale(objectTransformations["pointLight2"], glm::vec3(0.2f));
		}

		// Skybox objects - only set if not modified
		if (m_ModifiedObjects.find("skyBox") == m_ModifiedObjects.end() || !m_ModifiedObjects["skyBox"]) {
			objectTransformations["skyBox"] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.f));
		}
		if (m_ModifiedObjects.find("skyBoxHDR") == m_ModifiedObjects.end() || !m_ModifiedObjects["skyBoxHDR"]) {
			objectTransformations["skyBoxHDR"] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.f));
		}
		if (m_ModifiedObjects.find("cubeConv") == m_ModifiedObjects.end() || !m_ModifiedObjects["cubeConv"]) {
			objectTransformations["cubeConv"] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.f));
		}
		if (m_ModifiedObjects.find("prefilterCube") == m_ModifiedObjects.end() || !m_ModifiedObjects["prefilterCube"]) {
			objectTransformations["prefilterCube"] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.f));
		}

		// Only set default colors if not modified by SceneEditor
		if (m_ModifiedObjects.find("pointLight1") == m_ModifiedObjects.end() || !m_ModifiedObjects["pointLight1"]) {
			objectColor["pointLight1"] = glm::vec3(0.5f, 0.7f, 0.0f);
		}
		if (m_ModifiedObjects.find("pointLight2") == m_ModifiedObjects.end() || !m_ModifiedObjects["pointLight2"]) {
			objectColor["pointLight2"] = glm::vec3(0, 0.8, 0.8);
		}

		// --- NEW: Sync light positions with their representative cube objects ---
		auto extractTranslation = [](const glm::mat4& m) -> glm::vec3 {
			return glm::vec3(m[3][0], m[3][1], m[3][2]);
		};
		if (objectTransformations.count("pointLight1"))
			LightSourcePosition = extractTranslation(objectTransformations["pointLight1"]);
		if (objectTransformations.count("pointLight2"))
			LightSourcePosition2 = extractTranslation(objectTransformations["pointLight2"]);
		// --- END NEW ---

		glm::mat4 shadowMatrixPL1[] = {
			{ {lightProjection * glm::lookAt(LightSourcePosition, LightSourcePosition + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)) } } , // Right (+X)
			{ {lightProjection * glm::lookAt(LightSourcePosition, LightSourcePosition + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0))} } , // Left (-X)
			{ {lightProjection * glm::lookAt(LightSourcePosition, LightSourcePosition + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0))  } } , // Up (+Y)
			{ {lightProjection * glm::lookAt(LightSourcePosition, LightSourcePosition + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0))} } , // Down (-Y)
			{ {lightProjection * glm::lookAt(LightSourcePosition, LightSourcePosition + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)) } } , // Forward (+Z)
			{ {lightProjection * glm::lookAt(LightSourcePosition, LightSourcePosition + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0))} }  // Backward (-Z)
		};

		glm::mat4 shadowMatrixPL2[] = {
			{{lightProjection * glm::lookAt(LightSourcePosition2, LightSourcePosition2 + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)) }} ,     // Right (+X)
			{{lightProjection * glm::lookAt(LightSourcePosition2, LightSourcePosition2 + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0))}} ,    // Left (-X)
			{{lightProjection * glm::lookAt(LightSourcePosition2, LightSourcePosition2 + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0))  }} ,      // Up (+Y)
			{{lightProjection * glm::lookAt(LightSourcePosition2, LightSourcePosition2 + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0))}} ,    // Down (-Y)
			{{lightProjection * glm::lookAt(LightSourcePosition2, LightSourcePosition2 + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)) }} ,     // Forward (+Z)
			{{lightProjection * glm::lookAt(LightSourcePosition2, LightSourcePosition2 + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0))}}     // Backward (-Z)
		};

		pointLights = { { objectColor["pointLight1"], LightSourcePosition, ambientColor, diffuseColor, specular,viewpos, 1.f, 0.09f, 0.032f}, {objectColor["pointLight2"], LightSourcePosition2, ambientColor, diffuseColor, specular,viewpos, 1.f, 0.09f, 0.032f} };

		std::vector<shadowUBO> lightMatrixArray = {};
		lightMatrixArray.resize(2);
		std::memcpy(lightMatrixArray[0].pl_lightSpaceMatrix, shadowMatrixPL1, sizeof(shadowMatrixPL1));
		std::memcpy(lightMatrixArray[1].pl_lightSpaceMatrix, shadowMatrixPL2, sizeof(shadowMatrixPL2));

		for (auto& object : Objects) {
			glm::mat4 model = objectTransformations[object.name];
			glm::vec3 lightColor = objectColor[object.name];

			UniformBufferObject ubo{};
			ubo.model = model;
			ubo.view = camera->GetView();

			// Use cached projection matrix (avoids trig recompute per object)
			ubo.proj = mainProjection;

			ubo.proj[1][1] *= -1;

			ColorUniform colorUbo{};
			colorUbo.color = lightColor;

			if (object.pipeline->name == "lightPipeline")
			{
				memcpy(object.uniformBuffersMapped[0][currentImage], &ubo, sizeof(ubo));
				memcpy(object.uniformBuffersMapped[1][currentImage], &colorUbo, sizeof(colorUbo));
			}
			if (object.pipeline->name == "defaultPipeline")
			{
				memcpy(object.uniformBuffersMapped[0][currentImage], &ubo, sizeof(ubo));
				memcpy(object.uniformBuffersMapped[1][currentImage], pointLights.data(), sizeof(LightAttributes) * pointLights.size());
				memcpy(object.uniformBuffersMapped[2][currentImage], lightMatrixArray.data(), sizeof(shadowUBO) * lightMatrixArray.size());
			}
			if (object.pipeline->name == "skyboxPipeline")
			{
				ubo.view = glm::mat4(glm::mat3(camera->GetView()));
				memcpy(object.uniformBuffersMapped[0][currentImage], &ubo, sizeof(ubo));
			}
			if (object.pipeline->name == "skyboxPipelineHDR" || object.pipeline->name == "CubeConvPipeline" || object.pipeline->name == "prefilterPipeline")
			{
				ubo.view = glm::mat4(glm::mat3(camera->GetView()));
				memcpy(object.uniformBuffersMapped[0][currentImage], &ubo, sizeof(ubo));
				memcpy(object.uniformBuffersMapped[1][currentImage], cachedCaptureViews, sizeof(glm::mat4) * 6);
			}
		}
	}

	void VulkanRendererAPI::CacheObjectLists()
	{
		// Clear existing cached lists
		m_ShadowCasters.clear();
		m_SceneObjects.clear();
		m_LightObjects.clear();

		// Categorize objects for faster iteration
		for (auto& object : Objects) {
			if (object.pipeline->name == "defaultPipeline") {
				m_ShadowCasters.push_back(&object);
				m_SceneObjects.push_back(&object);
			}
			else if (object.pipeline->name == "lightPipeline") {
				m_LightObjects.push_back(&object);
			}
			else if (object.pipeline->name == "skyboxPipeline") {
				m_SceneObjects.push_back(&object);
			}
		}

		// Update shadow pass flag based on whether we have shadow casters
		m_ShadowPassEnabled = !m_ShadowCasters.empty();
		
		m_SceneObjectsCached = true;
		TKC_CORE_INFO("Cached {} shadow casters, {} scene objects, {} light objects", 
			m_ShadowCasters.size(), m_SceneObjects.size(), m_LightObjects.size());
	}

	void VulkanRendererAPI::RefreshImGuiOffscreenTexture()
	{
		if (m_ImGuiDescriptorSet == VK_NULL_HANDLE)
			return;

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = m_OffscreenResolveImageView;
		imageInfo.sampler = m_ImGuiSampler != VK_NULL_HANDLE ? m_ImGuiSampler : m_OffscreenSampler;

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet = m_ImGuiDescriptorSet;
		write.dstBinding = 0;
		write.dstArrayElement = 0;
		write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		write.descriptorCount = 1;
		write.pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);

		// Update ImGui texture ID used in UI code
		m_ImGuiTextureId = (ImTextureID)m_ImGuiDescriptorSet;
	}

    /*
     * Replace a texture for a given object/material channel at runtime. This is a convenience helper
     * for the SceneEditor panel. For simplicity we:
     * 1) Load the new texture via VulkanTextureManager (async, but we wait immediately).
     * 2) Replace the corresponding DescriptorTextureInfo in the object's texturesInfo vector.
     * 3) Re-create (or update) the descriptor sets for that object so the shader sees the new image.
     *
     * NOTE: This naive implementation re-allocates descriptor sets which is fine for editor usage.
     */
    void VulkanRendererAPI::UpdateObjectTexture(const std::string& objectName, const std::string& textureType, const std::string& newPath)
    {
        // Find the object
        VulkanObject* targetObj = nullptr;
        for (auto& obj : Objects) {
            if (obj.name == objectName) { targetObj = &obj; break; }
        }
        if (!targetObj) {
            TKC_CORE_WARN("UpdateObjectTexture: object '{}' not found", objectName);
            return;
        }

        // Request the texture load (blocking for now)
        std::vector<std::pair<std::string, std::string>> req = { {textureType, newPath} };
        auto futures = m_VulkanTextureManager->LoadTexturesBatch(req);
        DescriptorTextureInfo newInfo = futures[0].get();

        // Replace existing entry or append
        size_t replaceIndex = SIZE_MAX;
        for (size_t i = 0; i < targetObj->textureLocations.size(); ++i) {
            if (targetObj->textureLocations[i].first == textureType) { replaceIndex = i; break; }
        }

        if (replaceIndex == SIZE_MAX) {
            // Not found – append new mapping
            targetObj->textureLocations.push_back({textureType, newPath});
            targetObj->texturesInfo.push_back(newInfo);
        } else {
            targetObj->textureLocations[replaceIndex].second = newPath;
            // texturesInfo may not be 1-1 after the normal texture (extra textures for PBR)
            // If index within bounds, replace; otherwise append.
            if (replaceIndex < targetObj->texturesInfo.size())
                targetObj->texturesInfo[replaceIndex] = newInfo;
            else
                targetObj->texturesInfo.push_back(newInfo);
        }

        // Re-create / update descriptor sets for this object so the new image is visible
        try {
            m_VulkanGraphicsPipeline->createDescriptorSets(targetObj);
            TKC_CORE_INFO("Texture for '{}' updated (type: '{}')", objectName, textureType);
        } catch (const std::exception& e) {
            TKC_CORE_ERROR("Failed to recreate descriptor sets after texture update: {}", e.what());
        }

        // Mark as modified so auto-transform update doesn't overwrite external edits
        m_ModifiedObjects[objectName] = true;
    }

    ImTextureID VulkanRendererAPI::GetOrCreateImGuiTexture(VkSampler sampler, VkImageView view)
    {
        uint64_t key = (static_cast<uint64_t>(reinterpret_cast<uintptr_t>(sampler)) << 32) ^ static_cast<uint64_t>(reinterpret_cast<uintptr_t>(view));
        static std::unordered_map<uint64_t, ImTextureID> cache;
        auto it = cache.find(key);
        if (it != cache.end()) return it->second;

        if (!imGuiInitialized) return 0;

        VkDescriptorSet id = ImGui_ImplVulkan_AddTexture(sampler, view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        cache[key] = (ImTextureID)id;
        return (ImTextureID)id;
    }

    void VulkanRendererAPI::UpdateObjectTexture(VulkanObject* targetObj, const std::string& textureType, const std::string& newPath)
    {
        if (!targetObj) return;

        // Request load
        std::vector<std::pair<std::string, std::string>> req = { {textureType, newPath} };
        auto futures = m_VulkanTextureManager->LoadTexturesBatch(req);
        DescriptorTextureInfo newInfo = futures[0].get();

        size_t replaceIndex = SIZE_MAX;
        for (size_t i = 0; i < targetObj->textureLocations.size(); ++i) {
            if (targetObj->textureLocations[i].first == textureType) { replaceIndex = i; break; }
        }
        if (replaceIndex == SIZE_MAX) {
            targetObj->textureLocations.push_back({textureType, newPath});
            targetObj->texturesInfo.push_back(newInfo);
        } else {
            targetObj->textureLocations[replaceIndex].second = newPath;
            if (replaceIndex < targetObj->texturesInfo.size())
                targetObj->texturesInfo[replaceIndex] = newInfo;
            else
                targetObj->texturesInfo.push_back(newInfo);
        }
        try {
            m_VulkanGraphicsPipeline->createDescriptorSets(targetObj);
        } catch (...) {}
    }

    /*
     * Load an FBX model at runtime. The model is appended to the scene using the existing
     * createModel() helper. Descriptor pools / sets are regenerated so that all objects
     * (existing + new) share a fresh pool big enough to hold the enlarged object list.
     */
    void VulkanRendererAPI::LoadModelFromFBX(const std::string& modelPath, const std::string& textureDir)
    {
        // Derive a readable name from the file path (file stem without extension)
        std::string modelName = std::filesystem::path(modelPath).stem().string();

        // 1. Create the VulkanObject(s) for the FBX file (re-uses existing helper)
        createModel(modelName, modelPath, textureDir);

        // 2. Ensure textures for newly added meshes are prepared (default or custom)
        createTextureImage();

        // 3. Re-create descriptor pools with the new total object count so that future
        //    descriptor set allocations succeed even if we exceeded the original size.
        const uint32_t newObjectCount = static_cast<uint32_t>(Objects.size());
        for (auto& pipeline : Pipelines) {
            // Destroy the old pool (descriptor sets are implicitly freed)
            if (pipeline->descriptorPool != VK_NULL_HANDLE) {
                vkDestroyDescriptorPool(device, pipeline->descriptorPool, nullptr);
                pipeline->descriptorPool = VK_NULL_HANDLE;
            }
            // Create a fresh pool sized for all objects
            m_VulkanGraphicsPipeline->createDescriptorPool(pipeline, newObjectCount);
        }

        // 4. Ensure uniform buffers exist for any newly added objects
        for (auto& obj : Objects) {
            if (obj.uniformBuffers.empty()) {
                m_VulkanBuffer->createUniformBuffers(obj);
            }
        }

        // 5. Re-create descriptor sets for *all* objects so that they reference the new pools
        for (auto& obj : Objects) {
            m_VulkanGraphicsPipeline->createDescriptorSets(&obj);
        }

        // 6. Update cached object lists for faster iteration in future frames
        CacheObjectLists();

        TKC_CORE_INFO("Runtime FBX model loaded: {} ({} objects total)", modelName, Objects.size());
    }
}

