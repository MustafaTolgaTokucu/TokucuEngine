#include "tkcpch.h"
#include "VulkanRendererAPI.h"

#include <set>

#include "Tokucu/Renderer/Camera.h"
#include "Tokucu/Renderer/FBXLoader.h"


#include "VulkanSwapChain.h"
#include "VulkanCreateImage.h"
#include "VulkanFramebuffer.h"
#include "VulkanRenderPass.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanBuffer.h"

// ImGui Vulkan integration
//#include "imgui/imgui_impl_vulkan.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

namespace Tokucu {
	VulkanRendererAPI::VulkanRendererAPI()
	{
		secondVertices = {
			{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
			{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
			{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
			{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

			{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
			{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
			{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
			{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
		};
		secondIndices = {
		   0, 1, 2, 2, 3, 0,
		   4, 5, 6, 6, 7, 4
		};

		cubeVertices = {
			//Positions          // Normals           // Texture Coords
		   // Front face
		   {{-0.5f, -0.5f,  0.5f }  ,  {  0.0f,  0.0f,  1.0f}   ,{  0.0f, 0.0f  }}, // Bottom-left
		   {{ 0.5f, -0.5f,  0.5f }  ,  {  0.0f,  0.0f,  1.0f}   ,{  1.0f, 0.0f  }}, // Bottom-right
		   {{ 0.5f,  0.5f,  0.5f }  ,  {  0.0f,  0.0f,  1.0f}   ,{  1.0f, 1.0f  }}, // Top-right
		   {{-0.5f,  0.5f,  0.5f }  ,  {  0.0f,  0.0f,  1.0f}   ,{  0.0f, 1.0f  }}, // Top-left
		   // Back face		 
		   {{-0.5f, -0.5f, -0.5f }  ,  {  0.0f,  0.0f, -1.0f}   ,{  0.0f, 0.0f  }}, // Bottom-left
		   {{ 0.5f, -0.5f, -0.5f }  ,  {  0.0f,  0.0f, -1.0f}   ,{  1.0f, 0.0f  }}, // Bottom-right
		   {{ 0.5f,  0.5f, -0.5f }  ,  {  0.0f,  0.0f, -1.0f}   ,{  1.0f, 1.0f  }}, // Top-right
		   {{-0.5f,  0.5f, -0.5f }  ,  {  0.0f,  0.0f, -1.0f}   ,{  0.0f, 1.0f  }}, // Top-left
		   // Left face		
		   {{-0.5f, -0.5f, -0.5f }  ,  { -1.0f,  0.0f,  0.0f}   ,{  0.0f, 0.0f  }}, // Bottom-left
		   {{-0.5f,  0.5f, -0.5f }  ,  { -1.0f,  0.0f,  0.0f}   ,{  1.0f, 0.0f  }}, // Top-left
		   {{-0.5f,  0.5f,  0.5f }  ,  { -1.0f,  0.0f,  0.0f}   ,{  1.0f, 1.0f  }}, // Top-right
		   {{-0.5f, -0.5f,  0.5f }  ,  { -1.0f,  0.0f,  0.0f}   ,{  0.0f, 1.0f  }}, // Bottom-right
		   // Right face		
		   {{ 0.5f, -0.5f, -0.5f }  ,  {  1.0f,  0.0f,  0.0f}   ,{  0.0f, 0.0f  }}, // Bottom-left
		   {{ 0.5f,  0.5f, -0.5f }  ,  {  1.0f,  0.0f,  0.0f}   ,{  1.0f, 0.0f  }}, // Top-left
		   {{ 0.5f,  0.5f,  0.5f }  ,  {  1.0f,  0.0f,  0.0f}   ,{  1.0f, 1.0f  }}, // Top-right
		   {{ 0.5f, -0.5f,  0.5f }  ,  {  1.0f,  0.0f,  0.0f}   ,{  0.0f, 1.0f  }}, // Bottom-right
		   //Top face		
		   {{ -0.5f,  0.5f, -0.5f}  ,  {  0.0f,  1.0f,  0.0f}   ,{  0.0f, 0.0f  }}, // Bottom-left
		   {{  0.5f,  0.5f, -0.5f}  ,  {  0.0f,  1.0f,  0.0f}   ,{  1.0f, 0.0f  }}, // Bottom-right
		   {{  0.5f,  0.5f,  0.5f}  ,  {  0.0f,  1.0f,  0.0f}   ,{  1.0f, 1.0f  }}, // Top-right
		   {{ -0.5f,  0.5f,  0.5f}  ,  {  0.0f,  1.0f,  0.0f}   ,{  0.0f, 1.0f  }}, // Top-left
		   // Bottom face	 	
		   {{ -0.5f, -0.5f, -0.5f}  ,  {  0.0f, -1.0f,  0.0f}   ,{  0.0f, 0.0f  }}, // Bottom-left
		   {{  0.5f, -0.5f, -0.5f}  ,  {  0.0f, -1.0f,  0.0f}   ,{  1.0f, 0.0f  }}, // Bottom-right
		   {{  0.5f, -0.5f,  0.5f}  ,  {  0.0f, -1.0f,  0.0f}   ,{  1.0f, 1.0f  }}, // Top-right
		   {{ -0.5f, -0.5f,  0.5f}  ,  {  0.0f, -1.0f,  0.0f}   ,{  0.0f, 1.0f  }}  // Top-left
		};
		cubeIndices = {
			// Front face (CCW)
			1, 2, 0,  2, 3, 0,
			// Back face (CCW)
			6, 5, 4,  7, 6, 4,
			// Left face (CCW)
			8, 10, 9,  8, 11, 10,
			// Right face (CCW)
			12, 13, 14,  12, 14, 15,
			// Top face (CCW)
			16, 18, 17,  16, 19, 18,
			// Bottom face (CCW)
			20, 21, 22,  20, 22, 23
		};
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

		m_VulkanBuffer->createCommandPool(); // <-- Move this up before any image transitions

		createRenderPass();
		registerPipeline();
		createGraphicsPipeline();
		createShadowFramebuffer();

		createObject();

		createTextureSampler();
		createTextureImage();

		createUniformBuffers();

		createDescriptorPool();
		createDescriptorSets();

		m_VulkanBuffer->createCommandBuffers();
		m_VulkanCore->createSyncObjects();
		
		// Cache object lists for performance optimization
		CacheObjectLists();
		
		// Create offscreen resources immediately after Vulkan initialization
		CreateOffscreenResources();
		
		// Verify offscreen resources were created successfully
		if (m_OffscreenResolveImageView == VK_NULL_HANDLE) {
			TKC_CORE_ERROR("Failed to create offscreen resources during initialization!");
			throw std::runtime_error("Offscreen resources creation failed!");
		}
		TKC_CORE_INFO("Offscreen resources created successfully during initialization");
		
		// ImGui will be initialized by ImGuiLayer after renderer is ready
		// Note: Offscreen resources are now created during initialization
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
		// Order matters: destroy VulkanBuffer first to clean up command pool and buffers
		m_VulkanBuffer.reset();
		m_VulkanSwapChain.reset();
		m_VulkanGraphicsPipeline.reset();
		m_VulkanCore.reset();

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
			// Reset offscreen resources flag to force recreation
			m_OffscreenResourcesCreated = false;
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
			// Reset offscreen resources flag to force recreation
			m_OffscreenResourcesCreated = false;
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
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		poolInfo.maxSets = 1000 * 11; // 11 is the size of pool_sizes array
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
	}

	void VulkanRendererAPI::createObject() {
		BufferData vertexData;
		BufferData indexData;

		vertexData = m_VulkanBuffer->createVertexBuffer(cubeVertices);
		indexData = m_VulkanBuffer->createIndexBuffer(cubeIndices);
		BufferData planeVertexData = m_VulkanBuffer->createVertexBuffer(secondVertices);
		BufferData planeIndexData = m_VulkanBuffer->createIndexBuffer(secondIndices);

		//realCube = { "realCube", cubeVertices, cubeIndices, vertexData.buffer, vertexData.memory,indexData.buffer, indexData.memory, getBindingDescription() , getAttributeDescriptions(),&m_Pipeline,true,std::nullopt,
		//	{{"ambient","textures/Lambda.jpg"},{"diffuse","textures/Lambda.jpg"},{"specular","textures/LambdaSpecular.jpg"},{"normal","textures/defaultNormal.jpg"}} };
		//Objects.push_back(realCube);

		//VulkanObject base = { "base", cubeVertices,cubeIndices, vertexData.buffer, vertexData.memory,indexData.buffer, indexData.memory, getBindingDescription() , getAttributeDescriptions(),&m_Pipeline,true,std::nullopt };
		//Objects.push_back(base);
		//
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

		//createModel("Shotgun", "assets/models/shotgun/ShotgunTri.fbx", "assets/textures/Shotgun/");
		createModel("Helmet", "assets/models/Helmet/sci_fi_space_helmet_by_aliashasim.fbx", "assets/textures/Helmet/");
		//createModel("Glock", "assets/models/Glock/MDL_Glock.fbx", "assets/textures/Glock/");

	}

	void VulkanRendererAPI::createModel(std::string modelName, std::string modelLocation, std::string textureLocation) {
		std::unique_ptr<FBXLoader> loader = std::make_unique<FBXLoader>(modelLocation.c_str());
		FBXModelData model = loader->getModelData();

		std::vector<std::pair<std::string, std::string>> materialTextureLocations;
		for (int i = 0; i < model.meshes.size(); i++) {
			auto& mesh = model.meshes[i];
			BufferData vertexData = m_VulkanBuffer->createVertexBuffer(mesh.vertices);
			BufferData indexData = m_VulkanBuffer->createIndexBuffer(mesh.indices);

			std::string materialName = mesh.materialName;
			std::cout << "Processing material: " << materialName << std::endl;
			materialTextureLocations.clear();
			materialTextureLocations = { {"ambient",textureLocation + materialName + "Base.png"},{"diffuse",textureLocation + materialName + "Base.png"},{"specular",textureLocation + materialName + "Specular.png"},{"normal",textureLocation + materialName + "Normal.png"} };
			// Assign per-mesh materials
			VulkanObject obj = {
				modelName,
				mesh.vertices,
				mesh.indices,
				vertexData.buffer,
				vertexData.memory,
				indexData.buffer,
				indexData.memory,
				//getBindingDescription(),
				//getAttributeDescriptions(),
				&m_Pipeline,
				true,
				modelLocation,
				materialTextureLocations // Assign per-mesh materials
			};
			Objects.push_back(obj);
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
				for (int i = 0; i < 6; i++) {
					// Load image
					std::cout << "Loading skybox face: " << skyboxFaces[i] << std::endl;
					int texWidth, texHeight, texChannels;
					stbi_uc* pixels = stbi_load(skyboxFaces[i].c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
					VkDeviceSize imageSize = texWidth * texHeight * 4;

					if (!pixels) {
						throw std::runtime_error("Failed to load skybox face: " + skyboxFaces[i]);
					}
					// Create staging buffer
					VkBuffer stagingBuffer;
					VkDeviceMemory stagingBufferMemory;
					m_VulkanBuffer->createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
						VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
						stagingBuffer, stagingBufferMemory);
					// Copy pixels to staging buffer
					void* data;
					vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
					memcpy(data, pixels, static_cast<size_t>(imageSize));
					vkUnmapMemory(device, stagingBufferMemory);
					stbi_image_free(pixels);
					// Copy from buffer to the specific face of the cubemap
					m_VulkanBuffer->copyBufferToImage(stagingBuffer, skyboxImage,
						static_cast<uint32_t>(texWidth),
						static_cast<uint32_t>(texHeight), i);
					// Clean up staging resources
					vkDestroyBuffer(device, stagingBuffer, nullptr);
					vkFreeMemory(device, stagingBufferMemory, nullptr);
				}
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
			for (const auto& [type, path] : object.textureLocations) {

				std::cout << "Loading " << type << " texture: " << path << std::endl;
				int texWidth, texHeight, texChannels;
				stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
				VkDeviceSize imageSize = texWidth * texHeight * 4;
				//mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;
				mipLevels = 1;
				resizeMipLevels = mipLevels;

				if (!pixels) {
					TKC_CORE_ERROR("Failed to find correct texture image!");
					std::string location;
					if (type == "ambient" || type == "diffuse" || type == "specular")
					{
						location = "assets/textures/default.jpg";
					}
					else if (type == "normal")
					{
						location = "assets/textures/defaultNormal.jpg";
					}
					else
					{
						throw std::runtime_error("failed to load texture image!");
					}
					//texWidth, texHeight, texChannels = 0;
					pixels = stbi_load(location.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
					imageSize = texWidth * texHeight * 4;
					mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;
					if (!pixels) {
						throw std::runtime_error("failed to load texture image!");
					}
				}

				VkBuffer stagingBuffer = VK_NULL_HANDLE;
				VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;
				m_VulkanBuffer->createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

				void* data;
				vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
				memcpy(data, pixels, static_cast<size_t>(imageSize));
				vkUnmapMemory(device, stagingBufferMemory);

				stbi_image_free(pixels);

				VkImage textureImage = VK_NULL_HANDLE;
				VkDeviceMemory textureImageMemory = VK_NULL_HANDLE;

				m_VulkanCreateImage->createImage(texWidth, texHeight, mipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory, 1, 0);
				m_VulkanBuffer->transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, 1, mipLevels, 0);
				m_VulkanBuffer->copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 0);
				//generateMipmaps(textureImage, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, mipLevels);
				m_VulkanBuffer->transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, 1, mipLevels, 0);
				vkDestroyBuffer(device, stagingBuffer, nullptr);
				vkFreeMemory(device, stagingBufferMemory, nullptr);

				VkImageView textureImageView = m_VulkanCreateImage->createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, 0, 1);
				if (type == "ambient")
				{
					//object.textures.imageViews[0] = textureImageView;
					DescriptorTextureInfo ambientInfo = { VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, textureSampler, textureImageView };
					object.texturesInfo.push_back(ambientInfo);
				}
				else if (type == "diffuse")
				{
					//	object.textures.imageViews[1] = textureImageView;
					DescriptorTextureInfo diffuseInfo = { VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, textureSampler, textureImageView };
					object.texturesInfo.push_back(diffuseInfo);
				}
				else if (type == "specular")
				{
					//object.textures.imageViews[2] = textureImageView;
					DescriptorTextureInfo specularInfo = { VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, textureSampler, textureImageView };
					object.texturesInfo.push_back(specularInfo);
				}
				else if (type == "normal")
				{
					//object.textures.imageViews[3] = textureImageView;
					DescriptorTextureInfo normalInfo = { VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, textureSampler, textureImageView };
					object.texturesInfo.push_back(normalInfo);

					DescriptorTextureInfo shadowInfo = { VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, shadowSampler, shadowImageView };
					object.texturesInfo.push_back(shadowInfo);

					DescriptorTextureInfo cubeConvolutionInfo = { VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, textureSampler, CubeConvolutionImageView };
					object.texturesInfo.push_back(cubeConvolutionInfo);

					DescriptorTextureInfo prefilterInfo = { VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, prefilterTextureSampler, prefilterMapView };
					object.texturesInfo.push_back(prefilterInfo);

					DescriptorTextureInfo BRDFInfo = { VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, textureSampler, BRDFImageView };
					object.texturesInfo.push_back(BRDFInfo);


				}
				textureImages.push_back(textureImage);
				textureImagesView.push_back(textureImageView);
				textureImagesMemory.push_back(textureImageMemory);

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

				m_VulkanBuffer->transitionImageLayout(prefilterMapImage, VK_FORMAT_R32G32B32A32_SFLOAT, 
					VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
					VK_IMAGE_ASPECT_COLOR_BIT, 6, 1, mip);
				
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
				
				m_VulkanBuffer->transitionImageLayout(prefilterMapImage, VK_FORMAT_R32G32B32A32_SFLOAT, 
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 
					VK_IMAGE_ASPECT_COLOR_BIT, 6, 1, mip);
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
			
			// Transition offscreen resolve image to color attachment layout
			if (m_OffscreenResolveImage != VK_NULL_HANDLE && m_OffscreenFormat != VK_FORMAT_UNDEFINED) {
				try {
					m_VulkanBuffer->transitionImageLayout(
						m_OffscreenResolveImage, m_OffscreenFormat,
						VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
						VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, 0);
				}
				catch (const std::exception& e) {
					TKC_CORE_ERROR("Failed to transition offscreen resolve image: {}", e.what());
				}
			}
			
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
		
			// Transition offscreen resolve image to shader read layout for ImGui texture
			if (m_OffscreenResolveImage != VK_NULL_HANDLE && m_OffscreenFormat != VK_FORMAT_UNDEFINED) {
				try {
					m_VulkanBuffer->transitionImageLayout(
						m_OffscreenResolveImage, m_OffscreenFormat,
						VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
						VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, 0);
				}
				catch (const std::exception& e) {
					TKC_CORE_ERROR("Failed to transition offscreen resolve image to SHADER_READ_ONLY_OPTIMAL: {}", e.what());
				}
			}
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
		glm::mat4 lightProjection = glm::perspective(glm::radians(90.0f), 1.f, 0.1f, 100.f);
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
		glm::mat4 captureViews[] =
		{
		  lightProjection * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		  lightProjection * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		  lightProjection * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		  lightProjection * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		  lightProjection * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		  lightProjection * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
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

			//ubo.proj = glm::perspective(glm::radians(90.0f), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 100.0f);
			ubo.proj = glm::perspective(glm::radians(90.0f), m_VulkanSwapChain->getSwapChainExtent().width / (float)m_VulkanSwapChain->getSwapChainExtent().height, 0.1f, 100.0f);

			//ubo.proj = glm::perspective(glm::radians(90.0f), 1.f, 0.1f, 100.0f);
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
				memcpy(object.uniformBuffersMapped[1][currentImage], captureViews, sizeof(glm::mat4) * 6);
			}
		}
	}

	void VulkanRendererAPI::CreateOffscreenResources()
	{
		TKC_CORE_INFO("Creating offscreen resources...");
		
		// Wait for device to be idle before recreating resources
		vkDeviceWaitIdle(device);
		
		// Get swapchain extent and format
		m_OffscreenFormat = m_VulkanSwapChain->getSwapChainImageFormat();
		m_OffscreenExtent = m_VulkanSwapChain->getSwapChainExtent();
		
		TKC_CORE_INFO("Offscreen format: {}, extent: {}x{}", static_cast<int>(m_OffscreenFormat), m_OffscreenExtent.width, m_OffscreenExtent.height);
		
		// Cleanup existing offscreen resources
		if (m_OffscreenColorImageView) vkDestroyImageView(device, m_OffscreenColorImageView, nullptr);
		if (m_OffscreenColorImage) vkDestroyImage(device, m_OffscreenColorImage, nullptr);
		if (m_OffscreenColorImageMemory) vkFreeMemory(device, m_OffscreenColorImageMemory, nullptr);

		if (m_OffscreenResolveImageView) vkDestroyImageView(device, m_OffscreenResolveImageView, nullptr);
		if (m_OffscreenResolveImage) vkDestroyImage(device, m_OffscreenResolveImage, nullptr);
		if (m_OffscreenResolveImageMemory) vkFreeMemory(device, m_OffscreenResolveImageMemory, nullptr);

		if (m_OffscreenDepthImageView) vkDestroyImageView(device, m_OffscreenDepthImageView, nullptr);
		if (m_OffscreenDepthImage) vkDestroyImage(device, m_OffscreenDepthImage, nullptr);
		if (m_OffscreenDepthImageMemory) vkFreeMemory(device, m_OffscreenDepthImageMemory, nullptr);

		// Reset handles
		m_OffscreenColorImageView = VK_NULL_HANDLE;
		m_OffscreenColorImage = VK_NULL_HANDLE;
		m_OffscreenColorImageMemory = VK_NULL_HANDLE;
		m_OffscreenResolveImageView = VK_NULL_HANDLE;
		m_OffscreenResolveImage = VK_NULL_HANDLE;
		m_OffscreenResolveImageMemory = VK_NULL_HANDLE;
		m_OffscreenDepthImageView = VK_NULL_HANDLE;
		m_OffscreenDepthImage = VK_NULL_HANDLE;
		m_OffscreenDepthImageMemory = VK_NULL_HANDLE;

		// Create offscreen images
		VkFormat depthFormat = m_VulkanSwapChain->findDepthFormat();
		TKC_CORE_INFO("Depth format: {}", static_cast<int>(depthFormat));

		if (m_OffscreenExtent.width > 0 && m_OffscreenExtent.height > 0 && m_OffscreenFormat != VK_FORMAT_UNDEFINED) {
			TKC_CORE_INFO("Creating offscreen images...");
			
			// Color image (MSAA)
			m_VulkanCreateImage->createImage(
				m_OffscreenExtent.width, m_OffscreenExtent.height, 1, msaaSamples, m_OffscreenFormat,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				m_OffscreenColorImage, m_OffscreenColorImageMemory, 1, 0);
			m_OffscreenColorImageView = m_VulkanCreateImage->createImageView(
				m_OffscreenColorImage, m_OffscreenFormat, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, 1);
			TKC_CORE_INFO("Color image created: {}, view: {}", (ImU64)m_OffscreenColorImage, (ImU64)m_OffscreenColorImageView);

			// Resolve image (single-sampled, for ImGui)
			m_VulkanCreateImage->createImage(
				m_OffscreenExtent.width, m_OffscreenExtent.height, 1, VK_SAMPLE_COUNT_1_BIT, m_OffscreenFormat,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				m_OffscreenResolveImage, m_OffscreenResolveImageMemory, 1, 0);
			m_OffscreenResolveImageView = m_VulkanCreateImage->createImageView(
				m_OffscreenResolveImage, m_OffscreenFormat, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, 1);
			TKC_CORE_INFO("Resolve image created: {}, view: {}", (ImU64)m_OffscreenResolveImage, (ImU64)m_OffscreenResolveImageView);

			// Note: The image starts in UNDEFINED layout and will be transitioned to COLOR_ATTACHMENT_OPTIMAL
			// when needed in the command buffer recording

			// Depth image (MSAA)
			m_VulkanCreateImage->createImage(
				m_OffscreenExtent.width, m_OffscreenExtent.height, 1, msaaSamples, depthFormat,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				m_OffscreenDepthImage, m_OffscreenDepthImageMemory, 1, 0);
			m_OffscreenDepthImageView = m_VulkanCreateImage->createImageView(
				m_OffscreenDepthImage, depthFormat, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_DEPTH_BIT, 1, 0, 1);
			TKC_CORE_INFO("Depth image created: {}, view: {}", (ImU64)m_OffscreenDepthImage, (ImU64)m_OffscreenDepthImageView);
		}
		else {
			TKC_CORE_ERROR("Invalid offscreen extent or format - width: {}, height: {}, format: {}", 
				m_OffscreenExtent.width, m_OffscreenExtent.height, static_cast<int>(m_OffscreenFormat));
		}

		// Create offscreen render pass and framebuffer
		// We need a custom render pass for offscreen that sets the correct final layout
		m_OffscreenRenderPass = CreateOffscreenRenderPass(msaaSamples, m_OffscreenFormat);
		TKC_CORE_INFO("Offscreen render pass created: {}", (ImU64)m_OffscreenRenderPass);

		if (m_OffscreenColorImageView != VK_NULL_HANDLE && m_OffscreenDepthImageView != VK_NULL_HANDLE && m_OffscreenResolveImageView != VK_NULL_HANDLE) {
			std::vector<VkImageView> attachments = {
				m_OffscreenColorImageView, m_OffscreenDepthImageView, m_OffscreenResolveImageView
			};
			m_OffscreenFramebuffer = m_VulkanFramebuffer->createFramebuffers(
				m_OffscreenRenderPass, attachments, m_OffscreenExtent.width, m_OffscreenExtent.height, 1);
			TKC_CORE_INFO("Offscreen framebuffer created: {}", (ImU64)m_OffscreenFramebuffer);
		}
		else {
					TKC_CORE_ERROR("Failed to create offscreen framebuffer - color view: {}, depth view: {}, resolve view: {}", 
			(ImU64)m_OffscreenColorImageView, (ImU64)m_OffscreenDepthImageView, (ImU64)m_OffscreenResolveImageView);
		}
		
		// Mark offscreen resources as created first
		m_OffscreenResourcesCreated = true;
		TKC_CORE_INFO("Offscreen resources creation completed");
		
		// Update descriptor sets to use the new offscreen resolve image view
		// This is important because the old descriptor sets might still reference the destroyed image views
		//UpdateDescriptorSetsForOffscreenResources();
	}

	VkRenderPass VulkanRendererAPI::CreateOffscreenRenderPass(VkSampleCountFlagBits msaaSamples, VkFormat imageFormat) {
		VkRenderPass renderPass = VK_NULL_HANDLE;
		
		// Color attachment (MSAA)
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = imageFormat;
		colorAttachment.samples = msaaSamples;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		// Resolve attachment (single-sampled, for ImGui texture)
		VkAttachmentDescription colorAttachmentResolve{};
		colorAttachmentResolve.format = imageFormat;
		colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // Correct layout for ImGui texture

		// Depth attachment
		VkAttachmentDescription depthAttachment{};
		depthAttachment.format = m_VulkanSwapChain->findDepthFormat();
		depthAttachment.samples = msaaSamples;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		// Attachment references
		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference colorAttachmentResolveRef{};
		colorAttachmentResolveRef.attachment = 2;
		colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		// Subpass
		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;
		subpass.pResolveAttachments = &colorAttachmentResolveRef;

		// Subpass dependency
		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		// Create render pass
		std::array<VkAttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };
		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
			throw std::runtime_error("failed to create offscreen render pass!");
		}
		
		return renderPass;
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
}

