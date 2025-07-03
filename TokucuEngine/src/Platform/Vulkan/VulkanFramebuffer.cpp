#include "tkcpch.h"
#include "VulkanFramebuffer.h"

#include "VulkanSwapChain.h"

namespace Tokucu {


	/*
	VkFramebuffer VulkanFramebuffer::createFramebuffers(VkRenderPass renderPass, std::vector<VkImageView> attachments, int width, int height, int layer) {
		// Do not create another frambuffer here, affects recreating swapchain
		swapChainFramebuffers.resize(swapChainImageViews.size());
		//for (size_t i = 0; i < swapChainImageViews.size(); i++) {
			//std::array<VkImageView, 3> attachments = { colorImageView,depthImageView,swapChainImageViews[i] };
			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			//framebufferInfo.width = swapChainExtent.width;
			//framebufferInfo.height = swapChainExtent.height;

			framebufferInfo.width = width;
			framebufferInfo.height = height;

			framebufferInfo.layers = layer;

			if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create framebuffer!");
			}
		//}
	}

	*/

}