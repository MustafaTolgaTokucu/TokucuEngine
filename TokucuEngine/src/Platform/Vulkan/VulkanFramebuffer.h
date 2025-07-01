#pragma once

#include "VulkanCore.h"
namespace Tokucu {



	class VulkanFramebuffer
	{



	public:
		VulkanFramebuffer(VkDevice device, VkRenderPass renderPass, std::vector<VkImageView> attachments, int width, int height, int layer)
			:  device(device), renderPass(renderPass), attachments(attachments), width(width), height(height), layer(layer) {
			//createFramebuffers();
		}
		~VulkanFramebuffer() {
			cleanup();
		}
		VkFramebuffer createFramebuffers() {
			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = width;
			framebufferInfo.height = height;
			framebufferInfo.layers = layer;
			VkFramebuffer framebuffer;
			if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffer) != VK_SUCCESS) {
				throw std::runtime_error("failed to create framebuffer!");
			}
			return framebuffer;
		}

		void cleanup() {
			//for (auto framebuffer : framebuffers) {
			//	vkDestroyFramebuffer(device, framebuffer, nullptr);
			//}
			//framebuffers.clear();
		}

	private:

		
		VkDevice device;
		VkRenderPass renderPass;
		std::vector<VkImageView> attachments;
		int width, height, layer;




	};

}

