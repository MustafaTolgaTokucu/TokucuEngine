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
		}

		~VulkanBuffer() {
			// Ensure device is idle before cleanup
			if (device != VK_NULL_HANDLE) {
				vkDeviceWaitIdle(device);
			}
			
			// Free command buffers first
			if (!commandBuffers.empty() && commandPool != VK_NULL_HANDLE && device != VK_NULL_HANDLE) {
				vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
				commandBuffers.clear();
			}
			
			// Then destroy command pool
			if (commandPool != VK_NULL_HANDLE && device != VK_NULL_HANDLE) {
				vkDestroyCommandPool(device, commandPool, nullptr);
				commandPool = VK_NULL_HANDLE;
			}
			TKC_CORE_INFO("VulkanBuffer destroyed successfully");
		}
		void createCommandPool();
		void createUniformBuffers(VulkanObject& object);
		void createCommandBuffers();
		BufferData createVertexBuffer(const std::vector<Vertex>& vertexData);
		BufferData createIndexBuffer(const std::vector<uint32_t>& indexData);
		void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

		void generateMipmaps(VkImage image, VkFormat format, int32_t width, int32_t height, uint32_t mipLevels);
		void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlags aspectFlags, int layerCount, uint32_t mipLevels, uint32_t baseMipLevel);
		void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t baseArrayLayer);

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


