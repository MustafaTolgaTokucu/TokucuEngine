#include "tkcpch.h"
#include "VulkanSwapChain.h"
#include "VulkanFramebuffer.h"
#include "VulkanRenderPass.h"

namespace Tokucu {

	void VulkanSwapChain::createSwapChain() {
		SwapChainSupportDetails swapChainSupport = m_VulkanCore->querySwapChainSupport(physicalDevice);

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
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

		QueueFamilyIndices indices = m_VulkanCore->findQueueFamilies(physicalDevice);
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

	void VulkanSwapChain::cleanupSwapChain() {
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

	void VulkanSwapChain::recreateSwapChain() {
		int width = 0, height = 0;
		glfwWindow = m_VulkanCore->getGLFWWindow();
		glfwGetFramebufferSize(glfwWindow, &width, &height);
		while (width == 0 || height == 0) {
			glfwGetFramebufferSize(glfwWindow, &width, &height);
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(device);
		cleanupSwapChain();
		createSwapChain();
		createImageViews();
		createFramebuffers();
	}

	void VulkanSwapChain::createImageViews() {
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
			swapChainImageViews[i] = m_VulkanCreateImage->createImageView(swapChainImages[i], swapChainImageFormat, VK_IMAGE_VIEW_TYPE_2D, swapChainAspectFlags, mipLevels, 0, 1);
		}
		createColorResources();
		createDepthResources();

	}

	void VulkanSwapChain::createFramebuffers() {
		swapChainFramebuffers.resize(swapChainImageViews.size());
		for (size_t i = 0; i < swapChainImageViews.size(); i++) {
			std::vector<VkImageView> attachments = { colorImageView,depthImageView,swapChainImageViews[i] };
			swapChainFramebuffers[i] = m_VulkanFramebuffer->createFramebuffers(renderPass, attachments, swapChainExtent.width, swapChainExtent.height, 1);
		}
	}

	void VulkanSwapChain::createRenderPass() {
		VulkanRenderPass renderPass(device, physicalDevice);
		this->renderPass = renderPass.createRenderPass(msaaSamples, swapChainImageFormat);

	}
	//we need to choose the surface format(color depth) and presentation mode(conditions for"swapping" images to the screen)
	VkSurfaceFormatKHR VulkanSwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}

		return availableFormats[0];
	}
	//4 modes: VK_PRESENT_MODE_IMMEDIATE_KHR (may cause tearing), VK_PRESENT_MODE_FIFO_KHR(like v-sync), VK_PRESENT_MODE_FIFO_RELAXED_KHR(may cause visible tearing), VK_PRESENT_MODE_MAILBOX_KHR
	VkPresentModeKHR VulkanSwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
		}
		return VK_PRESENT_MODE_FIFO_KHR;
	}
	//we need to choose the swap extent(the resolution of the swap chain images)
	VkExtent2D VulkanSwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
		if (capabilities.currentExtent.width != UINT32_MAX) {
			return capabilities.currentExtent;
		}
		else {
			int width, height;
			glfwWindow = m_VulkanCore->getGLFWWindow();
			glfwGetFramebufferSize(glfwWindow, &width, &height);
			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};
			actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));
			return actualExtent;
		}
	}

	void VulkanSwapChain::createColorResources() {
		VkFormat colorFormat = swapChainImageFormat;
		m_VulkanCreateImage->createImage(swapChainExtent.width, swapChainExtent.height, mipLevels, msaaSamples, colorFormat, VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, colorImage, colorImageMemory, 1, 0);
		colorImageView = m_VulkanCreateImage->createImageView(colorImage, colorFormat, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels,0,1);

	}

	void VulkanSwapChain::createDepthResources() {
		VkFormat depthFormat = findDepthFormat();
		m_VulkanCreateImage->createImage(swapChainExtent.width, swapChainExtent.height, mipLevels, msaaSamples, depthFormat, VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory, 1, 0);
		depthImageView = m_VulkanCreateImage->createImageView(depthImage, depthFormat, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_DEPTH_BIT, mipLevels,0,1);
	}

	VkFormat VulkanSwapChain::findDepthFormat() {
		return findSupportedFormat(
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}
	
	VkFormat VulkanSwapChain::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
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
} 