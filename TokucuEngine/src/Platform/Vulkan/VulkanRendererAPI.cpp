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


/// <summary>
/// TODO: VulkanRendererAPI should be renamed to VulkanRenderer.
/// This class is a temporary implementation of the Vulkan Renderer API.
/// glfw will be used for vulcan core surface createion
/// </summary>

namespace Tokucu {

	//TODO: Better abstraction for clarity

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

		m_VulkanSwapChain = std::make_unique<VulkanSwapChain>(m_VulkanCore.get(), physicalDevice, device, surface);
		m_VulkanCreateImage = std::make_unique<VulkanCreateImage>(device, physicalDevice);
		m_VulkanGraphicsPipeline = std::make_unique<VulkanGraphicsPipeline>(m_VulkanCore.get(), device, physicalDevice);
		m_VulkanBuffer = std::make_unique<VulkanBuffer>(m_VulkanCore.get(), physicalDevice, device);

		createRenderPass();
		registerPipeline();
		createGraphicsPipeline();
		createShadowFramebuffer();
		
		m_VulkanBuffer->createCommandPool();
		createObject();
		
		createTextureImage();
		createTextureSampler();

		createUniformBuffers();

		createDescriptorPool();
		createDescriptorSets();

		m_VulkanBuffer->createCommandBuffers();
		createSyncObjects();
	}
	void VulkanRendererAPI::SetClearColor(const glm::vec4& color)
	{
	}
	void VulkanRendererAPI::Clear()
	{
		vkDeviceWaitIdle(device);

		//
		//vkDestroyImageView(device, colorImageView, nullptr);
		//vkDestroyImage(device, colorImage, nullptr);
		//vkFreeMemory(device, colorImageMemory, nullptr);
		//
		//vkDestroyImageView(device, depthImageView, nullptr);
		//vkDestroyImage(device, depthImage, nullptr);
		//vkFreeMemory(device, depthImageMemory, nullptr);
		
		vkDestroyImageView(device, shadowImageView, nullptr);
		vkDestroyImage(device, shadowImage, nullptr);
		vkFreeMemory(device, shadowImageMemory, nullptr);
		
		vkDestroyImageView(device, HDRCubeView, nullptr);
		vkDestroyImage(device, HDRCubeImage, nullptr);
		vkFreeMemory(device, HDRCubeImageMemory, nullptr);
		
		for (size_t i = 0; i < prefilterMapTempViews.size(); i++)
		{
			if (prefilterMapTempViews[i] != VK_NULL_HANDLE)
			{
				vkDestroyImageView(device, prefilterMapTempViews[i], nullptr);
				prefilterMapTempViews[i] = VK_NULL_HANDLE;
			}
		}
		vkDestroyImageView(device, prefilterMapView, nullptr);
		vkDestroyImage(device, prefilterMapImage, nullptr);
		vkFreeMemory(device, prefilterMapImageMemory, nullptr);
		
		vkDestroyImageView(device, BRDFImageView, nullptr);
		vkDestroyImage(device, BRDFImage, nullptr);
		vkFreeMemory(device, BRDFImageMemory, nullptr);
		
		vkDestroyImageView(device, skyboxImageView, nullptr);
		vkDestroyImage(device, skyboxImage, nullptr);
		vkFreeMemory(device, skyboxImageMemory, nullptr);
		
		vkDestroyImageView(device, skyboxHDRImageView, nullptr);
		vkDestroyImage(device, skyboxHDRImage, nullptr);
		vkFreeMemory(device, skyboxHDRImageMemory, nullptr);
		
		vkDestroyImageView(device, CubeConvolutionImageView, nullptr);
		vkDestroyImage(device, CubeConvolutionImage, nullptr);
		vkFreeMemory(device, CubeConvolutionImageMemory, nullptr);
		
		vkDestroySampler(device, textureSampler, nullptr);
		vkDestroySampler(device, shadowSampler, nullptr);
		vkDestroySampler(device, prefilterTextureSampler, nullptr);

		for (auto texture : textureImages)
		{
			if (texture != VK_NULL_HANDLE)
			{
				vkDestroyImage(device, texture, nullptr);
				texture = VK_NULL_HANDLE;
			}
			
		}
		for (auto textureMemory : textureImagesMemory)
		{
			if (textureMemory != VK_NULL_HANDLE)
			{
				vkFreeMemory(device, textureMemory, nullptr);
				textureMemory = VK_NULL_HANDLE;
			}
		}
		for (auto textureView : textureImagesView)
		{
			if (textureView != VK_NULL_HANDLE)
			{
				vkDestroyImageView(device, textureView, nullptr);
				textureView = VK_NULL_HANDLE;
			}
		}

		//cleanupSwapChain();

		vkDestroyFramebuffer(device, shadowFrameBuffer, nullptr);
		vkDestroyFramebuffer(device, HDRCubeFrameBuffer, nullptr);
		vkDestroyFramebuffer(device, CubeConvolutionFrameBuffer, nullptr);
		vkDestroyFramebuffer(device, BRDFFrameBuffer, nullptr);
		for (size_t i = 0; i < prefilterMapFramebuffers.size(); i++)
		{
			vkDestroyFramebuffer(device, prefilterMapFramebuffers[i], nullptr);
		}

		for (int i = 0; i < Objects.size(); i++)
		{

			if(Objects[i].b_PBR == true )
			{
				for (size_t j = 0; j < Objects[i].textures.imageViews.size(); j++)
				{
					if (Objects[i].textures.imageViews[j] != VK_NULL_HANDLE) {
						vkDestroyImageView(device, Objects[i].textures.imageViews[j], nullptr);
						Objects[i].textures.imageViews[j] = VK_NULL_HANDLE;
					}
				}

				if (Objects[i].textures.ambient != VK_NULL_HANDLE) {
					vkDestroyImage(device, Objects[i].textures.ambient, nullptr);
					vkFreeMemory(device, Objects[i].textures.ambientMemory, nullptr);
					Objects[i].textures.ambient = VK_NULL_HANDLE;
					Objects[i].textures.ambientMemory = VK_NULL_HANDLE;
				}
				if (Objects[i].textures.diffuse != VK_NULL_HANDLE) {
					vkDestroyImage(device, Objects[i].textures.diffuse, nullptr);
					vkFreeMemory(device, Objects[i].textures.diffuseMemory, nullptr);
					Objects[i].textures.diffuse = VK_NULL_HANDLE;
					Objects[i].textures.diffuseMemory = VK_NULL_HANDLE;
				}
				if (Objects[i].textures.specular != VK_NULL_HANDLE) {
					vkDestroyImage(device, Objects[i].textures.specular, nullptr);
					vkFreeMemory(device, Objects[i].textures.specularMemory, nullptr);
					Objects[i].textures.specular = VK_NULL_HANDLE;
					Objects[i].textures.specularMemory = VK_NULL_HANDLE;
				}
				if (Objects[i].textures.normal != VK_NULL_HANDLE) {
					vkDestroyImage(device, Objects[i].textures.normal, nullptr);
					vkFreeMemory(device, Objects[i].textures.normalMemory, nullptr);
					Objects[i].textures.normal = VK_NULL_HANDLE;
					Objects[i].textures.normalMemory = VK_NULL_HANDLE;
				}
			}

			for (size_t j = 0; j < MAX_FRAMES_IN_FLIGHT; j++) {
				for (size_t u = 0; u < Objects[i].uniformBuffers.size(); u++)
				{
					if (Objects[i].uniformBuffers[u][j] != VK_NULL_HANDLE) {
						vkDestroyBuffer(device, Objects[i].uniformBuffers[u][j], nullptr);
						Objects[i].uniformBuffers[u][j] = VK_NULL_HANDLE;
					}
				}
				for (size_t u = 0; u < Objects[i].uniformBuffersMemory.size(); u++)
				{
					if (Objects[i].uniformBuffersMemory[u][j] != VK_NULL_HANDLE) {
						vkFreeMemory(device, Objects[i].uniformBuffersMemory[u][j], nullptr);
						Objects[i].uniformBuffersMemory[u][j] = VK_NULL_HANDLE;
					}
				}
			}

			// //Destroy the vertex and index buffers
			//if (Objects[i].vertexBuffer != VK_NULL_HANDLE) {
			//	vkDestroyBuffer(device, Objects[i].vertexBuffer, nullptr);
			//	vkFreeMemory(device, Objects[i].vertexBufferMemory, nullptr);
			//	Objects[i].vertexBuffer = VK_NULL_HANDLE;
			//	Objects[i].vertexBufferMemory = VK_NULL_HANDLE;
			//}
			//if (Objects[i].indexBuffer != VK_NULL_HANDLE) {
			//	vkDestroyBuffer(device, Objects[i].indexBuffer, nullptr);
			//	vkFreeMemory(device, Objects[i].indexBufferMemory, nullptr);
			//	Objects[i].indexBuffer = VK_NULL_HANDLE;
			//	Objects[i].indexBufferMemory = VK_NULL_HANDLE;
			//}

			// Destroy the descriptor sets
			//for (size_t j = 0; j < Objects[i].descriptorSets.size(); j++) {
			//	vkFreeDescriptorSets(device, Objects[i].pipeline->descriptorPool, 1, &Objects[i].descriptorSets[j]);
			//}
			//Objects[i].descriptorSets.clear();
			// Destroy the pipeline and descriptor set layout




			//Objects[i].uniformBuffers.clear();
			//Objects[i].uniformBuffersMemory.clear();
		}

		for (size_t i = 0; i < objectCreationBuffers.size(); i++)
		{
		
			if (objectCreationBuffers[i].buffer != VK_NULL_HANDLE) {
				vkDestroyBuffer(device, objectCreationBuffers[i].buffer, nullptr);
				objectCreationBuffers[i].buffer = VK_NULL_HANDLE;
			}
			if (objectCreationBuffers[i].memory != VK_NULL_HANDLE) {
				vkFreeMemory(device, objectCreationBuffers[i].memory, nullptr);
				objectCreationBuffers[i].memory = VK_NULL_HANDLE;
			}
			objectCreationBuffers[i].buffer = VK_NULL_HANDLE;
			objectCreationBuffers[i].memory = VK_NULL_HANDLE;
		
		}

		for (auto& pipeline : Pipelines) {
			vkDestroyPipeline(device, pipeline->pipeline, nullptr);
			vkDestroyPipelineLayout(device, pipeline->pipelineLayout, nullptr);
			vkDestroyDescriptorSetLayout(device, pipeline->descriptorSetLayout, nullptr);
			vkDestroyDescriptorPool(device, pipeline->descriptorPool, nullptr);
			//delete pipeline;	
		}

		

		// After destroying all buffers, clear the list
		//Objects.clear();

		vkDestroyRenderPass(device, shadowRenderPass, nullptr);
		//vkDestroyRenderPass(device, renderPass, nullptr);
		vkDestroyRenderPass(device, renderPassHDR, nullptr);
		vkDestroyRenderPass(device, BRDFRenderPass, nullptr);



		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
			vkDestroyFence(device, inFlightFences[i], nullptr);
		}

		//vkDestroyCommandPool(device, commandPool, nullptr);
		vkDestroyDevice(device, nullptr);


		//if (enableValidationLayers) {
		//	DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		//}
		//vkDestroySurfaceKHR(instance, surface, nullptr);
		//vkDestroyInstance(instance, nullptr);

		//b_cubeconvulation = false;
		TKC_CORE_INFO("Cleanup Completed");
	}
	void VulkanRendererAPI::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
	{
	}
	void VulkanRendererAPI::Render()
	{
		vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

		uint32_t imageIndex;

		VkResult result = vkAcquireNextImageKHR(device, m_VulkanSwapChain->getSwapChain(), UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			m_VulkanSwapChain->recreateSwapChain();
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}
		updateUniformBuffer(currentFrame);

		vkResetFences(device, 1, &inFlightFences[currentFrame]);

		commandBuffers = m_VulkanBuffer->getCommandBuffers();
		vkResetCommandBuffer(commandBuffers[currentFrame], 0);
		recordCommandBuffer(commandBuffers[currentFrame], imageIndex, currentFrame);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

		VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
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
		//////////////////////////////////////////
		// TODO: Create general framebuffer function for better organization
		// /////////////////////////////////////

		// Framebuffer for Shadow Mapping
		m_VulkanCreateImage->createImage(shadowMapSize, shadowMapSize, 1, VK_SAMPLE_COUNT_1_BIT,
			VK_FORMAT_D32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			shadowImage, shadowImageMemory, 12, 16);
		shadowImageView = m_VulkanCreateImage->createImageView(shadowImage, VK_FORMAT_D32_SFLOAT, VK_IMAGE_VIEW_TYPE_CUBE_ARRAY, VK_IMAGE_ASPECT_DEPTH_BIT, 1, 0, 12);

		
		VulkanFramebuffer shadowFramebuffer(device, shadowRenderPass, { shadowImageView }, shadowMapSize, shadowMapSize, 12);
		shadowFrameBuffer = shadowFramebuffer.createFramebuffers();

		// Framebuffer for HDR Cubemap
		m_VulkanCreateImage->createImage(1024, 1024, 1, VK_SAMPLE_COUNT_1_BIT,
			VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			HDRCubeImage, HDRCubeImageMemory, 6, 16);
		HDRCubeView = m_VulkanCreateImage->createImageView(HDRCubeImage, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_VIEW_TYPE_CUBE, VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, 6);


		VulkanFramebuffer HDRCubeFramebuffer(device, renderPassHDR, { HDRCubeView }, 1024, 1024, 6);
		HDRCubeFrameBuffer = HDRCubeFramebuffer.createFramebuffers();


		// Framebuffer for Cube Convolution
		m_VulkanCreateImage->createImage(32, 32, 1, VK_SAMPLE_COUNT_1_BIT,
			VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			CubeConvolutionImage, CubeConvolutionImageMemory, 6, 16);
		CubeConvolutionImageView = m_VulkanCreateImage->createImageView(CubeConvolutionImage, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_VIEW_TYPE_CUBE, VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, 6);
		
		VulkanFramebuffer CubeConvolutionFramebuffer(device, renderPassHDR, { CubeConvolutionImageView }, 32, 32, 6);
		CubeConvolutionFrameBuffer = CubeConvolutionFramebuffer.createFramebuffers();

		// Framebuffer for Prefiltered Cubemap
		m_VulkanCreateImage->createImage(512, 512, 1, VK_SAMPLE_COUNT_1_BIT,
			VK_FORMAT_R32G32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			BRDFImage, BRDFImageMemory, 1, 0);
		BRDFImageView = m_VulkanCreateImage->createImageView(BRDFImage, VK_FORMAT_R32G32_SFLOAT, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1, 0,1);
		
		VulkanFramebuffer BRDFFramebuffer(device, BRDFRenderPass, { BRDFImageView }, 512, 512, 1);
		BRDFFrameBuffer = BRDFFramebuffer.createFramebuffers();
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

		// To delete buffer data
		objectCreationBuffers.push_back(planeVertexData);
		objectCreationBuffers.push_back(vertexData);
		objectCreationBuffers.push_back(indexData);

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

			objectCreationBuffers.push_back(vertexData);
			objectCreationBuffers.push_back(indexData);

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
				transitionImageLayout(skyboxHDRImage, VK_FORMAT_R32G32B32A32_SFLOAT,
					VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, 0);
				copyBufferToImage(stagingBuffer, skyboxHDRImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 0);
				transitionImageLayout(skyboxHDRImage, VK_FORMAT_R32G32B32A32_SFLOAT,
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
					VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, 0);
				vkDestroyBuffer(device, stagingBuffer, nullptr);
				vkFreeMemory(device, stagingBufferMemory, nullptr);
				
				skyboxHDRImageView = m_VulkanCreateImage->createImageView(skyboxHDRImage, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, 1);
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
				transitionImageLayout(skyboxImage, VK_FORMAT_R8G8B8A8_SRGB,
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
					copyBufferToImage(stagingBuffer, skyboxImage,
						static_cast<uint32_t>(texWidth),
						static_cast<uint32_t>(texHeight), i);
					// Clean up staging resources
					vkDestroyBuffer(device, stagingBuffer, nullptr);
					vkFreeMemory(device, stagingBufferMemory, nullptr);
				}
				// After all faces are copied, transition to shader read layout
				transitionImageLayout(skyboxImage, VK_FORMAT_R8G8B8A8_SRGB,
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
					VK_IMAGE_ASPECT_COLOR_BIT, 6, 1, 0);
				// Create the image view and sampler
				skyboxImageView = m_VulkanCreateImage->createImageView(skyboxImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_VIEW_TYPE_CUBE, VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, 6);
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
				transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, 1, mipLevels, 0);
				copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 0);
				//generateMipmaps(textureImage, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, mipLevels);
				transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, 1, mipLevels, 0);
				vkDestroyBuffer(device, stagingBuffer, nullptr);
				vkFreeMemory(device, stagingBufferMemory, nullptr);

				VkImageView textureImageView = m_VulkanCreateImage->createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, 0, 1);
				if (type == "ambient")
				{
					object.textures.ambient = textureImage;
					object.textures.ambientMemory = textureImageMemory;
					object.textures.imageViews[0] = textureImageView;
				}
				else if (type == "diffuse")
				{
					object.textures.diffuse = textureImage;
					object.textures.diffuseMemory = textureImageMemory;
					object.textures.imageViews[1] = textureImageView;
				}
				else if (type == "specular")
				{
					object.textures.specular = textureImage;
					object.textures.specularMemory = textureImageMemory;
					object.textures.imageViews[2] = textureImageView;
				}
				else if (type == "normal")
				{
					object.textures.specular = textureImage;
					object.textures.specularMemory = textureImageMemory;
					object.textures.imageViews[3] = textureImageView;
				}
				textureImages.push_back(textureImage);
				textureImagesView.push_back(textureImageView);
				textureImagesMemory.push_back(textureImageMemory);

			}
		}
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
			std::vector<VkDescriptorPoolSize> poolSizes;
			int iteration = 0;
			for (auto& [type, flag] : pipeline->descriptorLayout) {
				VkDescriptorPoolSize poolSize{};
				poolSize.type = type;
				poolSize.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * Objects.size());
				poolSizes.push_back(poolSize);
				iteration++;
			}
			VkDescriptorPoolCreateInfo poolInfo{};
			poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
			poolInfo.pPoolSizes = poolSizes.data();
			poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * Objects.size());

			if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &pipeline->descriptorPool) != VK_SUCCESS) {
				throw std::runtime_error("failed to create descriptor pool!");
			}
		}
	}

	void VulkanRendererAPI::createDescriptorSets() {
		for (auto& object : Objects)
		{
			std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, object.pipeline->descriptorSetLayout);
			VkDescriptorSetAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = object.pipeline->descriptorPool;
			allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
			allocInfo.pSetLayouts = layouts.data();

			object.descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);

			if (vkAllocateDescriptorSets(device, &allocInfo, object.descriptorSets.data()) != VK_SUCCESS) {
				throw std::runtime_error("failed to allocate descriptor sets!");
			}
			if (shadowImageView == VK_NULL_HANDLE) {
				throw std::runtime_error("Error: shadowImageView is VK_NULL_HANDLE!");
			}
			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
				std::vector<VkWriteDescriptorSet> descriptorWrites;
				descriptorWrites.clear();

				for (size_t j = 0; j < object.pipeline->bufferSize.size(); j++)
				{
					object.pipeline->descriptionBufferInfo[j].first->buffer = object.uniformBuffers[j][i];
					object.pipeline->descriptionBufferInfo[j].first->offset = 0;
					object.pipeline->descriptionBufferInfo[j].first->range = object.pipeline->bufferSize[j];
				}
				for (size_t j = 0; j < object.pipeline->descriptionSampleInfo.size(); j++)
				{	//for shadow mapping
					if (object.name == "skyBox")
					{
						auto& info = object.pipeline->descriptionSampleInfo[j];
						info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
						info.imageView = HDRCubeView;
						//object.pipeline->descriptionSampleInfo[j]->imageView = skyboxImageView;
						//object.pipeline->descriptionSampleInfo[j]->imageView = prefilterMapView;
						info.sampler = textureSampler;
						
						continue;
					}
					if (object.name == "skyBoxHDR")
					{
						auto& info = object.pipeline->descriptionSampleInfo[j];
						info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
						info.imageView = skyboxHDRImageView;
						info.sampler = textureSampler;
						continue;
					}
					if (object.name == "cubeConv")
					{
						auto& info = object.pipeline->descriptionSampleInfo[j];
						info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
						info.imageView = HDRCubeView;
						info.sampler = textureSampler;
						continue;
					}
					if (object.name == "prefilterCube")
					{
						auto& info = object.pipeline->descriptionSampleInfo[j];
						info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
						info.imageView = HDRCubeView;
						info.sampler = textureSampler;
						continue;
					}
					if (j == 4) {
						auto& info = object.pipeline->descriptionSampleInfo[j];
						if (!&info) {
							throw std::runtime_error("Error: descriptionSampleInfo is nullptr!");
						}
						info.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
						info.imageView = shadowImageView;
						info.sampler = shadowSampler;
						if (info.imageView == VK_NULL_HANDLE) {
							throw std::runtime_error("Error: descriptionSampleInfo.imageView is VK_NULL_HANDLE!");
						}
					}
					else if (j == 5) {
						auto& info = object.pipeline->descriptionSampleInfo[j];
						if (!&info) {
							throw std::runtime_error("Error: descriptionSampleInfo is nullptr!");
						}
						info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
						info.imageView = CubeConvolutionImageView;
						info.sampler = textureSampler;
						if (info.imageView == VK_NULL_HANDLE) {
							throw std::runtime_error("Error: descriptionSampleInfo.imageView is VK_NULL_HANDLE!");
						}
					}
					else if (j == 6) {
						auto& info = object.pipeline->descriptionSampleInfo[j];
						if (!&info) {
							throw std::runtime_error("Error: descriptionSampleInfo is nullptr!");
						}
						info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
						info.imageView = prefilterMapView;
						info.sampler = prefilterTextureSampler;
						if (info.imageView == VK_NULL_HANDLE) {
							throw std::runtime_error("Error: descriptionSampleInfo.imageView is VK_NULL_HANDLE!");
						}
					}
					else if (j == 7) {
						auto& info = object.pipeline->descriptionSampleInfo[j];
						if (!&info) {
							throw std::runtime_error("Error: descriptionSampleInfo is nullptr!");
						}
						info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
						info.imageView = BRDFImageView;
						info.sampler = textureSampler;
						if (info.imageView == VK_NULL_HANDLE) {
							throw std::runtime_error("Error: descriptionSampleInfo.imageView is VK_NULL_HANDLE!");
						}
					}
					else {
						auto& info = object.pipeline->descriptionSampleInfo[j];
						info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
						info.imageView = object.textures.imageViews[j];
						info.sampler = textureSampler;
					}
				}
				int binding = 0;
				for (size_t j = 0; j < object.pipeline->descriptionBufferInfo.size(); j++)
				{

					VkWriteDescriptorSet descriptorWriteBuffer{};
					descriptorWriteBuffer.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWriteBuffer.dstSet = object.descriptorSets[i];
					descriptorWriteBuffer.dstBinding = binding;
					descriptorWriteBuffer.dstArrayElement = 0;
					descriptorWriteBuffer.descriptorType = object.pipeline->descriptorLayout[j].first;
					descriptorWriteBuffer.descriptorCount = 1;
					descriptorWriteBuffer.pBufferInfo = object.pipeline->descriptionBufferInfo[j].first;
					descriptorWrites.push_back(descriptorWriteBuffer);
					binding++;
				}
				//for (size_t j = 0; j < object.pipeline->descriptionSampleInfo.size(); j++)
				for(size_t j = 0; j < object.pipeline->descriptionSampleInfoCount; j++)
				{
					auto& info = object.pipeline->descriptionSampleInfo[j];
					VkWriteDescriptorSet descriptorWriteSample{};
					descriptorWriteSample.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWriteSample.dstSet = object.descriptorSets[i];
					descriptorWriteSample.dstBinding = binding;
					descriptorWriteSample.dstArrayElement = 0;
					descriptorWriteSample.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					descriptorWriteSample.descriptorCount = 1;
					descriptorWriteSample.pImageInfo = &info;
					descriptorWrites.push_back(descriptorWriteSample);
					binding++;
				}
				std::cout << "writes size: " << descriptorWrites.size() << std::endl;
				vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
			}
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
				transitionImageLayout(prefilterMapImage, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, 6, 1, mip);
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
				transitionImageLayout(prefilterMapImage, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, 6, 1, mip);
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

	void VulkanRendererAPI::createSyncObjects() {
		imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
				vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
				vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create synchronization objects for a frame!");
			}
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
		glm::mat4 lightProjection = glm::perspective(glm::radians(90.f), 1.f, 0.1f, 100.f);
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
				memcpy(object.uniformBuffersMapped[1][currentFrame], &colorUbo, sizeof(colorUbo));
			}
			if (object.pipeline->name == "defaultPipeline")
			{
				memcpy(object.uniformBuffersMapped[0][currentImage], &ubo, sizeof(ubo));
				memcpy(object.uniformBuffersMapped[1][currentFrame], pointLights.data(), sizeof(LightAttributes) * pointLights.size());
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

	void VulkanRendererAPI::generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels) {
		VkCommandBuffer commandBuffer = m_VulkanBuffer->beginSingleTimeCommands();

		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, imageFormat, &formatProperties);
		if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
			throw std::runtime_error("texture image format does not support linear blitting!");
		}
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.image = image;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.levelCount = 1;

		int32_t mipWidth = texWidth;
		int32_t mipHeight = texHeight;

		for (uint32_t i = 1; i < mipLevels; i++) {
			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			vkCmdPipelineBarrier(commandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			VkImageBlit blit{};
			blit.srcOffsets[0] = { 0, 0, 0 };
			blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
			blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.mipLevel = i - 1;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = 1;
			blit.dstOffsets[0] = { 0, 0, 0 };
			blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
			blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.mipLevel = i;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount = 1;

			vkCmdBlitImage(commandBuffer,
				image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1, &blit,
				VK_FILTER_LINEAR);

			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(commandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);
			if (mipWidth > 1) mipWidth /= 2;
			if (mipHeight > 1) mipHeight /= 2;
		}

		barrier.subresourceRange.baseMipLevel = mipLevels - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);


		m_VulkanBuffer->endSingleTimeCommands(commandBuffer);
	}

	void VulkanRendererAPI::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlags aspectFlags, int layerCount, uint32_t mipLevels, uint32_t baseMipLevel) {
		VkCommandBuffer commandBuffer = m_VulkanBuffer->beginSingleTimeCommands();

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = aspectFlags;
		barrier.subresourceRange.baseMipLevel = baseMipLevel;
		barrier.subresourceRange.levelCount = mipLevels;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = layerCount;

		VkPipelineStageFlags sourceStage = 0;
		VkPipelineStageFlags destinationStage = 0;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		}
		else {
			throw std::invalid_argument("unsupported layout transition!");
		}

		vkCmdPipelineBarrier(
			commandBuffer,
			sourceStage , destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		m_VulkanBuffer->endSingleTimeCommands(commandBuffer);

	}

	void VulkanRendererAPI::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t baseArrayLayer) {
		VkCommandBuffer commandBuffer = m_VulkanBuffer->beginSingleTimeCommands();

		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = baseArrayLayer;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = {
			width,
			height,
			1
		};

		vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		m_VulkanBuffer->endSingleTimeCommands(commandBuffer);

	}
	
	
	

}