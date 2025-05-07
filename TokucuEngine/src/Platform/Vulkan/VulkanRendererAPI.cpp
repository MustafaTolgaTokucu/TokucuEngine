#include "tkcpch.h"
#include "VulkanRendererAPI.h"

#include <set>

#include "Tokucu/Renderer/Camera.h"
#include "Tokucu/Renderer/FBXLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

namespace Tokucu {
#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

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
		//b_cubeconvulation = false;
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::cout << extensionCount << " extensions supported\n";
	}

	void VulkanRendererAPI::Init(const std::shared_ptr<Window>& window)
	{
		glfwWindow = static_cast<GLFWwindow*>(window->GetNativeWindow());
		
		createInstance();
		setupDebugMessenger();
		createSurface();
		pickPhysicalDevice();
		createLogicalDevice();
		createSwapChain();
		createImageViews();
		createRenderPass();
		registerPipeline();
		createDescriptorSetLayout();
		createColorResources();
		createDepthResources();
		createShadowImage();
		createFramebuffers();
		createCommandPool();
		createGraphicsPipeline();
		createShadowFramebuffer();
		createObject();
		createTextureImage();
		createTextureSampler();
		createUniformBuffers();
		createDescriptorPool();
		createDescriptorSets();
		createCommandBuffers();
		createSyncObjects();
	}
	void VulkanRendererAPI::SetClearColor(const glm::vec4& color)
	{
	}
	void VulkanRendererAPI::Clear()
	{
		vkDeviceWaitIdle(device);

		
		vkDestroyImageView(device, colorImageView, nullptr);
		vkDestroyImage(device, colorImage, nullptr);
		vkFreeMemory(device, colorImageMemory, nullptr);
		
		vkDestroyImageView(device, depthImageView, nullptr);
		vkDestroyImage(device, depthImage, nullptr);
		vkFreeMemory(device, depthImageMemory, nullptr);
		
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

		cleanupSwapChain();

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
		vkDestroyRenderPass(device, renderPass, nullptr);
		vkDestroyRenderPass(device, renderPassHDR, nullptr);
		vkDestroyRenderPass(device, BRDFRenderPass, nullptr);



		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
			vkDestroyFence(device, inFlightFences[i], nullptr);
		}

		vkDestroyCommandPool(device, commandPool, nullptr);
		vkDestroyDevice(device, nullptr);


		if (enableValidationLayers) {
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}
		vkDestroySurfaceKHR(instance, surface, nullptr);
		vkDestroyInstance(instance, nullptr);

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
		VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}
		updateUniformBuffer(currentFrame);

		vkResetFences(device, 1, &inFlightFences[currentFrame]);

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

		VkSwapchainKHR swapChains[] = { swapChain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr; // Optional

		result = vkQueuePresentKHR(presentQueue, &presentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
			TKC_CORE_INFO("Recreating Swap Chain");
			framebufferResized = false;
			recreateSwapChain();
		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image!");
		}
		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}
	void VulkanRendererAPI::Resize(const std::shared_ptr<Window>& window)
	{
		glfwWindow = static_cast<GLFWwindow*>(window->GetNativeWindow());
		framebufferResized = true;
	}

	void VulkanRendererAPI::createInstance() {
		if (enableValidationLayers && !checkValidationLayerSupport()) {
			throw std::runtime_error("validation layers requested, but not available!");
		}
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "TKCRenderer";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "Tokucu Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		auto extensions = getRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
			populateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}
		else {
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}
		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
			throw std::runtime_error("failed to create instance!");
		}
	}

	void VulkanRendererAPI::setupDebugMessenger() {
		if (!enableValidationLayers) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		populateDebugMessengerCreateInfo(createInfo);

		if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
			throw std::runtime_error("failed to set up debug messenger!");
		}
	}
	//we need to create GLWF window first
	void VulkanRendererAPI::createSurface() {
		//GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(m_Window->GetNativeWindow());
		if (glfwWindow)
		{
			if (glfwCreateWindowSurface(instance, glfwWindow, nullptr, &surface) != VK_SUCCESS) {
				throw std::runtime_error("failed to create window surface!");
			}
		}
		else
		{
			throw std::runtime_error("GLFW window is not created!");
		}
	}
	//we need to check if the physical device is suitable for our needs
	void VulkanRendererAPI::pickPhysicalDevice() {
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
		if (deviceCount == 0) {
			throw std::runtime_error("failed to find GPUs with Vulkan support!");
		}
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
		for (const auto& device : devices) {
			if (isDeviceSuitable(device)) {
				physicalDevice = device;
				msaaSamples = getMaxUsableSampleCount();
				break;
			}
		}

		if (physicalDevice == VK_NULL_HANDLE) {
			throw std::runtime_error("failed to find a suitable GPU!");
		}
	}
	//we need to create logical device to interact with the physical device
	void VulkanRendererAPI::createLogicalDevice() {
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};
		deviceFeatures.samplerAnisotropy = VK_TRUE;
		deviceFeatures.geometryShader = VK_TRUE;
		deviceFeatures.imageCubeArray = VK_TRUE;
		deviceFeatures.sampleRateShading = VK_TRUE; // enable sample shading feature for the device

		//VkPhysicalDeviceMultiviewFeatures multiviewFeatures{};
		//multiviewFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES;
		//multiviewFeatures.multiview = VK_TRUE; // Enable multiview
		//multiviewFeatures.multiviewGeometryShader = VK_TRUE;

		//VkPhysicalDeviceImagelessFramebufferFeatures imagelessFramebufferFeatures{};
		//imagelessFramebufferFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES;
		//imagelessFramebufferFeatures.imagelessFramebuffer = VK_TRUE;

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		//createInfo.pNext = &multiviewFeatures; // Chain multiview features
		//createInfo.pNext = &imagelessFramebufferFeatures;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();

		createInfo.pEnabledFeatures = &deviceFeatures;

		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}
		if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
			throw std::runtime_error("failed to create logical device!");
		}
		vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
		vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
	}
	//we need to query the swap chain support details
	void VulkanRendererAPI::createSwapChain() {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}
		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;

		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		if (indices.graphicsFamily != indices.presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
			throw std::runtime_error("failed to create swap chain!");
		}

		vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
		swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

		swapChainImageFormat = surfaceFormat.format;
		swapChainExtent = extent;
	}
	
	void VulkanRendererAPI::cleanupSwapChain() {
		for (auto framebuffer : swapChainFramebuffers) {
			vkDestroyFramebuffer(device, framebuffer, nullptr);
		}

		for (auto imageView : swapChainImageViews) {

			if (imageView != VK_NULL_HANDLE) {
				vkDestroyImageView(device, imageView, nullptr);
				imageView = VK_NULL_HANDLE;
			}
		}
		vkDestroySwapchainKHR(device, swapChain, nullptr);
	}

	void VulkanRendererAPI::recreateSwapChain() {
		int width = 0, height = 0;
		//GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(window->GetNativeWindow());
		glfwGetFramebufferSize(glfwWindow, &width, &height);
		while (width == 0 || height == 0) {
			glfwGetFramebufferSize(glfwWindow, &width, &height);
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(device);
		cleanupSwapChain();
		createSwapChain();
		createImageViews();
		createColorResources();
		createDepthResources();
		createFramebuffers();
	}

	void VulkanRendererAPI::createImageViews() {
		swapChainImageViews.resize(swapChainImages.size());
		VkImageAspectFlags swapChainAspectFlags;
		if (swapChainImageFormat == VK_FORMAT_D32_SFLOAT || swapChainImageFormat == VK_FORMAT_D16_UNORM) {
			swapChainAspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;
		}
		else if (swapChainImageFormat == VK_FORMAT_D24_UNORM_S8_UINT || swapChainImageFormat == VK_FORMAT_D32_SFLOAT_S8_UINT) {
			swapChainAspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		}
		else {
			swapChainAspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;

		}
		for (uint32_t i = 0; i < swapChainImages.size(); i++) {
			swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainImageFormat, swapChainAspectFlags, mipLevels);
		}

	}

	void VulkanRendererAPI::createRenderPass() {
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = swapChainImageFormat;
		colorAttachment.samples = msaaSamples;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription colorAttachmentResolve{};
		colorAttachmentResolve.format = swapChainImageFormat;
		colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentResolveRef{};
		colorAttachmentResolveRef.attachment = 2;
		colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription depthAttachment{};
		depthAttachment.format = findDepthFormat();
		depthAttachment.samples = msaaSamples;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;
		subpass.pResolveAttachments = &colorAttachmentResolveRef;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

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
			throw std::runtime_error("failed to create render pass!");
		}

		VkAttachmentDescription shadowDepthAttachment{};
		shadowDepthAttachment.format = findDepthFormat();
		shadowDepthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		shadowDepthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		shadowDepthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		shadowDepthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		shadowDepthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		shadowDepthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		shadowDepthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

		VkAttachmentReference shadowDepthAttachmentRef{};
		shadowDepthAttachmentRef.attachment = 0;
		shadowDepthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription shadowSubpass{};
		shadowSubpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		shadowSubpass.pDepthStencilAttachment = &shadowDepthAttachmentRef;
		shadowSubpass.colorAttachmentCount = 0;
		shadowSubpass.pColorAttachments = nullptr;

		VkSubpassDependency dependencies{};
		dependencies.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependencies.dstSubpass = 0;
		dependencies.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		dependencies.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependencies.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		dependencies.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		dependencies.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		VkRenderPassCreateInfo renderPassCreateInfo{};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.attachmentCount = 1;
		renderPassCreateInfo.pAttachments = &shadowDepthAttachment;
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &shadowSubpass;
		renderPassCreateInfo.dependencyCount = 1;
		renderPassCreateInfo.pDependencies = &dependencies;

		if (vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &shadowRenderPass)) {
			throw std::runtime_error("failed to create render pass!");
		}

		//////////////////////////////////////////
		//for skybox hdr
		//////////////////////////////////////////
		VkAttachmentDescription colorAttachmentHDR{};
		colorAttachmentHDR.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		colorAttachmentHDR.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachmentHDR.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachmentHDR.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachmentHDR.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentHDR.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachmentHDR.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachmentHDR.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		// Create attachment reference
		VkAttachmentReference colorAttachmentRefHDR{};
		colorAttachmentRefHDR.attachment = 0;
		colorAttachmentRefHDR.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		// Create subpass
		VkSubpassDescription subpassHDR{};
		subpassHDR.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassHDR.colorAttachmentCount = 1;
		subpassHDR.pColorAttachments = &colorAttachmentRefHDR;

		//std::array<VkAttachmentDescription, 3> attachments2 = { colorAttachmentHDR, depthAttachment, colorAttachmentResolveHDR };
		VkRenderPassCreateInfo renderPassInfoHDR{};
		renderPassInfoHDR.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		//renderPassInfoHDR.attachmentCount = static_cast<uint32_t>(attachments2.size());
		renderPassInfoHDR.attachmentCount = 1;
		renderPassInfoHDR.pAttachments = &colorAttachmentHDR;
		renderPassInfoHDR.subpassCount = 1;
		renderPassInfoHDR.pSubpasses = &subpassHDR;
		renderPassInfoHDR.dependencyCount = 1;
		renderPassInfoHDR.pDependencies = &dependency;

		if (vkCreateRenderPass(device, &renderPassInfoHDR, nullptr, &renderPassHDR) != VK_SUCCESS) {
			throw std::runtime_error("failed to create render pass!");
		}
		//////////////////////////////////////////
		//for BRDF LUT
		//////////////////////////////////////////
		VkAttachmentDescription colorAttachmentBRDF{};
		colorAttachmentBRDF.format = VK_FORMAT_R32G32_SFLOAT;
		colorAttachmentBRDF.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachmentBRDF.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachmentBRDF.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachmentBRDF.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentBRDF.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachmentBRDF.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachmentBRDF.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		// Create attachment reference
		VkAttachmentReference colorAttachmentRefBRDF{};
		colorAttachmentRefBRDF.attachment = 0;
		colorAttachmentRefBRDF.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		// Create subpass
		VkSubpassDescription subpassBRDF{};
		subpassBRDF.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassBRDF.colorAttachmentCount = 1;
		subpassBRDF.pColorAttachments = &colorAttachmentRefBRDF;

		VkRenderPassCreateInfo renderPassInfoBRDF{};
		renderPassInfoBRDF.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfoBRDF.attachmentCount = 1;
		renderPassInfoBRDF.pAttachments = &colorAttachmentBRDF;
		renderPassInfoBRDF.subpassCount = 1;
		renderPassInfoBRDF.pSubpasses = &subpassBRDF;
		renderPassInfoBRDF.dependencyCount = 1;
		renderPassInfoBRDF.pDependencies = &dependency;

		if (vkCreateRenderPass(device, &renderPassInfoBRDF, nullptr, &BRDFRenderPass) != VK_SUCCESS) {
			throw std::runtime_error("failed to create render pass!");
		}
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
			{{&ambientInfo},{&diffuseInfo},{&specularInfo},{&normalInfo},{&shadowInfo},{&irradianceInfo},{&prefilterMapInfo},{&BRDFInfo}},
			{{sizeof(UniformBufferObject)},{sizeof(LightAttributes) * pointLights.size()},{sizeof(shadowUBO) * 2}} };
		Pipelines.push_back(&m_Pipeline);
		//Pipeline for point light object representives
		m_Pipeline2 = { "lightPipeline" ,"assets/shaders/vertPL.spv", "assets/shaders/fragPL.spv",{},
			{{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT},
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT}},
			{{&transformBufferInfo,VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT},{&colorBufferInfo,VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT}},{},{{sizeof(UniformBufferObject)},{sizeof(ColorUniform)}} };
		Pipelines.push_back(&m_Pipeline2);
		//Pipeline for skybox rendering using 6 layered cubemap
		m_PipelineSkybox = { "skyboxPipeline" ,"assets/shaders/skyboxVert.spv", "assets/shaders/skyboxFrag.spv",{},
			{{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT}},
			{{&transformBufferInfo,VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT}},{{&skyboxInfo} }, { {sizeof(UniformBufferObject)} }
		};
		Pipelines.push_back(&m_PipelineSkybox);
		//For Cubemap convulation will be usen in diffuse IBL
		m_PipelineCubeConv = { "CubeConvPipeline" ,"assets/shaders/cubemapConvVert.spv", "assets/shaders/cubemapConvFrag.spv","assets/shaders/cubemapConvGeom.spv",
			{{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT},
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_GEOMETRY_BIT},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT}},
			{{&transformBufferInfo,VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT},{&cubemapPosMatInfo,VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT}},{{&cubeConvInfo} }, { {sizeof(UniformBufferObject)},{sizeof(glm::mat4) * 6} }
		};
		Pipelines.push_back(&m_PipelineCubeConv);
		//HDR Image to cubemap conversion (equirectengular 2D to cube)
		m_PipelineSkyboxHDR = { "skyboxPipelineHDR" ,"assets/shaders/skyboxHDRVert.spv", "assets/shaders/skyboxHDRFrag.spv","assets/shaders/skyboxHDRGeom.spv",
			{{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT},
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_GEOMETRY_BIT},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT}},
			{{&transformBufferInfo,VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT},{&cubemapPosInfo,VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT}},{{&skyboxHDRInfo}}, {{sizeof(UniformBufferObject)},{sizeof(glm::mat4) * 6}}
		};
		Pipelines.push_back(&m_PipelineSkyboxHDR);
		//Prefiltering the cubemap for specular IBL
		m_PipelinePrefilter = { "prefilterPipeline" ,"assets/shaders/prefilterShaderVert.spv", "assets/shaders/prefilterShaderFrag.spv","assets/shaders/prefilterShaderGeom.spv",
			{{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT},
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_GEOMETRY_BIT},
			//{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_FRAGMENT_BIT},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT}},
			{{&transformBufferInfo,VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT},{&prefilterPosInfo,VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT}},{{&prefilterInfo}}, {{sizeof(UniformBufferObject)},{sizeof(glm::mat4) * 6}}
		};
		Pipelines.push_back(&m_PipelinePrefilter);
		//BRDF LUT for specular IBL
		m_PipelineBRDF = { "BRDFPipeline" ,"assets/shaders/BRDFVert.spv", "assets/shaders/BRDFFrag.spv",{},
			{{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT}},
			{{&transformBufferInfo,VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT}},{}, {{sizeof(UniformBufferObject)}}
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
		{{&ambientInfo},{&diffuseInfo},{&specularInfo},{&normalInfo},{&shadowInfo},{&irradianceInfo},{&prefilterMapInfo},{&BRDFInfo}},
		{ {sizeof(UniformBufferObject)},{sizeof(LightAttributes) * pointLights.size()},{sizeof(shadowUBO) * 2} } };
		Pipelines.push_back(&m_PipelineShadow);
	}

	void VulkanRendererAPI::createDescriptorSetLayout() {
		for (auto& pipeline : Pipelines)
		{
			std::vector<VkDescriptorSetLayoutBinding> bindings;
			int iteration = 0;
			for (auto& [type, flag] : pipeline->descriptorLayout)
			{
				std::cout << "Descriptor Type: " << type << ", Shader Stage: " << flag << std::endl;
				VkDescriptorSetLayoutBinding LayoutBinding{};
				LayoutBinding.binding = iteration;
				LayoutBinding.descriptorCount = 1;
				LayoutBinding.descriptorType = type;
				LayoutBinding.pImmutableSamplers = nullptr;
				LayoutBinding.stageFlags = flag;
				bindings.push_back(LayoutBinding);
				iteration++;
			}
			VkDescriptorSetLayoutCreateInfo layoutInfo{};
			layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
			layoutInfo.pBindings = bindings.data();

			if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &pipeline->descriptorSetLayout) != VK_SUCCESS) {
				throw std::runtime_error("failed to create descriptor set layout!");
			}

		}
	}

	void VulkanRendererAPI::createGraphicsPipeline() {
		for (auto& pipeline : Pipelines) {

			auto vertShaderCode = readFile(pipeline->vertexShader);
			auto fragShaderCode = readFile(pipeline->fragmentShader);

			VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
			VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

			VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
			vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
			vertShaderStageInfo.module = vertShaderModule;
			vertShaderStageInfo.pName = "main";

			VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
			fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			fragShaderStageInfo.module = fragShaderModule;
			fragShaderStageInfo.pName = "main";

			VkShaderModule geoShaderModule = VK_NULL_HANDLE;
			std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
			if (strlen(pipeline->geometryShader) > 0) {
				auto geoShaderCode = readFile(pipeline->geometryShader);
				geoShaderModule = createShaderModule(geoShaderCode);

				VkPipelineShaderStageCreateInfo geoShaderStageInfo{};
				geoShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				geoShaderStageInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
				geoShaderStageInfo.module = geoShaderModule;
				geoShaderStageInfo.pName = "main";

				shaderStages.push_back(geoShaderStageInfo);
			}
			shaderStages.push_back(vertShaderStageInfo);
			shaderStages.push_back(fragShaderStageInfo);

			VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
			vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			auto bindingDescription = getBindingDescription();
			auto attributeDescriptions = getAttributeDescriptions();

			vertexInputInfo.vertexBindingDescriptionCount = 1;
			vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
			vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
			vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

			VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
			inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			inputAssembly.primitiveRestartEnable = VK_FALSE;

			VkPipelineViewportStateCreateInfo viewportState{};
			viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewportState.viewportCount = 1;
			viewportState.scissorCount = 1;

			VkPipelineRasterizationStateCreateInfo rasterizer{};
			rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rasterizer.depthClampEnable = VK_FALSE;
			rasterizer.rasterizerDiscardEnable = VK_FALSE;
			rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
			rasterizer.lineWidth = 1.0f;
			rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
			rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
			rasterizer.depthBiasEnable = VK_FALSE;
			if (pipeline->name == "skyboxPipeline" /* || pipeline->name == "skyboxPipelineHDR"*/)
			{
				rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
			}
			else if (pipeline->name == "BRDFPipeline")
			{
				rasterizer.cullMode = VK_CULL_MODE_NONE;
			}

			VkPipelineMultisampleStateCreateInfo multisampling{};
			multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multisampling.sampleShadingEnable = VK_FALSE;
			if (pipeline->name == "shadowPipeline" || pipeline->name == "skyboxPipelineHDR" || pipeline->name == "CubeConvPipeline" || pipeline->name == "prefilterPipeline" || pipeline->name == "BRDFPipeline") {
				multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
			}
			else
			{
				multisampling.rasterizationSamples = msaaSamples;
				multisampling.sampleShadingEnable = VK_TRUE; // enable sample shading in the pipeline
				multisampling.minSampleShading = .2f; // min fraction for sample shading
			}

			VkPipelineColorBlendAttachmentState colorBlendAttachment{};
			colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			colorBlendAttachment.blendEnable = VK_FALSE;


			VkPipelineColorBlendStateCreateInfo colorBlending{};

			if (pipeline->name == "shadowPipeline")
			{
				colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
				colorBlending.attachmentCount = 0;  // No color attachments
				colorBlending.pAttachments = nullptr;
			}
			else
			{
				colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
				colorBlending.logicOpEnable = VK_FALSE;
				colorBlending.logicOp = VK_LOGIC_OP_COPY;
				colorBlending.attachmentCount = 1;
				colorBlending.pAttachments = &colorBlendAttachment;
				colorBlending.blendConstants[0] = 0.0f;
				colorBlending.blendConstants[1] = 0.0f;
				colorBlending.blendConstants[2] = 0.0f;
				colorBlending.blendConstants[3] = 0.0f;

			}

			VkPipelineDepthStencilStateCreateInfo depthStencil{};
			depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			depthStencil.depthTestEnable = VK_TRUE;
			depthStencil.depthWriteEnable = VK_TRUE;
			depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
			depthStencil.depthBoundsTestEnable = VK_FALSE;
			depthStencil.stencilTestEnable = VK_FALSE;

			VkPushConstantRange pushConstantRange{};
			pushConstantRange.stageFlags = VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
			pushConstantRange.offset = 0;
			pushConstantRange.size = sizeof(int);

			std::vector<VkDynamicState> dynamicStates = {
				VK_DYNAMIC_STATE_VIEWPORT,
				VK_DYNAMIC_STATE_SCISSOR
			};

			VkPipelineDynamicStateCreateInfo dynamicState{};
			dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
			dynamicState.pDynamicStates = dynamicStates.data();

			VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
			pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutInfo.setLayoutCount = 1;
			pipelineLayoutInfo.pSetLayouts = &pipeline->descriptorSetLayout;
			pipelineLayoutInfo.pushConstantRangeCount = 1;
			pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

			if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipeline->pipelineLayout) != VK_SUCCESS) {
				throw std::runtime_error("failed to create pipeline layout!");
			}

			VkGraphicsPipelineCreateInfo pipelineInfo{};
			pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			pipelineInfo.stageCount = shaderStages.size();
			pipelineInfo.pStages = shaderStages.data();
			pipelineInfo.pVertexInputState = &vertexInputInfo;
			pipelineInfo.pInputAssemblyState = &inputAssembly;
			pipelineInfo.pViewportState = &viewportState;
			pipelineInfo.pRasterizationState = &rasterizer;
			pipelineInfo.pMultisampleState = &multisampling;
			pipelineInfo.pDepthStencilState = &depthStencil;
			pipelineInfo.pDynamicState = &dynamicState;
			if (pipeline->name == "shadowPipeline")
			{
				pipelineInfo.pColorBlendState = nullptr;
				pipelineInfo.layout = pipeline->pipelineLayout;
				pipelineInfo.renderPass = shadowRenderPass;
			}
			else if (pipeline->name == "skyboxPipelineHDR" || pipeline->name == "CubeConvPipeline" || pipeline->name == "prefilterPipeline" /* || pipeline->name == "BRDFPipeline" */)
			{
				pipelineInfo.pColorBlendState = &colorBlending;
				pipelineInfo.layout = pipeline->pipelineLayout;
				pipelineInfo.renderPass = renderPassHDR;
			}

			else
			{
				pipelineInfo.pColorBlendState = &colorBlending;
				pipelineInfo.layout = pipeline->pipelineLayout;
				pipelineInfo.renderPass = renderPass;
			}
			if (pipeline->name == "BRDFPipeline")
			{
				pipelineInfo.pColorBlendState = &colorBlending;
				pipelineInfo.layout = pipeline->pipelineLayout;
				pipelineInfo.renderPass = BRDFRenderPass;
			}
			pipelineInfo.subpass = 0;
			pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; 
			pipelineInfo.basePipelineIndex = -1; 

			if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline->pipeline) != VK_SUCCESS) {
				throw std::runtime_error("failed to create graphics pipeline!");
			}

			vkDestroyShaderModule(device, fragShaderModule, nullptr);
			vkDestroyShaderModule(device, vertShaderModule, nullptr);
			if (geoShaderModule != VK_NULL_HANDLE)
			{
				vkDestroyShaderModule(device, geoShaderModule, nullptr);
			}
		}
	}

	void VulkanRendererAPI::createFramebuffers() {
		// Do not create another frambuffer here, affects recreating swapchain
		swapChainFramebuffers.resize(swapChainImageViews.size());
		for (size_t i = 0; i < swapChainImageViews.size(); i++) {
			std::array<VkImageView, 3> attachments = { colorImageView,depthImageView,swapChainImageViews[i] };
			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = swapChainExtent.width;
			framebufferInfo.height = swapChainExtent.height;
			framebufferInfo.layers = 1;
			if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create framebuffer!");
			}
		}
	}

	void VulkanRendererAPI::createShadowFramebuffer() {
		//////////////////////////////////////////
		// TODO: Create general framebuffer function for better organization
		// /////////////////////////////////////
			// Framebuffer for Shadow Mapping (Cubemap)
		VkFramebufferCreateInfo shadowFramebufferInfo{};
		shadowFramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		shadowFramebufferInfo.renderPass = shadowRenderPass;
		shadowFramebufferInfo.attachmentCount = 1;
		shadowFramebufferInfo.pAttachments = &shadowImageView;
		shadowFramebufferInfo.width = shadowMapSize;
		shadowFramebufferInfo.height = shadowMapSize;
		shadowFramebufferInfo.layers = 12;
		if (vkCreateFramebuffer(device, &shadowFramebufferInfo, nullptr, &shadowFrameBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}

		// Framebuffer for HDR Cubemap
		createCubemapImage(1024, 1024, 1, VK_SAMPLE_COUNT_1_BIT,
			VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			HDRCubeImage, HDRCubeImageMemory, 6);
		HDRCubeView = createCubemapImageView(HDRCubeImage, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT, 1, 0);

		VkFramebufferCreateInfo framebufferInfoHDR{};
		framebufferInfoHDR.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfoHDR.renderPass = renderPassHDR;
		framebufferInfoHDR.attachmentCount = 1;
		framebufferInfoHDR.pAttachments = &HDRCubeView;
		framebufferInfoHDR.width = 1024;
		framebufferInfoHDR.height = 1024;
		framebufferInfoHDR.layers = 6;
		if (vkCreateFramebuffer(device, &framebufferInfoHDR, nullptr, &HDRCubeFrameBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
		createCubemapImage(32, 32, 1, VK_SAMPLE_COUNT_1_BIT,
			VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			CubeConvolutionImage, CubeConvolutionImageMemory, 6);
		CubeConvolutionImageView = createCubemapImageView(CubeConvolutionImage, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT, 1, 0);

		VkFramebufferCreateInfo framebufferInfoCube{};
		framebufferInfoCube.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfoCube.renderPass = renderPassHDR;
		framebufferInfoCube.attachmentCount = 1;
		framebufferInfoCube.pAttachments = &CubeConvolutionImageView;
		framebufferInfoCube.width = 32;
		framebufferInfoCube.height = 32;
		framebufferInfoCube.layers = 6;
		if (vkCreateFramebuffer(device, &framebufferInfoCube, nullptr, &CubeConvolutionFrameBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}

		createImage(512, 512, 1, VK_SAMPLE_COUNT_1_BIT,
			VK_FORMAT_R32G32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			BRDFImage, BRDFImageMemory, 1);
		BRDFImageView = createImageView(BRDFImage, VK_FORMAT_R32G32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT, 1);

		VkFramebufferCreateInfo BRDFFramebufferInfo{};
		BRDFFramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		BRDFFramebufferInfo.renderPass = BRDFRenderPass;
		BRDFFramebufferInfo.attachmentCount = 1;
		BRDFFramebufferInfo.pAttachments = &BRDFImageView;
		BRDFFramebufferInfo.width = 512;
		BRDFFramebufferInfo.height = 512;
		BRDFFramebufferInfo.layers = 1;
		if (vkCreateFramebuffer(device, &BRDFFramebufferInfo, nullptr, &BRDFFrameBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}

	/*VkFramebuffer VulkanRendererAPI::createPrefilterFrameBuffer(uint16_t width, uint16_t height, uint16_t miplevel)
	{
		prefilterMipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(prefilterMapResolution, prefilterMapResolution)))) + 1;
		//When creating a framebuffer, each attachment must reference only a single mip level.
		VkImageView prefilterMapTempView = createCubemapImageView(prefilterMapImage, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT, 1, miplevel);
		// Create Framebuffer for prefilter map
		VkFramebuffer prefilterMapFramebuffer;
		VkFramebufferCreateInfo prefilterFramebufferInfo{};
		prefilterFramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		prefilterFramebufferInfo.renderPass = renderPassHDR;
		prefilterFramebufferInfo.attachmentCount = 1;
		prefilterFramebufferInfo.pAttachments = &prefilterMapTempView;
		prefilterFramebufferInfo.width = width;
		prefilterFramebufferInfo.height = height;
		prefilterFramebufferInfo.layers = 6;
		if (vkCreateFramebuffer(device, &prefilterFramebufferInfo, nullptr, &prefilterMapFramebuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
		return prefilterMapFramebuffer;
	}*/

	void VulkanRendererAPI::createCommandPool() {
		QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

		if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create command pool!");
		}
	}

	void VulkanRendererAPI::createDepthResources() {
		VkFormat depthFormat = findDepthFormat();
		createImage(swapChainExtent.width, swapChainExtent.height, mipLevels, msaaSamples, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory, 1);
		depthImageView = createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, mipLevels);
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

	BufferData VulkanRendererAPI::createVertexBuffer(const std::vector<Vertex>& vertexData) {
		VkDeviceSize bufferSize = sizeof(vertexData[0]) * vertexData.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertexData.data(), (size_t)bufferSize);
		vkUnmapMemory(device, stagingBufferMemory);

		VkBuffer vertexBuffer = VK_NULL_HANDLE;
		VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

		copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);
		return { vertexBuffer, vertexBufferMemory };
	}

	BufferData VulkanRendererAPI::createIndexBuffer(const std::vector<uint32_t>& intexData) {
		VkDeviceSize bufferSize = sizeof(intexData[0]) * intexData.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, intexData.data(), (size_t)bufferSize);
		vkUnmapMemory(device, stagingBufferMemory);
		VkBuffer indexBuffer = VK_NULL_HANDLE;
		VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;
		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

		copyBuffer(stagingBuffer, indexBuffer, bufferSize);

		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);
		return { indexBuffer, indexBufferMemory };
	}

	void VulkanRendererAPI::createObject() {
		BufferData vertexData;
		BufferData indexData;
		
		vertexData = createVertexBuffer(cubeVertices);
		indexData = createIndexBuffer(cubeIndices);
		BufferData planeVertexData = createVertexBuffer(secondVertices);
		BufferData planeIndexData = createIndexBuffer(secondIndices);
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

		VulkanObject pointLight1 = { "pointLight1", cubeVertices,cubeIndices, vertexData.buffer, vertexData.memory,indexData.buffer, indexData.memory, getBindingDescription() , getAttributeDescriptions(),&m_Pipeline2,false,std::nullopt };
		Objects.push_back(pointLight1);

		VulkanObject pointLight2 = { "pointLight2", cubeVertices,cubeIndices, vertexData.buffer, vertexData.memory,indexData.buffer, indexData.memory, getBindingDescription() , getAttributeDescriptions(),&m_Pipeline2,false,std::nullopt };
		Objects.push_back(pointLight2);

		VulkanObject skybox = { "skyBox", cubeVertices,cubeIndices, vertexData.buffer, vertexData.memory,indexData.buffer, indexData.memory, getBindingDescription() , getAttributeDescriptions(),&m_PipelineSkybox,false,std::nullopt };
		Objects.push_back(skybox);

		VulkanObject skyboxHDR = { "skyBoxHDR", cubeVertices,cubeIndices, vertexData.buffer, vertexData.memory,indexData.buffer, indexData.memory, getBindingDescription() , getAttributeDescriptions(),&m_PipelineSkyboxHDR,false,std::nullopt,
		{{"skyboxHDR","assets/textures/qwantani_noon_4k.hdr"}} };
		Objects.push_back(skyboxHDR);

		VulkanObject cubeConv = { "cubeConv", cubeVertices,cubeIndices, vertexData.buffer, vertexData.memory,indexData.buffer, indexData.memory, getBindingDescription() , getAttributeDescriptions(),&m_PipelineCubeConv,false,std::nullopt };
		Objects.push_back(cubeConv);

		VulkanObject prefilterCube = { "prefilterCube", cubeVertices,cubeIndices, vertexData.buffer, vertexData.memory,indexData.buffer, indexData.memory, getBindingDescription() , getAttributeDescriptions(),&m_PipelinePrefilter,false,std::nullopt };
		Objects.push_back(prefilterCube);

		VulkanObject BRDFLud = { "BRDFLud", secondVertices,secondIndices, planeVertexData.buffer, planeVertexData.memory,planeIndexData.buffer, planeIndexData.memory,
			getBindingDescription() , getAttributeDescriptions(),&m_PipelineBRDF,false,std::nullopt };
		Objects.push_back(BRDFLud);

		createModel("Shotgun", "assets/models/shotgun/ShotgunTri.fbx", "assets/textures/Shotgun/");
		createModel("Helmet", "assets/models/Helmet/sci_fi_space_helmet_by_aliashasim.fbx", "assets/textures/Helmet/");
		createModel("Glock", "assets/models/Glock/MDL_Glock.fbx", "assets/textures/Glock/");

	}

	void VulkanRendererAPI::createModel(std::string modelName, std::string modelLocation, std::string textureLocation) {
		std::unique_ptr<FBXLoader> loader = std::make_unique<FBXLoader>(modelLocation.c_str());
		FBXModelData model = loader->getModelData();

		std::vector<std::pair<std::string, std::string>> materialTextureLocations;
		for (int i = 0; i < model.meshes.size(); i++) {
			auto& mesh = model.meshes[i];
			BufferData vertexData = createVertexBuffer(mesh.vertices);
			BufferData indexData = createIndexBuffer(mesh.indices);

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
				getBindingDescription(),
				getAttributeDescriptions(),
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
				createCubemapImage(prefilterMapResolution, prefilterMapResolution, prefilterMipLevels, VK_SAMPLE_COUNT_1_BIT,
					VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
					VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
					prefilterMapImage, prefilterMapImageMemory, 6);
				prefilterMapView = createCubemapImageView(prefilterMapImage, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT, prefilterMipLevels, 0);
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
				createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					stagingBuffer, stagingBufferMemory);
				void* data;
				vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
				memcpy(data, pixels, static_cast<size_t>(imageSize));
				vkUnmapMemory(device, stagingBufferMemory);
				stbi_image_free(pixels);
				createImage(texWidth, texHeight, 1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R32G32B32A32_SFLOAT,
					VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
					VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, skyboxHDRImage,
					skyboxHDRImageMemory, 1);
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
				skyboxHDRImageView = createImageView(skyboxHDRImage, VK_FORMAT_R32G32B32A32_SFLOAT,
					VK_IMAGE_ASPECT_COLOR_BIT, 1);
				continue;
			}
			// Create texture image for skybox having 6 layers (cubemap)
			if (object.name == "skyBox")
			{
				int texWidth = 2048;  // Use consistent size for all faces
				int texHeight = 2048;
				// Create a single cubemap image that will hold all 6 faces
				createCubemapImage(texWidth, texHeight, 1, VK_SAMPLE_COUNT_1_BIT,
					VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
					VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
					skyboxImage, skyboxImageMemory, 6);
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
					createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
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
				skyboxImageView = createCubemapImageView(skyboxImage, VK_FORMAT_R8G8B8A8_SRGB,
					VK_IMAGE_ASPECT_COLOR_BIT, 1, 0);
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
				createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

				void* data;
				vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
				memcpy(data, pixels, static_cast<size_t>(imageSize));
				vkUnmapMemory(device, stagingBufferMemory);

				stbi_image_free(pixels);

				VkImage textureImage = VK_NULL_HANDLE;
				VkDeviceMemory textureImageMemory = VK_NULL_HANDLE;

				createImage(texWidth, texHeight, mipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory, 1);
				transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, 1, mipLevels, 0);
				copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 0);
				//generateMipmaps(textureImage, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, mipLevels);
				transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, 1, mipLevels, 0);
				vkDestroyBuffer(device, stagingBuffer, nullptr);
				vkFreeMemory(device, stagingBufferMemory, nullptr);

				VkImageView textureImageView = createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels);
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

		for (auto& object : Objects) {
			size_t numUBOs = object.pipeline->bufferSize.size();
			object.uniformBuffers.resize(numUBOs);
			object.uniformBuffersMemory.resize(numUBOs);
			object.uniformBuffersMapped.resize(numUBOs);

			for (size_t uboIndex = 0; uboIndex < numUBOs; uboIndex++) {
				VkDeviceSize bufferSize = object.pipeline->bufferSize[uboIndex];

				object.uniformBuffers[uboIndex].resize(MAX_FRAMES_IN_FLIGHT);
				object.uniformBuffersMemory[uboIndex].resize(MAX_FRAMES_IN_FLIGHT);
				object.uniformBuffersMapped[uboIndex].resize(MAX_FRAMES_IN_FLIGHT);

				for (size_t frame = 0; frame < MAX_FRAMES_IN_FLIGHT; frame++) {
					createBuffer(bufferSize, object.pipeline->descriptionBufferInfo[uboIndex].second,
						VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
						object.uniformBuffers[uboIndex][frame], object.uniformBuffersMemory[uboIndex][frame]);

					vkMapMemory(device, object.uniformBuffersMemory[uboIndex][frame], 0, bufferSize, 0,
						&object.uniformBuffersMapped[uboIndex][frame]);
				}
			}
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
						object.pipeline->descriptionSampleInfo[j]->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
						object.pipeline->descriptionSampleInfo[j]->imageView = HDRCubeView;
						//object.pipeline->descriptionSampleInfo[j]->imageView = skyboxImageView;
						//object.pipeline->descriptionSampleInfo[j]->imageView = prefilterMapView;
						object.pipeline->descriptionSampleInfo[j]->sampler = textureSampler;
						continue;
					}
					if (object.name == "skyBoxHDR")
					{
						object.pipeline->descriptionSampleInfo[j]->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
						object.pipeline->descriptionSampleInfo[j]->imageView = skyboxHDRImageView;
						object.pipeline->descriptionSampleInfo[j]->sampler = textureSampler;
						continue;
					}
					if (object.name == "cubeConv")
					{
						object.pipeline->descriptionSampleInfo[j]->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
						object.pipeline->descriptionSampleInfo[j]->imageView = HDRCubeView;
						object.pipeline->descriptionSampleInfo[j]->sampler = textureSampler;
						continue;
					}
					if (object.name == "prefilterCube")
					{
						object.pipeline->descriptionSampleInfo[j]->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
						object.pipeline->descriptionSampleInfo[j]->imageView = HDRCubeView;
						object.pipeline->descriptionSampleInfo[j]->sampler = textureSampler;
						continue;
					}
					if (j == 4) {
						if (!object.pipeline->descriptionSampleInfo[j]) {
							throw std::runtime_error("Error: descriptionSampleInfo is nullptr!");
						}
						object.pipeline->descriptionSampleInfo[j]->imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
						object.pipeline->descriptionSampleInfo[j]->imageView = shadowImageView;
						object.pipeline->descriptionSampleInfo[j]->sampler = shadowSampler;
						if (object.pipeline->descriptionSampleInfo[j]->imageView == VK_NULL_HANDLE) {
							throw std::runtime_error("Error: descriptionSampleInfo.imageView is VK_NULL_HANDLE!");
						}
					}
					else if (j == 5) {
						if (!object.pipeline->descriptionSampleInfo[j]) {
							throw std::runtime_error("Error: descriptionSampleInfo is nullptr!");
						}
						object.pipeline->descriptionSampleInfo[j]->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
						object.pipeline->descriptionSampleInfo[j]->imageView = CubeConvolutionImageView;
						object.pipeline->descriptionSampleInfo[j]->sampler = textureSampler;
						if (object.pipeline->descriptionSampleInfo[j]->imageView == VK_NULL_HANDLE) {
							throw std::runtime_error("Error: descriptionSampleInfo.imageView is VK_NULL_HANDLE!");
						}
					}
					else if (j == 6) {
						if (!object.pipeline->descriptionSampleInfo[j]) {
							throw std::runtime_error("Error: descriptionSampleInfo is nullptr!");
						}
						object.pipeline->descriptionSampleInfo[j]->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
						object.pipeline->descriptionSampleInfo[j]->imageView = prefilterMapView;
						object.pipeline->descriptionSampleInfo[j]->sampler = prefilterTextureSampler;
						if (object.pipeline->descriptionSampleInfo[j]->imageView == VK_NULL_HANDLE) {
							throw std::runtime_error("Error: descriptionSampleInfo.imageView is VK_NULL_HANDLE!");
						}
					}
					else if (j == 7) {
						if (!object.pipeline->descriptionSampleInfo[j]) {
							throw std::runtime_error("Error: descriptionSampleInfo is nullptr!");
						}
						object.pipeline->descriptionSampleInfo[j]->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
						object.pipeline->descriptionSampleInfo[j]->imageView = BRDFImageView;
						object.pipeline->descriptionSampleInfo[j]->sampler = textureSampler;
						if (object.pipeline->descriptionSampleInfo[j]->imageView == VK_NULL_HANDLE) {
							throw std::runtime_error("Error: descriptionSampleInfo.imageView is VK_NULL_HANDLE!");
						}
					}
					else {
						object.pipeline->descriptionSampleInfo[j]->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
						object.pipeline->descriptionSampleInfo[j]->imageView = object.textures.imageViews[j];
						object.pipeline->descriptionSampleInfo[j]->sampler = textureSampler;
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
				for (size_t j = 0; j < object.pipeline->descriptionSampleInfo.size(); j++)
				{
					VkWriteDescriptorSet descriptorWriteSample{};
					descriptorWriteSample.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWriteSample.dstSet = object.descriptorSets[i];
					descriptorWriteSample.dstBinding = binding;
					descriptorWriteSample.dstArrayElement = 0;
					descriptorWriteSample.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					descriptorWriteSample.descriptorCount = 1;
					descriptorWriteSample.pImageInfo = object.pipeline->descriptionSampleInfo[j];
					descriptorWrites.push_back(descriptorWriteSample);
					binding++;
				}
				std::cout << "writes size: " << descriptorWrites.size() << std::endl;
				vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
			}
		}
	}

	void VulkanRendererAPI::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create buffer!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate buffer memory!");
		}

		vkBindBufferMemory(device, buffer, bufferMemory, 0);
	}

	void VulkanRendererAPI::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {

		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

		VkBufferCopy copyRegion{};
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		endSingleTimeCommands(commandBuffer);

	}

	uint32_t VulkanRendererAPI::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}
		throw std::runtime_error("failed to find suitable memory type!");
	}

	void VulkanRendererAPI::createCommandBuffers() {
		commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();;

		if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
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
				
				VkImageView prefilterMapTempView = createCubemapImageView(prefilterMapImage, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT, 1, mip);
				
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
			prefilterMapView = createCubemapImageView(prefilterMapImage, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT, prefilterMipLevels, 0);
			b_cubeconvulation = true;
		}
		//render pass info for main object pass
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
		renderPassInfo.renderArea.offset = { 0, 0 };
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
			ubo.proj = glm::perspective(glm::radians(90.0f), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 100.0f);
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

	void VulkanRendererAPI::createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, uint32_t layerCount) {
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = mipLevels;
		imageInfo.arrayLayers = layerCount;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.samples = numSamples;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image!");
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device, image, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate image memory!");
		}

		vkBindImageMemory(device, image, imageMemory, 0);

	}

	void VulkanRendererAPI::createCubemapImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, uint32_t layerCount) {
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = mipLevels;
		imageInfo.arrayLayers = 6;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.samples = numSamples;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
		if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image!");
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device, image, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate image memory!");
		}

		vkBindImageMemory(device, image, imageMemory, 0);

	}

	void VulkanRendererAPI::createShadowImage() {
		VkImageCreateInfo imageCreateInfo = {};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format = VK_FORMAT_D32_SFLOAT;  // Or whatever format you're using
		imageCreateInfo.extent.width = shadowMapSize;
		imageCreateInfo.extent.height = shadowMapSize;
		imageCreateInfo.extent.depth = 1;
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 12;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT; // Add SAMPLED_BIT
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // Initial layout
		imageCreateInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
		//imageCreateInfo.flags = VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT;
		if (vkCreateImage(device, &imageCreateInfo, nullptr, &shadowImage) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image!");
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device, shadowImage, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		if (vkAllocateMemory(device, &allocInfo, nullptr, &shadowImageMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate image memory!");
		}
		vkBindImageMemory(device, shadowImage, shadowImageMemory, 0);

		shadowImageView = createShadowImageView(shadowImage, VK_FORMAT_D32_SFLOAT, VK_IMAGE_ASPECT_DEPTH_BIT);
	}

	void VulkanRendererAPI::createTextureSampler() {

		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(physicalDevice, &properties);

		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.minLod = 0.0f; // Optional
		samplerInfo.maxLod = static_cast<float>(mipLevels);
		samplerInfo.mipLodBias = 0.0f; // Optional

		if (vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture sampler!");
		}
		//test for prefilter mipmapping sampler
		VkSamplerCreateInfo prefilterSamplerInfo{};
		prefilterSamplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		prefilterSamplerInfo.magFilter = VK_FILTER_LINEAR;
		prefilterSamplerInfo.minFilter = VK_FILTER_LINEAR;
		prefilterSamplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		prefilterSamplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		prefilterSamplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		prefilterSamplerInfo.anisotropyEnable = VK_TRUE;
		prefilterSamplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
		prefilterSamplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		prefilterSamplerInfo.unnormalizedCoordinates = VK_FALSE;
		prefilterSamplerInfo.compareEnable = VK_FALSE;
		prefilterSamplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		prefilterSamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		prefilterSamplerInfo.minLod = 0.0f; // Optional
		prefilterSamplerInfo.maxLod = prefilterMipLevels;
		prefilterSamplerInfo.mipLodBias = 0.0f; // Optional

		if (vkCreateSampler(device, &prefilterSamplerInfo, nullptr, &prefilterTextureSampler) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture sampler!");
		}

		VkSamplerCreateInfo shadowSamplerInfo{};
		shadowSamplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		shadowSamplerInfo.magFilter = VK_FILTER_LINEAR;
		shadowSamplerInfo.minFilter = VK_FILTER_LINEAR;
		shadowSamplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		shadowSamplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		shadowSamplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		shadowSamplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		shadowSamplerInfo.compareEnable = VK_TRUE; // Enable PCF
		shadowSamplerInfo.compareOp = VK_COMPARE_OP_LESS_OR_EQUAL; // Standard depth test for shadow mapping
		shadowSamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

		if (vkCreateSampler(device, &shadowSamplerInfo, nullptr, &shadowSampler) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create shadow sampler!");
		}

	}

	VkImageView VulkanRendererAPI::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels) {
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = mipLevels;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkImageView imageView;
		if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture image view!");
		}

		return imageView;
	}

	VkImageView VulkanRendererAPI::createShadowImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) {
		VkImageViewCreateInfo shadowViewInfo = {};
		shadowViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		shadowViewInfo.image = image;
		shadowViewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;  // <-- Change this
		shadowViewInfo.format = format;
		shadowViewInfo.subresourceRange.aspectMask = aspectFlags;
		shadowViewInfo.subresourceRange.baseMipLevel = 0;
		shadowViewInfo.subresourceRange.levelCount = 1;
		shadowViewInfo.subresourceRange.baseArrayLayer = 0;
		shadowViewInfo.subresourceRange.layerCount = 12;  // <-- Must be 6 for cube maps
		VkImageView imageView;
		if (vkCreateImageView(device, &shadowViewInfo, nullptr, &imageView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image view!");
		}

		return imageView;
	}

	VkImageView VulkanRendererAPI::createCubemapImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels, uint32_t baseMipLevel) {
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = baseMipLevel;
		viewInfo.subresourceRange.levelCount = mipLevels;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 6;

		VkImageView imageView;
		if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture image view!");
		}

		return imageView;
	}

	void VulkanRendererAPI::generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels) {
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

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


		endSingleTimeCommands(commandBuffer);
	}

	VkCommandBuffer VulkanRendererAPI::beginSingleTimeCommands() {
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}

	void VulkanRendererAPI::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(graphicsQueue);

		vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
	}

	void VulkanRendererAPI::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlags aspectFlags, int layerCount, uint32_t mipLevels, uint32_t baseMipLevel) {
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

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

		endSingleTimeCommands(commandBuffer);

	}

	void VulkanRendererAPI::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t baseArrayLayer) {
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

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

		endSingleTimeCommands(commandBuffer);

	}

	std::vector<const char*> VulkanRendererAPI::getRequiredExtensions() {
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (enableValidationLayers) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}
	//we need to populate the debug messenger create info
	void VulkanRendererAPI::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
	}

	bool VulkanRendererAPI::checkValidationLayerSupport() {
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : validationLayers) {
			bool layerFound = false;
			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}
			if (!layerFound) {
				return false;
			}
		}
		return true;
	}
	//we need to create a callback function for the debug messenger
	VKAPI_ATTR VkBool32 VKAPI_CALL VulkanRendererAPI::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	}
	VkResult VulkanRendererAPI::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}
	void VulkanRendererAPI::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(instance, debugMessenger, pAllocator);
		}
	}
	//we need to check if the device is suitable for our operations
	bool VulkanRendererAPI::isDeviceSuitable(VkPhysicalDevice device) {
		QueueFamilyIndices indices = findQueueFamilies(device);

		bool extensionsSupported = checkDeviceExtensionSupport(device);
		bool swapChainAdequate = false;
		if (extensionsSupported) {
			SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}
		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

		return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
	}
	//we need to check if the device supports the extensions we need
	bool VulkanRendererAPI::checkDeviceExtensionSupport(VkPhysicalDevice device) {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}
	//we need to find the queue families that support the operations we need
	QueueFamilyIndices VulkanRendererAPI::findQueueFamilies(VkPhysicalDevice device) {
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = i;
			}
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
			if (presentSupport) {
				indices.presentFamily = i;
			}
			if (indices.isComplete()) {
				break;
			}
			i++;
		}

		return indices;
	}
	//we query for the swap chain support details by querying the surface capabilities(#of images and width,height of them in SC), formats(pixel format, color space) and presentation modes
	SwapChainSupportDetails VulkanRendererAPI::querySwapChainSupport(VkPhysicalDevice device) {
		SwapChainSupportDetails details;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
		}
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
		}
		return details;
	}
	//we need to choose the surface format(color depth) and presentation mode(conditions for"swapping" images to the screen)
	VkSurfaceFormatKHR VulkanRendererAPI::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}

		return availableFormats[0];
	}
	//4 modes: VK_PRESENT_MODE_IMMEDIATE_KHR (may cause tearing), VK_PRESENT_MODE_FIFO_KHR(like v-sync), VK_PRESENT_MODE_FIFO_RELAXED_KHR(may cause visible tearing), VK_PRESENT_MODE_MAILBOX_KHR
	VkPresentModeKHR VulkanRendererAPI::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
		}
		return VK_PRESENT_MODE_FIFO_KHR;
	}
	//we need to choose the swap extent(the resolution of the swap chain images)
	VkExtent2D VulkanRendererAPI::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
		if (capabilities.currentExtent.width != UINT32_MAX) {
			return capabilities.currentExtent;
		}
		else {
			int width, height;
			glfwGetFramebufferSize(glfwWindow, &width, &height);
			//glfwGetFramebufferSize(glfwWindow, &width, &height);
			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};
			actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));
			return actualExtent;
		}
	}
	//we need to read the shader files
	VkShaderModule VulkanRendererAPI::createShaderModule(const std::vector<char>& code) {
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("failed to create shader module!");
		}

		return shaderModule;
	}

	VkFormat VulkanRendererAPI::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
		for (VkFormat format : candidates) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
				return format;
			}
		}
		throw std::runtime_error("failed to find supported format!");
	}

	VkFormat VulkanRendererAPI::findDepthFormat() {
		return findSupportedFormat(
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}

	bool VulkanRendererAPI::hasStencilComponent(VkFormat format) {
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}

	VkSampleCountFlagBits VulkanRendererAPI::getMaxUsableSampleCount() {
		VkPhysicalDeviceProperties physicalDeviceProperties;
		vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

		VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
		if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
		if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
		if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
		if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
		if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
		if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

		return VK_SAMPLE_COUNT_1_BIT;
	}

	void VulkanRendererAPI::createColorResources() {
		VkFormat colorFormat = swapChainImageFormat;

		createImage(swapChainExtent.width, swapChainExtent.height, 1, msaaSamples, colorFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, colorImage, colorImageMemory, 1);
		colorImageView = createImageView(colorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);

	}

	VkVertexInputBindingDescription VulkanRendererAPI::getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	std::array<VkVertexInputAttributeDescription, 4> VulkanRendererAPI::getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, Position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, Normal);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, TexCoords);

		attributeDescriptions[3].binding = 0;
		attributeDescriptions[3].location = 3;
		attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[3].offset = offsetof(Vertex, Tangent);
		return attributeDescriptions;
	} 

}