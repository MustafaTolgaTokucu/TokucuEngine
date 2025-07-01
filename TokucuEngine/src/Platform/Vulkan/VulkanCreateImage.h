#pragma once
#include "VulkanCore.h"

namespace Tokucu
{
	class VulkanCreateImage
	{

	public:
		VulkanCreateImage(VkDevice device, VkPhysicalDevice physicalDevice)
			: device(device), physicalDevice(physicalDevice) {
		}
		VkImage createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
			VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, uint32_t layerCount, int flags);
		VkImageView createImageView(VkImage image, VkFormat format, VkImageViewType viewType, VkImageAspectFlags aspectFlags, uint32_t mipLevels, uint32_t baseMipLevel, uint32_t layerCount);

		VkSampler createSampler(VkFilter               magFilter,
			VkFilter               minFilter,
			VkSamplerAddressMode   addrMode,
			bool                   enableAniso,
			float                  maxAniso,
			VkBorderColor          borderColor,
			bool                   compareEnable,
			VkCompareOp            compareOp,
			uint32_t               mipLevels);
		
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	private:

		VkDevice device;
		VkPhysicalDevice physicalDevice;
		







	};
}


