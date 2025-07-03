#include "tkcpch.h"
#include "VulkanCreateImage.h"

namespace Tokucu {


	VkImage VulkanCreateImage::createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
		VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, uint32_t layerCount, int flags) {
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
		imageInfo.flags = flags; // No special flags for a simple image = 0

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
		return image;
	}

	VkImageView VulkanCreateImage::createImageView(VkImage image, VkFormat format,VkImageViewType viewType, VkImageAspectFlags aspectFlags, uint32_t mipLevels, uint32_t baseMipLevel, uint32_t layerCount) {
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = viewType;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = baseMipLevel;
		viewInfo.subresourceRange.levelCount = mipLevels;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = layerCount;
		VkImageView imageView;
		if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture image view!");
		}
		return imageView;
	}

	VkSampler VulkanCreateImage::createSampler(
		VkFilter               magFilter,
		VkFilter               minFilter,
		VkSamplerAddressMode   addrMode,
		bool                   enableAniso,
		float                  maxAniso,
		VkBorderColor          borderColor,
		bool                   compareEnable,
		VkCompareOp            compareOp,
		uint32_t               mipLevels)
	{
		VkPhysicalDeviceProperties props{};
		vkGetPhysicalDeviceProperties(physicalDevice, &props);

		VkSamplerCreateInfo info{ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
		info.magFilter = magFilter;
		info.minFilter = minFilter;
		info.addressModeU = addrMode;
		info.addressModeV = addrMode;
		info.addressModeW = addrMode;
		info.anisotropyEnable = enableAniso ? VK_TRUE : VK_FALSE;
		info.maxAnisotropy = enableAniso
			? std::min(maxAniso, props.limits.maxSamplerAnisotropy)
			: 1.0f;
		info.borderColor = borderColor;
		info.unnormalizedCoordinates = VK_FALSE;
		info.compareEnable = compareEnable ? VK_TRUE : VK_FALSE;
		info.compareOp = compareOp;
		info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		info.minLod = 0.0f;
		info.maxLod = static_cast<float>(mipLevels);
		info.mipLodBias = 0.0f;

		VkSampler sampler;
		if (vkCreateSampler(device, &info, nullptr, &sampler) != VK_SUCCESS) {
			throw std::runtime_error("failed to create sampler!");
		}
		return sampler;
	}

	uint32_t VulkanCreateImage::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}
		throw std::runtime_error("failed to find suitable memory type!");
	}




}