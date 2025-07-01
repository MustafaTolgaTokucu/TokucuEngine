#include "tkcpch.h"
#include "VulkanRenderPass.h"

namespace Tokucu {

	VkRenderPass VulkanRenderPass::createRenderPass() {
		VkRenderPass renderPass = VK_NULL_HANDLE;
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = imageFormat;
		colorAttachment.samples = msaaSamples;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription colorAttachmentResolve{};
		colorAttachmentResolve.format = imageFormat;
		colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentResolveRef{};
		colorAttachmentResolveRef.attachment = 2;
		colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription depthAttachment{};
		depthAttachment.format = findDepthFormat();
		depthAttachment.samples = msaaSamples;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;
		subpass.pResolveAttachments = &colorAttachmentResolveRef;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		std::array<VkAttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };
		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
			throw std::runtime_error("failed to create render pass!");
		}
		return renderPass;
	}

	VkRenderPass VulkanRenderPass::createShadowRenderPass() {

		VkRenderPass shadowRenderPass = VK_NULL_HANDLE;

		VkAttachmentDescription shadowDepthAttachment{};
		shadowDepthAttachment.format = findDepthFormat();
		shadowDepthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		shadowDepthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		shadowDepthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		shadowDepthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		shadowDepthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		shadowDepthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		shadowDepthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

		VkAttachmentReference shadowDepthAttachmentRef{};
		shadowDepthAttachmentRef.attachment = 0;
		shadowDepthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription shadowSubpass{};
		shadowSubpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		shadowSubpass.pDepthStencilAttachment = &shadowDepthAttachmentRef;
		shadowSubpass.colorAttachmentCount = 0;
		shadowSubpass.pColorAttachments = nullptr;

		VkSubpassDependency dependencies{};
		dependencies.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependencies.dstSubpass = 0;
		dependencies.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		dependencies.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependencies.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		dependencies.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		dependencies.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		VkRenderPassCreateInfo renderPassCreateInfo{};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.attachmentCount = 1;
		renderPassCreateInfo.pAttachments = &shadowDepthAttachment;
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &shadowSubpass;
		renderPassCreateInfo.dependencyCount = 1;
		renderPassCreateInfo.pDependencies = &dependencies;

		if (vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &shadowRenderPass)) {
			throw std::runtime_error("failed to create render pass!");
		}
		return shadowRenderPass;
	
	}

	VkRenderPass VulkanRenderPass::createHDRRenderPass() {

		//////////////////////////////////////////
		//for skybox hdr
		//////////////////////////////////////////
		VkRenderPass renderPassHDR = VK_NULL_HANDLE;
		VkAttachmentDescription colorAttachmentHDR{};
		colorAttachmentHDR.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		colorAttachmentHDR.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachmentHDR.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachmentHDR.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachmentHDR.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentHDR.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachmentHDR.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachmentHDR.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		// Create attachment reference
		VkAttachmentReference colorAttachmentRefHDR{};
		colorAttachmentRefHDR.attachment = 0;
		colorAttachmentRefHDR.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		// Create subpass
		VkSubpassDescription subpassHDR{};
		subpassHDR.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassHDR.colorAttachmentCount = 1;
		subpassHDR.pColorAttachments = &colorAttachmentRefHDR;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		//std::array<VkAttachmentDescription, 3> attachments2 = { colorAttachmentHDR, depthAttachment, colorAttachmentResolveHDR };
		VkRenderPassCreateInfo renderPassInfoHDR{};
		renderPassInfoHDR.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		//renderPassInfoHDR.attachmentCount = static_cast<uint32_t>(attachments2.size());
		renderPassInfoHDR.attachmentCount = 1;
		renderPassInfoHDR.pAttachments = &colorAttachmentHDR;
		renderPassInfoHDR.subpassCount = 1;
		renderPassInfoHDR.pSubpasses = &subpassHDR;
		renderPassInfoHDR.dependencyCount = 1;
		renderPassInfoHDR.pDependencies = &dependency;

		if (vkCreateRenderPass(device, &renderPassInfoHDR, nullptr, &renderPassHDR) != VK_SUCCESS) {
			throw std::runtime_error("failed to create render pass!");
		}

		return renderPassHDR;
	}

	VkRenderPass VulkanRenderPass::createBRDFRenderPass() {

		//////////////////////////////////////////
		//for BRDF LUT
		//////////////////////////////////////////

		VkRenderPass BRDFRenderPass = VK_NULL_HANDLE;

		VkAttachmentDescription colorAttachmentBRDF{};
		colorAttachmentBRDF.format = VK_FORMAT_R32G32_SFLOAT;
		colorAttachmentBRDF.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachmentBRDF.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachmentBRDF.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachmentBRDF.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentBRDF.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachmentBRDF.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachmentBRDF.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		// Create attachment reference
		VkAttachmentReference colorAttachmentRefBRDF{};
		colorAttachmentRefBRDF.attachment = 0;
		colorAttachmentRefBRDF.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		// Create subpass
		VkSubpassDescription subpassBRDF{};
		subpassBRDF.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassBRDF.colorAttachmentCount = 1;
		subpassBRDF.pColorAttachments = &colorAttachmentRefBRDF;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfoBRDF{};
		renderPassInfoBRDF.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfoBRDF.attachmentCount = 1;
		renderPassInfoBRDF.pAttachments = &colorAttachmentBRDF;
		renderPassInfoBRDF.subpassCount = 1;
		renderPassInfoBRDF.pSubpasses = &subpassBRDF;
		renderPassInfoBRDF.dependencyCount = 1;
		renderPassInfoBRDF.pDependencies = &dependency;

		if (vkCreateRenderPass(device, &renderPassInfoBRDF, nullptr, &BRDFRenderPass) != VK_SUCCESS) {
			throw std::runtime_error("failed to create render pass!");
		}

		return BRDFRenderPass;
	}

	VkFormat VulkanRenderPass::findDepthFormat() {
		return findSupportedFormat(
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}

	VkFormat VulkanRenderPass::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
		for (VkFormat format : candidates) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
				return format;
			}
		}
		throw std::runtime_error("failed to find supported format!");
	}



}