#pragma once

#include "VulkanCore.h"
namespace Tokucu {

	class VulkanFramebuffer
	{
	public:
		VulkanFramebuffer(VkDevice device )
			:  device(device) {
			//createFramebuffers();
		}
		~VulkanFramebuffer() {
			cleanup();
		}
		VkFramebuffer createFramebuffers(VkRenderPass renderPass, std::vector<VkImageView> attachments, uint32_t width, uint32_t height, uint32_t layerCount) {
			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = width;
			framebufferInfo.height = height;
			framebufferInfo.layers = layerCount;
			VkFramebuffer framebuffer;
			if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffer) != VK_SUCCESS) {
				throw std::runtime_error("failed to create framebuffer!");
			}
			return framebuffer;
		}

		// IBL-specific framebuffer methods already implemented in VulkanRendererAPI

		void cleanup() {
		}

	private:
		VkDevice device;
	};

}

