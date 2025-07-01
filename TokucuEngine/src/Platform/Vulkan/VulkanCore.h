#pragma once
#define GLFW_INCLUDE_VULKAN
#define GLM_ENABLE_EXPERIMENTAL
#include <GLFW/glfw3.h>

#include "vulkan/vulkan.h"
#include <optional>
#include <vector>
#include <iostream>

#include "Tokucu/Renderer/ObjectAttributes.h"

namespace Tokucu
{
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily = std::nullopt;
		std::optional<uint32_t> presentFamily = std::nullopt;
		bool isComplete() {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities = {};
		std::vector<VkSurfaceFormatKHR> formats = {};
		std::vector<VkPresentModeKHR> presentModes = {};
	};

	static std::vector<char> readFile(const std::string& filename) {
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("failed to open file!");
		}
		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();
		return buffer;
	}

	struct Pipeline {
		std::string name = "default";
		char vertexShader[64]{};
		char fragmentShader[64]{};
		char geometryShader[64]{};
		std::vector<std::pair<VkDescriptorType, VkShaderStageFlagBits>> descriptorLayout = {}; // Store descriptor types and their corresponding shader stages
		std::vector<std::pair<VkDescriptorBufferInfo*, VkBufferUsageFlagBits>> descriptionBufferInfo = {}; // Store buffer info and usage flags
		int descriptionSampleInfoCount = 0; // Count of descriptor sets
		std::vector<VkDeviceSize> bufferSize = {}; // Store buffer sizes
		VkRenderPass renderPass = VK_NULL_HANDLE; // Render pass associated with the pipeline
		VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT; // Default cull mode
		VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT; // Default MSAA samples
		bool b_colorAttachment = true; // Flag to indicate if color attachment is used

		VkPipeline pipeline = VK_NULL_HANDLE;
		VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
		VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
		VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
		std::vector<VkDescriptorImageInfo> descriptionSampleInfo = {}; // Store image info
	};

	struct VulkanObject {
		std::string name = "default";
		std::vector<Vertex> vertexData = {};
		std::vector<uint32_t> indexData = {};
		VkBuffer vertexBuffer = VK_NULL_HANDLE;
		VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
		VkBuffer indexBuffer = VK_NULL_HANDLE;
		VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;
		//VkVertexInputBindingDescription bindingDescriptions{};
		//std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};
		Pipeline* pipeline = nullptr; // Pointer to the pipeline associated with this object
		bool b_PBR = false; // Flag to indicate if the object uses PBR
		std::optional<std::string> modelLocation = std::nullopt; // Optional model location
		std::vector<std::pair<std::string, std::string>> textureLocations = { {"ambient", "assets/textures/default.jpg"},{"diffuse", "assets/textures/default.jpg"},{"specular", "assets/textures/default.jpg"},
			{"normal", "assets/textures/defaultNormal.jpg"} }; // Store multiple texture paths

		std::vector<std::vector<VkBuffer>> uniformBuffers = {};
		std::vector<std::vector<VkDeviceMemory>> uniformBuffersMemory = {};
		std::vector<std::vector<void*>> uniformBuffersMapped = {};

		std::vector<VkDescriptorSet> descriptorSets = {};
		struct Textures {
			VkImage ambient = VK_NULL_HANDLE;
			VkDeviceMemory ambientMemory = VK_NULL_HANDLE;
			VkImage diffuse = VK_NULL_HANDLE;
			VkDeviceMemory diffuseMemory = VK_NULL_HANDLE;
			VkImage specular = VK_NULL_HANDLE;
			VkDeviceMemory specularMemory = VK_NULL_HANDLE;
			VkImage normal = VK_NULL_HANDLE;
			VkDeviceMemory normalMemory = VK_NULL_HANDLE;

			std::array<VkImageView, 4> imageViews;//ambient,diffuse,specular respectively
		} textures;
	};

	struct BufferData {
		VkBuffer buffer = VK_NULL_HANDLE;
		VkDeviceMemory memory = VK_NULL_HANDLE;
	};

	class VulkanCore{

	public:
		
		VulkanCore(GLFWwindow* window) : glfwWindow(window)
		{
			uint32_t extensionCount = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
			std::cout << extensionCount << " extensions supported\n";
			createInstance();
			setupDebugMessenger();
			createSurface(glfwWindow);
			pickPhysicalDevice();
			createLogicalDevice();
		}

		~VulkanCore() {
			if (device != VK_NULL_HANDLE) {
				vkDestroyDevice(device, nullptr);
			}
			if (debugMessenger != VK_NULL_HANDLE) {
				DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
			}
			if (surface != VK_NULL_HANDLE) {
				vkDestroySurfaceKHR(instance, surface, nullptr);
			}
			if (instance != VK_NULL_HANDLE) {
				vkDestroyInstance(instance, nullptr);
			}
		}
		
		void createInstance();
		void pickPhysicalDevice();
		void createLogicalDevice();
		void createSurface(GLFWwindow* glfwWindow);
		//void createSyncObjects();
		void setupDebugMessenger();
		bool checkValidationLayerSupport();
		std::vector<const char*> getRequiredExtensions();
		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		bool isDeviceSuitable(VkPhysicalDevice device);
		bool checkDeviceExtensionSupport(VkPhysicalDevice device);
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
		VkSampleCountFlagBits getMaxUsableSampleCount();
		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator,
			VkDebugUtilsMessengerEXT* pDebugMessenger);
		void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

		//Getters
		VkDevice getDevice() const { return device; }
		VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; }
		VkQueue getGraphicsQueue() const { return graphicsQueue; }
		VkQueue getPresentQueue() const { return presentQueue; }
		VkSampleCountFlagBits getMsaaSamples() const { return msaaSamples; }
		VkSurfaceKHR getSurface() const { return surface; }
		VkInstance getInstance() const { return instance; }
		GLFWwindow* getGLFWWindow() const { return glfwWindow; }
		int getMaxFramesInFlight() const { return MAX_FRAMES_IN_FLIGHT; }

		//Setters
		void setGLFWWindow(GLFWwindow* window) { glfwWindow = window; }

		VkDebugUtilsMessengerEXT getDebugMessenger() const { return debugMessenger; }
		const std::vector<const char*>& getValidationLayers() const { return validationLayers; }
		const std::vector<const char*>& getDeviceExtensions() const { return deviceExtensions; }

	private:
		VkInstance instance = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
		const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
		const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
		GLFWwindow* glfwWindow = nullptr;
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice device = VK_NULL_HANDLE;
		VkQueue graphicsQueue = VK_NULL_HANDLE;
		VkQueue presentQueue = VK_NULL_HANDLE;
		VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
		QueueFamilyIndices queueFamilyIndices;
		SwapChainSupportDetails swapChainSupportDetails;

		const int MAX_FRAMES_IN_FLIGHT = 2;
	};
}


