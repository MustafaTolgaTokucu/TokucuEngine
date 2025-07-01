#pragma once
#include "VulkanCore.h"

namespace Tokucu
{
	class VulkanBuffer
	{
	public:
		VulkanBuffer(VulkanCore* vulkanCore, VkPhysicalDevice physicalDevice, VkDevice device)
			: m_VulkanCore(vulkanCore), physicalDevice(physicalDevice), device(device) {
			MAX_FRAMES_IN_FLIGHT = m_VulkanCore->getMaxFramesInFlight();
			//graphicsQueue = m_VulkanCore->getGraphicsQueue();
		}
		~VulkanBuffer() {
			if (commandPool != VK_NULL_HANDLE) {
				vkDestroyCommandPool(device, commandPool, nullptr);
			}
		}
		void createCommandPool();
		void createUniformBuffers(VulkanObject& object);
		void createCommandBuffers();
		BufferData createVertexBuffer(const std::vector<Vertex>& vertexData);
		BufferData createIndexBuffer(const std::vector<uint32_t>& indexData);
		void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		VkCommandBuffer beginSingleTimeCommands();
		void endSingleTimeCommands(VkCommandBuffer commandBuffer);
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		//Getters
		VkCommandPool getCommandPool() const { return commandPool; }
		std::vector<VkCommandBuffer> getCommandBuffers() const { return commandBuffers; }

	private:
		VulkanCore* m_VulkanCore;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice device = VK_NULL_HANDLE;
		VkCommandPool commandPool = VK_NULL_HANDLE;
		int MAX_FRAMES_IN_FLIGHT = 2;
		std::vector<VkCommandBuffer> commandBuffers;
		VkQueue graphicsQueue = VK_NULL_HANDLE;

	};
}


