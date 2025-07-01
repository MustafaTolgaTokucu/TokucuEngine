#pragma once

#include "VulkanCore.h"


namespace Tokucu
{
	class VulkanRenderPass
	{

	public:


		VulkanRenderPass(VkDevice device, VkPhysicalDevice physicalDevice, VkSampleCountFlagBits msaaSamples, VkFormat swapChainImageFormat)
			: device(device),physicalDevice(physicalDevice), msaaSamples(msaaSamples), imageFormat(swapChainImageFormat) {
		}

		VkRenderPass createRenderPass();
		VkRenderPass createShadowRenderPass();
		VkRenderPass createHDRRenderPass();
		VkRenderPass createBRDFRenderPass();

		VkFormat findDepthFormat();
		VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

	private:
		VkDevice device;
		VkSampleCountFlagBits msaaSamples;
		VkFormat imageFormat;
		//VkRenderPass renderPass = VK_NULL_HANDLE;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;



		//VkFormat findDepthFormat() {
		//	return VulkanCore::findSupportedFormat(
		//		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		//		VK_IMAGE_TILING_OPTIMAL,
		//		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		//	);
		//}
		//bool hasStencilComponent(VkFormat format) {
		//	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
		//}
	
	
	
	};
}


