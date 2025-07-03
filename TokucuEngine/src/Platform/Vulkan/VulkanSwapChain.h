#pragma once

#include "VulkanCore.h"
#include "VulkanCreateImage.h"

namespace Tokucu {

	// Forward declarations
	class VulkanFramebuffer;

	class VulkanSwapChain
	{

	public:
		VulkanSwapChain(VulkanCore* vulkanCore,VulkanFramebuffer* vulkanFramebuffer, VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface)
			: m_VulkanCore(vulkanCore),m_VulkanFramebuffer(vulkanFramebuffer), physicalDevice(physicalDevice), device(device), surface(surface) {
			msaaSamples = m_VulkanCore->getMsaaSamples();
			m_VulkanCreateImage = std::make_unique<VulkanCreateImage>(device, physicalDevice);
			createSwapChain();
			createImageViews();
			createRenderPass();
			createFramebuffers();
		}
		~VulkanSwapChain() {
			for (auto framebuffer : swapChainFramebuffers) {
				vkDestroyFramebuffer(device, framebuffer, nullptr);
			}
			for (auto imageView : swapChainImageViews) {
				vkDestroyImageView(device, imageView, nullptr);
			}

			vkDestroyRenderPass(device, renderPass, nullptr);
			if (depthImageView != VK_NULL_HANDLE) {
				vkDestroyImageView(device, depthImageView, nullptr);
			}
			if (colorImageView != VK_NULL_HANDLE) {
				vkDestroyImageView(device, colorImageView, nullptr);
			}
			if (depthImage != VK_NULL_HANDLE) {
				vkDestroyImage(device, depthImage, nullptr);
			}
			if (colorImage != VK_NULL_HANDLE) {
				vkDestroyImage(device, colorImage, nullptr);
			}
			if (depthImageMemory != VK_NULL_HANDLE) {
				vkFreeMemory(device, depthImageMemory, nullptr);
			}
			if (colorImageMemory != VK_NULL_HANDLE) {
				vkFreeMemory(device, colorImageMemory, nullptr);
			}
			if (swapChain != VK_NULL_HANDLE) {
				vkDestroySwapchainKHR(device, swapChain, nullptr);
			}
			// Clear all vectors
			swapChainImages.clear();
			swapChainImageViews.clear();
			swapChainFramebuffers.clear();
			swapChain = VK_NULL_HANDLE;

			cleanupSwapChain();
			TKC_CORE_INFO("VulkanSwapChain destroyed successfully");
		}
		void createSwapChain();

		void recreateSwapChain();
		void createImageViews();
		void createFramebuffers();
		void createRenderPass();
		void cleanupSwapChain();

	    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	    VkFormat findDepthFormat();

		void createColorResources();
		void createDepthResources();


		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		//GETTERS
		VkRenderPass getRenderPass() const { return renderPass; }
		VkSwapchainKHR getSwapChain() const { return swapChain; }
		const std::vector<VkImage>& getSwapChainImages() const { return swapChainImages; }
		VkFormat getSwapChainImageFormat() const { return swapChainImageFormat; }
		VkExtent2D getSwapChainExtent() const { return swapChainExtent; }
		const std::vector<VkImageView>& getSwapChainImageViews() const { return swapChainImageViews; }
		const std::vector<VkFramebuffer>& getSwapChainFramebuffers() const { return swapChainFramebuffers; }

	private:

		VulkanCore* m_VulkanCore;
		VulkanFramebuffer* m_VulkanFramebuffer;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice device = VK_NULL_HANDLE;
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		VkSwapchainKHR swapChain = VK_NULL_HANDLE;
		std::vector<VkImage> swapChainImages = {};
		VkFormat swapChainImageFormat = VK_FORMAT_UNDEFINED;
		VkExtent2D swapChainExtent = {};
		std::vector<VkImageView> swapChainImageViews = {};
		std::vector<VkFramebuffer> swapChainFramebuffers = {};

		VkImageView depthImageView = VK_NULL_HANDLE;
		VkImageView colorImageView = VK_NULL_HANDLE;

		VkImage depthImage = VK_NULL_HANDLE;
		VkImage colorImage = VK_NULL_HANDLE;

		VkDeviceMemory depthImageMemory = VK_NULL_HANDLE;
		VkDeviceMemory colorImageMemory = VK_NULL_HANDLE;

		VkRenderPass renderPass = VK_NULL_HANDLE;

		uint32_t mipLevels = 1.0;
		VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;

		GLFWwindow* glfwWindow = nullptr;
		std::unique_ptr<VulkanCreateImage> m_VulkanCreateImage = nullptr;
	};
}



