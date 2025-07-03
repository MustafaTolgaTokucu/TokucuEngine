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
		m_VulkanSwapChain = std::make_unique<VulkanSwapChain>(m_VulkanCore.get(),m_VulkanFramebuffer.get() , physicalDevice, device, surface);

		createRenderPass();
		registerPipeline();
		createGraphicsPipeline();
		createShadowFramebuffer();
		
		m_VulkanBuffer->createCommandPool();
		createObject();
		
		createTextureSampler();
		createTextureImage();

		createUniformBuffers();

		createDescriptorPool();
		createDescriptorSets();

		m_VulkanBuffer->createCommandBuffers();
		m_VulkanCore->createSyncObjects();
	}
	void VulkanRendererAPI::SetClearColor(const glm::vec4& color)
	{
	}
	void VulkanRendererAPI::Clear()
	{
		// Ensure all operations are complete before cleanup
		vkDeviceWaitIdle(device);

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
		vkWaitForFences(device, 1, &m_VulkanCore->getInFlightFences()[currentFrame], VK_TRUE, UINT64_MAX);

		uint32_t imageIndex;

		VkResult result = vkAcquireNextImageKHR(device, m_VulkanSwapChain->getSwapChain(), UINT64_MAX, m_VulkanCore->getImageAvailableSemaphores()[currentFrame], VK_NULL_HANDLE, &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			m_VulkanSwapChain->recreateSwapChain();
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
			TKC_CORE_INFO("Recreating Swap Chain");
			framebufferResized = false;
			// Recreate the swap chain and render pass
			m_VulkanSwapChain->recreateSwapChain();
		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image!");
		}
		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
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
		VulkanRenderPass RenderPass(device, physicalDevice, VK_SAMPLE_COUNT_1_BIT, m_VulkanSwapChain->findDepthFormat());
		swapRenderPass = m_VulkanSwapChain->getRenderPass();

		shadowRenderPass = RenderPass.createShadowRenderPass();

		// Create the HDR render pass
		renderPassHDR = RenderPass.createHDRRenderPass();

		// Create the BRDF LUT render pass
		BRDFRenderPass = RenderPass.createBRDFRenderPass();
		 
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
			{{sizeof(UniformBufferObject)},{sizeof(LightAttributes) * pointLights.size()},{sizeof(shadowUBO) * 2}},swapRenderPass,VK_CULL_MODE_BACK_BIT ,msaaSamples };
		Pipelines.push_back(&m_Pipeline);
		//Pipeline for point light object representives
		m_Pipeline2 = { "lightPipeline" ,"assets/shaders/vertPL.spv", "assets/shaders/fragPL.spv",{},
			{{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT},
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT}},
			{{&transformBufferInfo,VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT},{&colorBufferInfo,VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT}},
			//{},
			0,
			{{sizeof(UniformBufferObject)},{sizeof(ColorUniform)}},swapRenderPass ,VK_CULL_MODE_BACK_BIT,msaaSamples };
		Pipelines.push_back(&m_Pipeline2);
		//Pipeline for skybox rendering using 6 layered cubemap
		m_PipelineSkybox = { "skyboxPipeline" ,"assets/shaders/skyboxVert.spv", "assets/shaders/skyboxFrag.spv",{},
			{{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT}},
			{{&transformBufferInfo,VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT}},
			//{{&skyboxInfo} },
			1,
			{ {sizeof(UniformBufferObject)} },swapRenderPass,VK_CULL_MODE_FRONT_BIT,msaaSamples
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
			{ {sizeof(UniformBufferObject)},{sizeof(glm::mat4) * 6} },renderPassHDR,VK_CULL_MODE_BACK_BIT
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
			{{sizeof(UniformBufferObject)},{sizeof(glm::mat4) * 6}},renderPassHDR,VK_CULL_MODE_BACK_BIT
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
			{{sizeof(UniformBufferObject)},{sizeof(glm::mat4) * 6}},renderPassHDR,VK_CULL_MODE_BACK_BIT
		};
		Pipelines.push_back(&m_PipelinePrefilter);
		//BRDF LUT for specular IBL
		m_PipelineBRDF = { "BRDFPipeline" ,"assets/shaders/BRDFVert.spv", "assets/shaders/BRDFFrag.spv",{},
			{{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT}},
			{{&transformBufferInfo,VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT}},
			//{},
			0,
			{{sizeof(UniformBufferObject)}},BRDFRenderPass,VK_CULL_MODE_NONE
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
		{ {sizeof(UniformBufferObject)},{sizeof(LightAttributes) * pointLights.size()},{sizeof(shadowUBO) * 2} },shadowRenderPass, VK_CULL_MODE_BACK_BIT,VK_SAMPLE_COUNT_1_BIT };
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
		BRDFImageView = m_VulkanCreateImage->createImageView(BRDFImage, VK_FORMAT_R32G32_SFLOAT, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1, 0,1);
		
		BRDFFrameBuffer = m_VulkanFramebuffer->createFramebuffers(BRDFRenderPass, { BRDFImageView }, 512, 512, 1);
	}
	/*ModelData VulkanRendererAPI::loadModel(std::string modelLocation) {
		// Load model using tinyobjloader
		//FBX loader is used for the moment...

		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, modelLocation.c_str())) {
			throw std::runtime_error(warn + err);
		}
		for (const auto& shape : shapes) {
			for (size_t i = 0; i < shape.mesh.indices.size(); i += 3) { // Process triangles
				Vertex v0{}, v1{}, v2{};
				tinyobj::index_t index0 = shape.mesh.indices[i];
				tinyobj::index_t index1 = shape.mesh.indices[i + 1];
				tinyobj::index_t index2 = shape.mesh.indices[i + 2];

				// Position
				v0.Position = {
					attrib.vertices[3 * index0.vertex_index + 0],
					attrib.vertices[3 * index0.vertex_index + 1],
					attrib.vertices[3 * index0.vertex_index + 2]
				};
				v1.Position = {
					attrib.vertices[3 * index1.vertex_index + 0],
					attrib.vertices[3 * index1.vertex_index + 1],
					attrib.vertices[3 * index1.vertex_index + 2]
				};
				v2.Position = {
					attrib.vertices[3 * index2.vertex_index + 0],
					attrib.vertices[3 * index2.vertex_index + 1],
					attrib.vertices[3 * index2.vertex_index + 2]
				};

				// Normals
				v0.Normal = {
					attrib.normals[3 * index0.normal_index + 0],
					attrib.normals[3 * index0.normal_index + 1],
					attrib.normals[3 * index0.normal_index + 2]
				};
				v1.Normal = {
					attrib.normals[3 * index1.normal_index + 0],
					attrib.normals[3 * index1.normal_index + 1],
					attrib.normals[3 * index1.normal_index + 2]
				};
				v2.Normal = {
					attrib.normals[3 * index2.normal_index + 0],
					attrib.normals[3 * index2.normal_index + 1],
					attrib.normals[3 * index2.normal_index + 2]
				};

				// Texture Coordinates
				v0.TexCoords = {
					attrib.texcoords[2 * index0.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index0.texcoord_index + 1]
				};
				v1.TexCoords = {
					attrib.texcoords[2 * index1.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index1.texcoord_index + 1]
				};
				v2.TexCoords = {
					attrib.texcoords[2 * index2.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index2.texcoord_index + 1]
				};

				// Compute Tangent and Bitangent
				glm::vec3 edge1 = v1.Position - v0.Position;
				glm::vec3 edge2 = v2.Position - v0.Position;

				glm::vec2 deltaUV1 = v1.TexCoords - v0.TexCoords;
				glm::vec2 deltaUV2 = v2.TexCoords - v0.TexCoords;

				float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

				glm::vec3 tangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
				//glm::vec3 bitangent = f * (-deltaUV2.x * edge1 + deltaUV1.x * edge2);

				// Store Tangent and Bitangent
				v0.Tangent = v1.Tangent = v2.Tangent = glm::normalize(tangent);
				//v0.bitangent = v1.bitangent = v2.bitangent = glm::normalize(bitangent);

				// Push vertices
				vertices.push_back(v0);
				vertices.push_back(v1);
				vertices.push_back(v2);

				indices.push_back(indices.size());
				indices.push_back(indices.size());
				indices.push_back(indices.size());
			}
		}
		return {vertices, indices};
	}*/
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
		{{"skyboxHDR","assets/textures/qwantani_noon_4k.hdr"}} };
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
		//createModel("Helmet", "assets/models/Helmet/sci_fi_space_helmet_by_aliashasim.fbx", "assets/textures/Helmet/");
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
	
		//for (auto& object : Objects) {
		//	size_t numUBOs = object.pipeline->bufferSize.size();
		//	object.uniformBuffers.resize(numUBOs);
		//	object.uniformBuffersMemory.resize(numUBOs);
		//	object.uniformBuffersMapped.resize(numUBOs);
		//
		//	for (size_t uboIndex = 0; uboIndex < numUBOs; uboIndex++) {
		//		VkDeviceSize bufferSize = object.pipeline->bufferSize[uboIndex];
		//
		//		object.uniformBuffers[uboIndex].resize(MAX_FRAMES_IN_FLIGHT);
		//		object.uniformBuffersMemory[uboIndex].resize(MAX_FRAMES_IN_FLIGHT);
		//		object.uniformBuffersMapped[uboIndex].resize(MAX_FRAMES_IN_FLIGHT);
		//
		//		for (size_t frame = 0; frame < MAX_FRAMES_IN_FLIGHT; frame++) {
		//			createBuffer(bufferSize, object.pipeline->descriptionBufferInfo[uboIndex].second,
		//				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		//				object.uniformBuffers[uboIndex][frame], object.uniformBuffersMemory[uboIndex][frame]);
		//
		//			vkMapMemory(device, object.uniformBuffersMemory[uboIndex][frame], 0, bufferSize, 0,
		//				&object.uniformBuffersMapped[uboIndex][frame]);
		//		}
		//	}
		//}


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
	}

	void VulkanRendererAPI::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, uint32_t currentImage) {
		
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { {0.0f, 0.4f, 0.6f, 1.0f} };
		clearValues[1].depthStencil = { 1.0f, 0 };
		//first render pass for shadow map
		VkRenderPassBeginInfo shadowRenderPassInfo{};
		shadowRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		shadowRenderPassInfo.renderPass = shadowRenderPass;
		shadowRenderPassInfo.framebuffer = shadowFrameBuffer; // Shadow framebuffer
		shadowRenderPassInfo.renderArea.extent.width = shadowMapSize;
		shadowRenderPassInfo.renderArea.extent.height = shadowMapSize;

		VkClearValue clearDepth{};
		clearDepth.depthStencil = { 1.0f, 0 }; // Clear depth to maximum (1.0)

		shadowRenderPassInfo.clearValueCount = 1;
		shadowRenderPassInfo.pClearValues = &clearDepth;
		//viewport and scissor dynamics, need to match with shadow map's size
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = shadowMapSize;
		viewport.height = shadowMapSize;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent.width = shadowMapSize;
		scissor.extent.height = shadowMapSize;

		VkDeviceSize offsets[] = { 0 };
		vkCmdBeginRenderPass(commandBuffer, &shadowRenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		// Shadow map pass 
		for (int i = 0; i < 2; i++)
		{
			int lightindex = i;
			lightIndexUBO indexUBO = {};
			indexUBO.lightIndex = lightindex;
			for (auto& object : Objects)
			{
				//Do not create shadows for light objects
				if (object.pipeline->name == "defaultPipeline")
				{
					if (object.pipeline->pipeline == VK_NULL_HANDLE) {
						throw std::runtime_error("Error: pipeline is NULL before binding!");
					}
					vkCmdPushConstants(commandBuffer, object.pipeline->pipelineLayout, VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(lightIndexUBO), &indexUBO);
					vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineShadow.pipeline);
					vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
					vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
					//using specific pipeline with object specific layout...Layouts should be correaleted
					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, object.pipeline->pipelineLayout, 0, 1, &object.descriptorSets[currentFrame], 0, nullptr);
					vkCmdBindVertexBuffers(commandBuffer, 0, 1, &object.vertexBuffer, offsets);
					vkCmdBindIndexBuffer(commandBuffer, object.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
					vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(object.indexData.size()), 1, 0, 0, 0);
				}
			}
		}
		vkCmdEndRenderPass(commandBuffer);
		if (!b_cubeconvulation)
		{
			//renderpass for hdr skybox
			VkRenderPassBeginInfo eqToCubeRenderPassInfo{};
			eqToCubeRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			eqToCubeRenderPassInfo.renderPass = renderPassHDR;
			eqToCubeRenderPassInfo.framebuffer = HDRCubeFrameBuffer; // hdr framebuffer
			eqToCubeRenderPassInfo.renderArea.extent.width = 1024;
			eqToCubeRenderPassInfo.renderArea.extent.height = 1024;

			eqToCubeRenderPassInfo.clearValueCount = 1;
			eqToCubeRenderPassInfo.pClearValues = &clearDepth;

			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = 1024;
			viewport.height = 1024;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;

			scissor.offset = { 0, 0 };
			scissor.extent.width = 1024;
			scissor.extent.height = 1024;

			vkCmdBeginRenderPass(commandBuffer, &eqToCubeRenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
			// hdr pass 

			for (auto& object : Objects)
			{
				if (object.pipeline->name == "skyboxPipelineHDR")
				{
					if (object.pipeline->pipeline == VK_NULL_HANDLE) {
						throw std::runtime_error("Error: pipeline is NULL before binding!");
					}
					vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineSkyboxHDR.pipeline);
					vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
					vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
					//using specific pipeline with object specific layout...Layouts should be correaleted
					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, object.pipeline->pipelineLayout, 0, 1, &object.descriptorSets[currentFrame], 0, nullptr);
					vkCmdBindVertexBuffers(commandBuffer, 0, 1, &object.vertexBuffer, offsets);
					vkCmdBindIndexBuffer(commandBuffer, object.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
					vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(object.indexData.size()), 1, 0, 0, 0);
				}
			}

			vkCmdEndRenderPass(commandBuffer);
			//HDR Convolution pass
			VkRenderPassBeginInfo CubeConvRenderPassInfo{};
			CubeConvRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			CubeConvRenderPassInfo.renderPass = renderPassHDR;
			CubeConvRenderPassInfo.framebuffer = CubeConvolutionFrameBuffer; // hdr framebuffer
			CubeConvRenderPassInfo.renderArea.extent.width = 32;
			CubeConvRenderPassInfo.renderArea.extent.height = 32;

			CubeConvRenderPassInfo.clearValueCount = 1;
			CubeConvRenderPassInfo.pClearValues = &clearDepth;

			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = 32;
			viewport.height = 32;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;

			scissor.offset = { 0, 0 };
			scissor.extent.width = 32;
			scissor.extent.height = 32;
			vkCmdBeginRenderPass(commandBuffer, &CubeConvRenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
			for (auto& object : Objects)
			{
				if (object.pipeline->name == "CubeConvPipeline")
				{
					if (object.pipeline->pipeline == VK_NULL_HANDLE) {
						throw std::runtime_error("Error: pipeline is NULL before binding!");
					}
					vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineCubeConv.pipeline);
					vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
					vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
					//using specific pipeline with object specific layout...Layouts should be correaleted
					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, object.pipeline->pipelineLayout, 0, 1, &object.descriptorSets[currentFrame], 0, nullptr);
					vkCmdBindVertexBuffers(commandBuffer, 0, 1, &object.vertexBuffer, offsets);
					vkCmdBindIndexBuffer(commandBuffer, object.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
					vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(object.indexData.size()), 1, 0, 0, 0);
				}
			}

			vkCmdEndRenderPass(commandBuffer);
			//BRDF lud pass
			VkRenderPassBeginInfo BRDFRenderPassInfo{};
			BRDFRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			BRDFRenderPassInfo.renderPass = BRDFRenderPass;
			BRDFRenderPassInfo.framebuffer = BRDFFrameBuffer; // hdr framebuffer
			BRDFRenderPassInfo.renderArea.extent.width = 512;
			BRDFRenderPassInfo.renderArea.extent.height = 512;

			BRDFRenderPassInfo.clearValueCount = 1;
			BRDFRenderPassInfo.pClearValues = &clearDepth;

			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = 512;
			viewport.height = 512;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;

			scissor.offset = { 0, 0 };
			scissor.extent.width = 512;
			scissor.extent.height = 512;
			vkCmdBeginRenderPass(commandBuffer, &BRDFRenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
			for (auto& object : Objects)
			{
				if (object.pipeline->name == "BRDFPipeline")
				{
					if (object.pipeline->pipeline == VK_NULL_HANDLE) {
						throw std::runtime_error("Error: pipeline is NULL before binding!");
					}
					vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineBRDF.pipeline);
					vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
					vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
					//using specific pipeline with object specific layout...Layouts should be correaleted
					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, object.pipeline->pipelineLayout, 0, 1, &object.descriptorSets[currentFrame], 0, nullptr);
					vkCmdBindVertexBuffers(commandBuffer, 0, 1, &object.vertexBuffer, offsets);
					vkCmdBindIndexBuffer(commandBuffer, object.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
					vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(object.indexData.size()), 1, 0, 0, 0);
				}
			}

			vkCmdEndRenderPass(commandBuffer);

			//prefilter pass for specular ibl
			for (size_t mip = 0; mip < prefilterMipLevels; mip++)
			{
				uint32_t mipWidth = std::max(1u, static_cast<uint32_t>(prefilterMapResolution * std::pow(0.5f, mip)));
				uint32_t mipHeight = std::max(1u, static_cast<uint32_t>(prefilterMapResolution * std::pow(0.5f, mip)));
				m_VulkanBuffer->transitionImageLayout(prefilterMapImage, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, 6, 1, mip);
				VkRenderPassBeginInfo prefilterRenderPassInfo{};
				prefilterRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
				prefilterRenderPassInfo.renderPass = renderPassHDR;
				//VkFramebuffer prefilterMapFramebuffer = createPrefilterFrameBuffer(mipWidth, mipHeight, mip);

				prefilterMipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(prefilterMapResolution, prefilterMapResolution)))) + 1;
				//When creating a framebuffer, each attachment must reference only a single mip level.

				VkImageView prefilterMapTempView = m_VulkanCreateImage->createImageView(prefilterMapImage, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_VIEW_TYPE_CUBE, VK_IMAGE_ASPECT_COLOR_BIT, 1, mip, 6);

				// Create Framebuffer for prefilter map
				VkFramebuffer prefilterMapFramebuffer = VK_NULL_HANDLE;

				VkFramebufferCreateInfo prefilterFramebufferInfo{};
				prefilterFramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				prefilterFramebufferInfo.renderPass = renderPassHDR;
				prefilterFramebufferInfo.attachmentCount = 1;
				prefilterFramebufferInfo.pAttachments = &prefilterMapTempView;
				prefilterFramebufferInfo.width = mipWidth;
				prefilterFramebufferInfo.height = mipHeight;
				prefilterFramebufferInfo.layers = 6;
				if (vkCreateFramebuffer(device, &prefilterFramebufferInfo, nullptr, &prefilterMapFramebuffer) != VK_SUCCESS) {
					throw std::runtime_error("failed to create framebuffer!");
				}
				prefilterMapFramebuffers.push_back(prefilterMapFramebuffer);
				prefilterMapTempViews.push_back(prefilterMapTempView);
				prefilterRenderPassInfo.framebuffer = prefilterMapFramebuffer;
				prefilterRenderPassInfo.renderArea.extent.width = mipWidth;
				prefilterRenderPassInfo.renderArea.extent.height = mipHeight;

				prefilterRenderPassInfo.clearValueCount = 1;
				prefilterRenderPassInfo.pClearValues = &clearDepth;

				viewport.x = 0.0f;
				viewport.y = 0.0f;

				viewport.minDepth = 0.0f;
				viewport.maxDepth = 1.0f;
				viewport.width = mipWidth;
				viewport.height = mipHeight;

				scissor.extent.width = mipWidth;
				scissor.extent.height = mipHeight;
				scissor.offset = { 0, 0 };

				vkCmdBeginRenderPass(commandBuffer, &prefilterRenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
				// Calculate roughness for this mip level
				float roughness = static_cast<float>(mip) / static_cast<float>(prefilterMipLevels - 1);

				for (auto& object : Objects)
				{
					if (object.pipeline->name == "prefilterPipeline")
					{
						//TKC_CORE_INFO("prefilterPipeline");
						if (object.pipeline->pipeline == VK_NULL_HANDLE) {
							throw std::runtime_error("Error: pipeline is NULL before binding!");
						}
						vkCmdPushConstants(commandBuffer, object.pipeline->pipelineLayout, VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(float), &roughness);
						vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelinePrefilter.pipeline);
						vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
						vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
						//using specific pipeline with object specific layout...Layouts should be correaleted
						vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, object.pipeline->pipelineLayout, 0, 1, &object.descriptorSets[currentFrame], 0, nullptr);
						vkCmdBindVertexBuffers(commandBuffer, 0, 1, &object.vertexBuffer, offsets);
						vkCmdBindIndexBuffer(commandBuffer, object.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
						vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(object.indexData.size()), 1, 0, 0, 0);
					}
				}
				vkCmdEndRenderPass(commandBuffer);
				m_VulkanBuffer->transitionImageLayout(prefilterMapImage, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, 6, 1, mip);
			}
			prefilterMapView = m_VulkanCreateImage->createImageView(prefilterMapImage, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_VIEW_TYPE_CUBE, VK_IMAGE_ASPECT_COLOR_BIT, prefilterMipLevels, 0, 6);
			b_cubeconvulation = true;
		}
		//render pass info for main object pass
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_VulkanSwapChain->getRenderPass();

		//renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];

		std::vector<VkFramebuffer> swapChainFramebuffers = m_VulkanSwapChain->getSwapChainFramebuffers();
		renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];

		renderPassInfo.renderArea.offset = { 0, 0 };

		VkExtent2D swapChainExtent = m_VulkanSwapChain->getSwapChainExtent();
		renderPassInfo.renderArea.extent = swapChainExtent;



		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		//scissor and viewport size should be modified for main pass
		scissor.extent = swapChainExtent;
		viewport.width = swapChainExtent.width;
		viewport.height = swapChainExtent.height;
		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		for (auto& object : Objects)
		{
			if (object.pipeline->name == "defaultPipeline" || object.pipeline->name == "lightPipeline" || object.pipeline->name == "skyboxPipeline")
			{
				if (object.pipeline->pipeline == VK_NULL_HANDLE) {
					throw std::runtime_error("Error: pipeline is NULL before binding!");
				}
				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, object.pipeline->pipeline);
				vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
				vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
				//VkBuffer vertexBuffers[] = { object.vertexBuffer };

				vkCmdBindVertexBuffers(commandBuffer, 0, 1, &object.vertexBuffer, offsets);
				vkCmdBindIndexBuffer(commandBuffer, object.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, object.pipeline->pipelineLayout, 0, 1, &object.descriptorSets[currentFrame], 0, nullptr);
				vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(object.indexData.size()), 1, 0, 0, 0);
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

		objectTransformations["Shotgun"] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -3.0f, 5.0f));
		objectTransformations["Helmet"] = glm::translate(glm::mat4(1.0f), glm::vec3(7.0f, -20.0f, 0.0f));
		//objectTransformations["Shotgun"] = glm::scale(objectTransformations["Shotgun"], glm::vec3(0.1));
		objectTransformations["Helmet"] = glm::scale(objectTransformations["Helmet"], glm::vec3(0.1));
		//objectTransformations["Shotgun"] = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//objectTransformations["Glock"] = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, -3.0f, 5.0f));
		objectTransformations["Glock"] = glm::translate(glm::mat4(1.0f), glm::vec3(-7, -6.0f, 0));

		objectTransformations["target"] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, 0.f));
		objectTransformations["realCube"] = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		objectTransformations["base"] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -7, 0.f));
		objectTransformations["base"] = glm::scale(objectTransformations["base"], glm::vec3(50.f, 0.3f, 50.f));

		objectTransformations["top"] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 25, 0.f));
		objectTransformations["top"] = glm::scale(objectTransformations["top"], glm::vec3(50.f, 0.3f, 50.f));

		objectTransformations["right"] = glm::translate(glm::mat4(1.0f), glm::vec3(25, 0.0f, 0.f));
		objectTransformations["right"] = glm::scale(objectTransformations["right"], glm::vec3(0.3f, 50.f, 50.f));

		objectTransformations["left"] = glm::translate(glm::mat4(1.0f), glm::vec3(-25, 0.0f, 0.f));
		objectTransformations["left"] = glm::scale(objectTransformations["left"], glm::vec3(0.3f, 50.f, 50.f));

		objectTransformations["front"] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 25));
		objectTransformations["front"] = glm::scale(objectTransformations["front"], glm::vec3(50.f, 50.f, 0.3f));

		objectTransformations["back"] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -25));
		objectTransformations["back"] = glm::scale(objectTransformations["back"], glm::vec3(50.f, 50.f, 0.3f));

		objectTransformations["pointLight1"] = glm::translate(glm::mat4(1.0f), LightSourcePosition);
		//objectTransformations["pointLight2"] = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, 10.f));
		objectTransformations["pointLight2"] = glm::translate(glm::mat4(1.0f), LightSourcePosition2);
		objectTransformations["pointLight1"] = glm::scale(objectTransformations["pointLight1"], glm::vec3(0.2f));
		objectTransformations["pointLight2"] = glm::scale(objectTransformations["pointLight2"], glm::vec3(0.2f));

		//objectTransformations["skyBox"] = glm::mat4(1.0f);
		objectTransformations["skyBox"] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.f));
		objectTransformations["skyBoxHDR"] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.f));
		objectTransformations["cubeConv"] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.f));
		objectTransformations["prefilterCube"] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.f));
		//objectTransformations["skyBox"] = glm::scale(objectTransformations["skyBox"], glm::vec3(50.f, 50.f, 50.f));

		objectColor["pointLight1"] = glm::vec3(0.5f, 0.7f, 0.0f);
		objectColor["pointLight2"] = glm::vec3(0, 0.8, 0.8);
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


}