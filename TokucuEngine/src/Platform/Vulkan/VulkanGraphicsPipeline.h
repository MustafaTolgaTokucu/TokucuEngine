#pragma once
#include "VulkanCore.h"

namespace Tokucu
{
	class VulkanGraphicsPipeline
	{

	public:
		VulkanGraphicsPipeline(VulkanCore* vulkanCore, VkDevice device, VkPhysicalDevice physicalDevice) : m_VulkanCore(vulkanCore), device(device), physicalDevice(physicalDevice) {

			MAX_FRAMES_IN_FLIGHT = m_VulkanCore->getMaxFramesInFlight();
		}
		~VulkanGraphicsPipeline() {

			if (commandPool != VK_NULL_HANDLE) {
				vkDestroyCommandPool(device, commandPool, nullptr);
			}
			TKC_CORE_INFO("VulkanGraphicsPipeline destroyed successfully");

		}
		void createGraphicsPipeline(Pipeline* pipeline);
		void createDescriptorSetLayout(Pipeline* pipeline);
		void createDescriptorPool(Pipeline* pipeline, uint32_t objectSize);
		void createDescriptorSets(VulkanObject* object);
		void renderObject(VkCommandBuffer& commandBuffer, Pipeline* pipeline, RenderPassInfo& renderPassInfo, VulkanObject& object, uint32_t currentFrame);
		VkVertexInputBindingDescription getBindingDescription();
		std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions();
		VkShaderModule createShaderModule(const std::vector<char>& code);

	private:

		VulkanCore* m_VulkanCore;

		VkDevice device;
		VkPhysicalDevice physicalDevice;
		VkCommandPool commandPool = VK_NULL_HANDLE;

		uint16_t MAX_FRAMES_IN_FLIGHT = 2;
	};
}