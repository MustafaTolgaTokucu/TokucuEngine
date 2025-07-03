#include "tkcpch.h"
#include "VulkanBuffer.h"
#include <functional>
#include <array>



namespace Tokucu
{

	void VulkanBuffer::createCommandPool() {
		QueueFamilyIndices queueFamilyIndices = m_VulkanCore->findQueueFamilies(physicalDevice);
		//QueueFamilyIndices queueFamilyIndices = queueFamilyIndices;
	
		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
	
		if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create command pool!");
		}
	}

	void VulkanBuffer::createUniformBuffers(VulkanObject& object) {

		
		size_t numUBOs = object.pipeline->bufferSize.size();
		object.uniformBuffers.resize(numUBOs);
		object.uniformBuffersMemory.resize(numUBOs);
		object.uniformBuffersMapped.resize(numUBOs);

		for (size_t uboIndex = 0; uboIndex < numUBOs; uboIndex++) {
			VkDeviceSize bufferSize = object.pipeline->bufferSize[uboIndex];

			object.uniformBuffers[uboIndex].resize(MAX_FRAMES_IN_FLIGHT);
			object.uniformBuffersMemory[uboIndex].resize(MAX_FRAMES_IN_FLIGHT);
			object.uniformBuffersMapped[uboIndex].resize(MAX_FRAMES_IN_FLIGHT);

			for (size_t frame = 0; frame < MAX_FRAMES_IN_FLIGHT; frame++) {
				createBuffer(bufferSize, object.pipeline->descriptionBufferInfo[uboIndex].second,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					object.uniformBuffers[uboIndex][frame], object.uniformBuffersMemory[uboIndex][frame]);

				vkMapMemory(device, object.uniformBuffersMemory[uboIndex][frame], 0, bufferSize, 0,
					&object.uniformBuffersMapped[uboIndex][frame]);
			}
		}
		
	}

	void VulkanBuffer::createCommandBuffers() {
		commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();;

		if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}

	BufferData VulkanBuffer::createVertexBuffer(const std::vector<Vertex>& vertexData) {
		VkDeviceSize bufferSize = sizeof(vertexData[0]) * vertexData.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertexData.data(), (size_t)bufferSize);
		vkUnmapMemory(device, stagingBufferMemory);

		VkBuffer vertexBuffer = VK_NULL_HANDLE;
		VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

		copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);
		return { vertexBuffer, vertexBufferMemory };
	}

	BufferData VulkanBuffer::createIndexBuffer(const std::vector<uint32_t>& intexData) {
		VkDeviceSize bufferSize = sizeof(intexData[0]) * intexData.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, intexData.data(), (size_t)bufferSize);
		vkUnmapMemory(device, stagingBufferMemory);
		VkBuffer indexBuffer = VK_NULL_HANDLE;
		VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;
		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

		copyBuffer(stagingBuffer, indexBuffer, bufferSize);

		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);
		return { indexBuffer, indexBufferMemory };
	}

	void VulkanBuffer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create buffer!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate buffer memory!");
		}

		vkBindBufferMemory(device, buffer, bufferMemory, 0);
	}

	void VulkanBuffer::generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels) {
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, imageFormat, &formatProperties);
		if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
			throw std::runtime_error("texture image format does not support linear blitting!");
		}
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.image = image;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.levelCount = 1;

		int32_t mipWidth = texWidth;
		int32_t mipHeight = texHeight;

		for (uint32_t i = 1; i < mipLevels; i++) {
			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			vkCmdPipelineBarrier(commandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			VkImageBlit blit{};
			blit.srcOffsets[0] = { 0, 0, 0 };
			blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
			blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.mipLevel = i - 1;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = 1;
			blit.dstOffsets[0] = { 0, 0, 0 };
			blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
			blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.mipLevel = i;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount = 1;

			vkCmdBlitImage(commandBuffer,
				image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1, &blit,
				VK_FILTER_LINEAR);

			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(commandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);
			if (mipWidth > 1) mipWidth /= 2;
			if (mipHeight > 1) mipHeight /= 2;
		}

		barrier.subresourceRange.baseMipLevel = mipLevels - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);


		endSingleTimeCommands(commandBuffer);
	}

	void VulkanBuffer::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlags aspectFlags, int layerCount, uint32_t mipLevels, uint32_t baseMipLevel) {
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = aspectFlags;
		barrier.subresourceRange.baseMipLevel = baseMipLevel;
		barrier.subresourceRange.levelCount = mipLevels;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = layerCount;

		VkPipelineStageFlags sourceStage = 0;
		VkPipelineStageFlags destinationStage = 0;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		}
		else {
			throw std::invalid_argument("unsupported layout transition!");
		}

		vkCmdPipelineBarrier(
			commandBuffer,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		endSingleTimeCommands(commandBuffer);

	}

	void VulkanBuffer::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t baseArrayLayer) {
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = baseArrayLayer;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = {
			width,
			height,
			1
		};

		vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		endSingleTimeCommands(commandBuffer);

	}

	void VulkanBuffer::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {

		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

		VkBufferCopy copyRegion{};
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		endSingleTimeCommands(commandBuffer);

	}

	VkCommandBuffer VulkanBuffer::beginSingleTimeCommands() {
		// Ensure command pool and device are valid
		if (commandPool == VK_NULL_HANDLE || device == VK_NULL_HANDLE) {
			throw std::runtime_error("Command pool or device is invalid for single-time commands");
		}
		
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}

	void VulkanBuffer::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		graphicsQueue = m_VulkanCore->getGraphicsQueue();
		vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(graphicsQueue);

		// Only free command buffer if pool and device are still valid
		if (commandPool != VK_NULL_HANDLE && device != VK_NULL_HANDLE) {
			vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
		}
	}

	uint32_t VulkanBuffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}
		throw std::runtime_error("failed to find suitable memory type!");
	}

	// Command recording functions
	//void VulkanBuffer::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, uint32_t currentImage,
	//	const std::vector<VulkanObject>& objects, const std::vector<LightAttributes>& lights,
	//	VkRenderPass mainRenderPass, VkRenderPass shadowRenderPass, VkRenderPass hdrRenderPass, VkRenderPass brdfRenderPass,
	//	const std::vector<VkFramebuffer>& swapChainFramebuffers, VkExtent2D swapChainExtent,
	//	VkFramebuffer shadowFramebuffer, VkFramebuffer hdrFramebuffer, VkFramebuffer irradianceFramebuffer, 
	//	VkFramebuffer brdfFramebuffer, const std::vector<VkFramebuffer>& prefilterFramebuffers,
	//	bool& b_cubeconvulation, uint32_t shadowMapSize, uint32_t prefilterMipLevels) {
	//	
	//	VkCommandBufferBeginInfo beginInfo{};
	//	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	//	beginInfo.flags = 0;
	//	beginInfo.pInheritanceInfo = nullptr;
	//
	//	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
	//		throw std::runtime_error("failed to begin recording command buffer!");
	//	}
	//
	//	// Record shadow pass
	//	VkClearValue clearDepth{};
	//	clearDepth.depthStencil = { 1.0f, 0 };
	//	recordRenderPass(commandBuffer, shadowRenderPass, shadowFramebuffer, shadowMapSize, shadowMapSize, { clearDepth },
	//		[this, &objects, &lights, currentImage](VkCommandBuffer cmdBuffer, uint32_t frame) {
	//			// Render shadow maps for each light
	//			for (int i = 0; i < 2; i++) {
	//				lightIndexUBO indexUBO{};
	//				indexUBO.lightIndex = i;
	//				
	//				renderObjects(cmdBuffer, frame, objects, {"defaultPipeline"}, 
	//					[&indexUBO](VkCommandBuffer cmd, const VulkanObject& obj, uint32_t currentFrame) {
	//						vkCmdPushConstants(cmd, obj.pipeline->pipelineLayout, 
	//							VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 
	//							0, sizeof(lightIndexUBO), &indexUBO);
	//					});
	//			}
	//		}, currentImage);
	//
	//	// Record HDR passes (only once)
	//	if (!b_cubeconvulation) {
	//		// HDR Skybox pass
	//		recordRenderPass(commandBuffer, hdrRenderPass, hdrFramebuffer, 1024, 1024, { clearDepth },
	//			[this, &objects, currentImage](VkCommandBuffer cmdBuffer, uint32_t frame) {
	//				renderObjects(cmdBuffer, frame, objects, {"skyboxPipelineHDR"});
	//			}, currentImage);
	//
	//		// Cube Convolution pass
	//		recordRenderPass(commandBuffer, hdrRenderPass, irradianceFramebuffer, 32, 32, { clearDepth },
	//			[this, &objects, currentImage](VkCommandBuffer cmdBuffer, uint32_t frame) {
	//				renderObjects(cmdBuffer, frame, objects, {"CubeConvPipeline"});
	//			}, currentImage);
	//
	//		// BRDF LUT pass
	//		recordRenderPass(commandBuffer, brdfRenderPass, brdfFramebuffer, 512, 512, { clearDepth },
	//			[this, &objects, currentImage](VkCommandBuffer cmdBuffer, uint32_t frame) {
	//				renderObjects(cmdBuffer, frame, objects, {"BRDFPipeline"});
	//			}, currentImage);
	//
	//		// Prefilter passes for each mip level
	//		for (size_t mip = 0; mip < prefilterMipLevels; mip++) {
	//			uint32_t mipWidth = std::max(1u, static_cast<uint32_t>(128 * std::pow(0.5f, mip)));
	//			uint32_t mipHeight = std::max(1u, static_cast<uint32_t>(128 * std::pow(0.5f, mip)));
	//			
	//			recordRenderPass(commandBuffer, hdrRenderPass, prefilterFramebuffers[mip], mipWidth, mipHeight, { clearDepth },
	//				[this, &objects, currentImage, mip, prefilterMipLevels](VkCommandBuffer cmdBuffer, uint32_t frame) {
	//					float roughness = static_cast<float>(mip) / static_cast<float>(prefilterMipLevels - 1);
	//					
	//					renderObjects(cmdBuffer, frame, objects, {"prefilterPipeline"}, 
	//						[roughness](VkCommandBuffer cmd, const VulkanObject& obj, uint32_t currentFrame) {
	//							vkCmdPushConstants(cmd, obj.pipeline->pipelineLayout, 
	//								VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 
	//								0, sizeof(float), &roughness);
	//						});
	//				}, currentImage);
	//		}
	//		
	//		b_cubeconvulation = true;
	//	}
	//
	//	// Record main render pass
	//	std::array<VkClearValue, 2> clearValues{};
	//	clearValues[0].color = { {0.0f, 0.4f, 0.6f, 1.0f} };
	//	clearValues[1].depthStencil = { 1.0f, 0 };
	//
	//	recordRenderPass(commandBuffer, mainRenderPass, swapChainFramebuffers[imageIndex], 
	//		swapChainExtent.width, swapChainExtent.height, { clearValues[0], clearValues[1] },
	//		[this, &objects, currentImage](VkCommandBuffer cmdBuffer, uint32_t frame) {
	//			renderObjects(cmdBuffer, frame, objects, {"defaultPipeline", "lightPipeline", "skyboxPipeline"});
	//		}, currentImage);
	//
	//	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
	//		throw std::runtime_error("failed to record command buffer!");
	//	}
	//}
	//
	//void VulkanBuffer::recordRenderPass(VkCommandBuffer commandBuffer, VkRenderPass renderPass, VkFramebuffer framebuffer,
	//	uint32_t width, uint32_t height, const std::vector<VkClearValue>& clearValues,
	//	const std::function<void(VkCommandBuffer, uint32_t)>& renderFunction, uint32_t currentImage) {
	//	
	//	VkRenderPassBeginInfo renderPassBeginInfo{};
	//	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	//	renderPassBeginInfo.renderPass = renderPass;
	//	renderPassBeginInfo.framebuffer = framebuffer;
	//	renderPassBeginInfo.renderArea.offset = { 0, 0 };
	//	renderPassBeginInfo.renderArea.extent = { width, height };
	//	renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	//	renderPassBeginInfo.pClearValues = clearValues.data();
	//
	//	vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	//	
	//	setupViewportAndScissor(commandBuffer, width, height);
	//	
	//	renderFunction(commandBuffer, currentImage); // Pass the actual current frame index
	//	
	//	vkCmdEndRenderPass(commandBuffer);
	//}
	//
	//void VulkanBuffer::renderObjects(VkCommandBuffer commandBuffer, uint32_t currentImage, 
	//	const std::vector<VulkanObject>& objects,
	//	const std::vector<std::string>& pipelineNames, 
	//	const std::function<void(VkCommandBuffer, const VulkanObject&, uint32_t)>& customRender) {
	//	
	//	VkDeviceSize offsets[] = { 0 };
	//
	//	for (const auto& object : objects) {
	//		// Check if this object's pipeline should be rendered
	//		bool shouldRender = false;
	//		for (const auto& pipelineName : pipelineNames) {
	//			if (object.pipeline->name == pipelineName) {
	//				shouldRender = true;
	//				break;
	//			}
	//		}
	//
	//		if (!shouldRender) continue;
	//
	//		if (object.pipeline->pipeline == VK_NULL_HANDLE) {
	//			throw std::runtime_error("Error: pipeline is NULL before binding!");
	//		}
	//
	//		// Apply custom rendering logic if provided
	//		if (customRender) {
	//			customRender(commandBuffer, object, currentImage);
	//		}
	//
	//		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, object.pipeline->pipeline);
	//		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, 
	//			object.pipeline->pipelineLayout, 0, 1, &object.descriptorSets[currentImage], 0, nullptr);
	//		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &object.vertexBuffer, offsets);
	//		vkCmdBindIndexBuffer(commandBuffer, object.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
	//		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(object.indexData.size()), 1, 0, 0, 0);
	//	}
	//}
	//
	//void VulkanBuffer::setupViewportAndScissor(VkCommandBuffer commandBuffer, uint32_t width, uint32_t height) {
	//	VkViewport viewport{};
	//	viewport.x = 0.0f;
	//	viewport.y = 0.0f;
	//	viewport.width = static_cast<float>(width);
	//	viewport.height = static_cast<float>(height);
	//	viewport.minDepth = 0.0f;
	//	viewport.maxDepth = 1.0f;
	//
	//	VkRect2D scissor{};
	//	scissor.offset = { 0, 0 };
	//	scissor.extent = { width, height };
	//
	//	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	//	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	//}
}