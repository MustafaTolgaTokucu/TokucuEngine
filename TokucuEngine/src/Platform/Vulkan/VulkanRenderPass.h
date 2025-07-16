#pragma once

#include "VulkanCore.h"

#include <vector>

namespace Tokucu
{
	class VulkanRenderPass
	{

	public:
		VulkanRenderPass(VkDevice device, VkPhysicalDevice physicalDevice )
			: device(device),physicalDevice(physicalDevice) {
		}

		VkRenderPass createRenderPass(VkSampleCountFlagBits msaaSamples, VkFormat imageFormat);
		// Create a render pass suitable for offscreen rendering where the resolve attachment
		// (used as the final single-sampled image that ImGui samples from) is transitioned to
		// VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL at the end of the sub-pass so it can be
		// bound directly in shader descriptor sets without an extra barrier.
		VkRenderPass createOffscreenRenderPass(VkSampleCountFlagBits msaaSamples, VkFormat imageFormat);
		VkRenderPass createShadowRenderPass();
		VkRenderPass createHDRRenderPass();
		VkRenderPass createBRDFRenderPass();

		RenderPassInfo createRenderPassInfo(VkRenderPass renderPass ,VkFramebuffer framebuffer,const VkClearValue& Clear, uint32_t size) {
			
			RenderPassInfo result{};
			result.clearValue = Clear; // Store the clear value directly
			
			result.renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			result.renderPassInfo.pNext = nullptr;
			result.renderPassInfo.renderPass = renderPass;
			result.renderPassInfo.framebuffer = framebuffer;
			result.renderPassInfo.renderArea.offset = { 0, 0 };
			result.renderPassInfo.renderArea.extent.width = size;
			result.renderPassInfo.renderArea.extent.height = size;
			result.renderPassInfo.clearValueCount = 1;
			result.renderPassInfo.pClearValues = &result.clearValue; // Point to the stored clear value

			result.viewport.x = 0.0f;
			result.viewport.y = 0.0f;
			result.viewport.width = static_cast<float>(size);
			result.viewport.height = static_cast<float>(size);
			result.viewport.minDepth = 0.0f;
			result.viewport.maxDepth = 1.0f;
			
			result.scissor.offset = { 0, 0 };
			result.scissor.extent.height = size;
			result.scissor.extent.width = size;
			
			return result;
		}
		RenderPassInfo createSwapChainRenderPassInfo(VkRenderPass renderPass, VkFramebuffer framebuffer, std::array<VkClearValue, 2>& clear, VkExtent2D extent) {
			RenderPassInfo result{};
			result.clearValues = clear; // Store the clear values directly
			result.useMultipleClearValues = true; // Use multiple clear values
			
			result.renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			result.renderPassInfo.pNext = nullptr;
			result.renderPassInfo.renderPass = renderPass;
			result.renderPassInfo.framebuffer = framebuffer;
			result.renderPassInfo.renderArea.offset = { 0, 0 };
			result.renderPassInfo.renderArea.extent.width = extent.width;
			result.renderPassInfo.renderArea.extent.height = extent.height;
			result.renderPassInfo.clearValueCount = clear.size();
			result.renderPassInfo.pClearValues = result.clearValues.data(); // Point to the stored clear values
			
			result.viewport.x = 0.0f;
			result.viewport.y = 0.0f;
			result.viewport.width = static_cast<float>(extent.width);
			result.viewport.height = static_cast<float>(extent.height);
			result.viewport.minDepth = 0.0f;
			result.viewport.maxDepth = 1.0f;
			
			result.scissor.offset = { 0, 0 };
			result.scissor.extent = extent;
			result.scissor.extent.width = extent.width;
			result.scissor.extent.height = extent.height;
			
			return result;
		}

		VkFormat findDepthFormat();
		VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

	private:
		VkDevice device;
		//VkSampleCountFlagBits msaaSamples;
		//VkFormat imageFormat;
		//VkRenderPass renderPass = VK_NULL_HANDLE;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	};
}


