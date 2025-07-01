#pragma once
#include "VulkanCore.h"

namespace Tokucu
{
	class VulkanGraphicsPipeline
	{

	public:
		VulkanGraphicsPipeline(VulkanCore* vulkanCore,VkDevice device, VkPhysicalDevice physicalDevice) : m_VulkanCore(vulkanCore), device(device), physicalDevice(physicalDevice) {}
		~VulkanGraphicsPipeline() {
			if (commandPool != VK_NULL_HANDLE) {
				vkDestroyCommandPool(device, commandPool, nullptr);
			}
		}
		void createGraphicsPipeline(Pipeline* pipeline);
		void createDescriptorSetLayout(Pipeline* pipeline);
		VkVertexInputBindingDescription getBindingDescription();
		std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions();
		VkShaderModule createShaderModule(const std::vector<char>& code);

	private:

		VulkanCore* m_VulkanCore;

		VkDevice device;
		VkPhysicalDevice physicalDevice;
		VkCommandPool commandPool = VK_NULL_HANDLE;
	};
}
