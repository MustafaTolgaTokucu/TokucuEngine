#pragma once
#define GLFW_INCLUDE_VULKAN
#define GLM_ENABLE_EXPERIMENTAL
#include <GLFW/glfw3.h>

#include "vulkan/vulkan.h"
#include <optional>
#include <vector>
#include <iostream>

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


	};
}


