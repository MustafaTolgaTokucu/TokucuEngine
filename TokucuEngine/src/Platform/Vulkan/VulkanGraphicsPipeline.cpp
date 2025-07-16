#include "tkcpch.h"
#include "VulkanGraphicspipeline.h"


namespace Tokucu
{

	void VulkanGraphicsPipeline::createGraphicsPipeline(Pipeline* pipeline) {
		//for (auto& pipeline : Pipelines) {
			//adds value to descriptionSampleInfo 
		pipeline->descriptionSampleInfo.resize(pipeline->descriptionSampleInfoCount);

		auto vertShaderCode = readFile(pipeline->vertexShader);
		auto fragShaderCode = readFile(pipeline->fragmentShader);

		VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
		VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

		VkShaderModule geoShaderModule = VK_NULL_HANDLE;
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
		if (strlen(pipeline->geometryShader) > 0) {
			auto geoShaderCode = readFile(pipeline->geometryShader);
			geoShaderModule = createShaderModule(geoShaderCode);

			VkPipelineShaderStageCreateInfo geoShaderStageInfo{};
			geoShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			geoShaderStageInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
			geoShaderStageInfo.module = geoShaderModule;
			geoShaderStageInfo.pName = "main";

			shaderStages.push_back(geoShaderStageInfo);
		}
		shaderStages.push_back(vertShaderStageInfo);
		shaderStages.push_back(fragShaderStageInfo);

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		auto bindingDescription = getBindingDescription();
		auto attributeDescriptions = getAttributeDescriptions();

		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = pipeline->cullMode;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;

		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = pipeline->msaaSamples;

		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;

		if (pipeline->b_colorAttachment == false)
		{
			colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			colorBlending.attachmentCount = 0;  // No color attachments
			colorBlending.pAttachments = nullptr;
		}

		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.stencilTestEnable = VK_FALSE;

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		// Use a larger size to accommodate different push constant types (lightIndexUBO, float, etc.)
		pushConstantRange.size = sizeof(float) * 4; // Large enough for most push constant types

		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &pipeline->descriptorSetLayout;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipeline->pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = shaderStages.size();
		pipelineInfo.pStages = shaderStages.data();
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.layout = pipeline->pipelineLayout;
		pipelineInfo.renderPass = pipeline->renderPass;
		// If the pipeline does not have a color attachment, set pColorBlendState to nullptr
		if (pipeline->b_colorAttachment == false)
		{
			pipelineInfo.pColorBlendState = nullptr;
		}
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;

		if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline->pipeline) != VK_SUCCESS) {
			throw std::runtime_error("failed to create graphics pipeline!");
		}

		vkDestroyShaderModule(device, fragShaderModule, nullptr);
		vkDestroyShaderModule(device, vertShaderModule, nullptr);
		if (geoShaderModule != VK_NULL_HANDLE)
		{
			vkDestroyShaderModule(device, geoShaderModule, nullptr);
		}
		//}
	}

	void VulkanGraphicsPipeline::createDescriptorSetLayout(Pipeline* pipeline) {

		std::vector<VkDescriptorSetLayoutBinding> bindings;
		int iteration = 0;
		for (auto& [type, flag] : pipeline->descriptorLayout)
		{
			std::cout << "Descriptor Type: " << type << ", Shader Stage: " << flag << std::endl;
			VkDescriptorSetLayoutBinding LayoutBinding{};
			LayoutBinding.binding = iteration;
			LayoutBinding.descriptorCount = 1;
			LayoutBinding.descriptorType = type;
			LayoutBinding.pImmutableSamplers = nullptr;
			LayoutBinding.stageFlags = flag;
			bindings.push_back(LayoutBinding);
			iteration++;
		}
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &pipeline->descriptorSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}

	void VulkanGraphicsPipeline::createDescriptorPool(Pipeline* pipeline, uint32_t objectSize) {
		std::vector<VkDescriptorPoolSize> poolSizes;
		int iteration = 0;
		for (auto& [type, flag] : pipeline->descriptorLayout) {
			VkDescriptorPoolSize poolSize{};
			poolSize.type = type;
			poolSize.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * objectSize);
			poolSizes.push_back(poolSize);
			iteration++;
		}
		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * objectSize);

		if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &pipeline->descriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}

	void VulkanGraphicsPipeline::createDescriptorSets(VulkanObject* object) {
		std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, object->pipeline->descriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = object->pipeline->descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		allocInfo.pSetLayouts = layouts.data();

		object->descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);

		if (vkAllocateDescriptorSets(device, &allocInfo, object->descriptorSets.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			std::vector<VkWriteDescriptorSet> descriptorWrites;
			descriptorWrites.clear();

			for (size_t j = 0; j < object->pipeline->bufferSize.size(); j++)
			{
				object->pipeline->descriptionBufferInfo[j].first->buffer = object->uniformBuffers[j][i];
				object->pipeline->descriptionBufferInfo[j].first->offset = 0;
				object->pipeline->descriptionBufferInfo[j].first->range = object->pipeline->bufferSize[j];
			}
			// Use the minimum of available textures and expected samplers to avoid out-of-bounds access
			size_t textureCount = std::min(object->pipeline->descriptionSampleInfo.size(), object->texturesInfo.size());
			TKC_CORE_INFO("Object: {} - Expected samplers: {}, Available textures: {}, Using: {}", 
						 object->name, object->pipeline->descriptionSampleInfo.size(), 
						 object->texturesInfo.size(), textureCount);
			for (size_t j = 0; j < textureCount; j++)
			{
				auto& info = object->pipeline->descriptionSampleInfo[j];
				info.imageLayout = object->texturesInfo[j].imageLayout;
				info.imageView = object->texturesInfo[j].imageView;
				info.sampler = object->texturesInfo[j].sampler;
			}
			int binding = 0;
			for (size_t j = 0; j < object->pipeline->descriptionBufferInfo.size(); j++)
			{
				VkWriteDescriptorSet descriptorWriteBuffer{};
				descriptorWriteBuffer.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWriteBuffer.dstSet = object->descriptorSets[i];
				descriptorWriteBuffer.dstBinding = binding;
				descriptorWriteBuffer.dstArrayElement = 0;
				descriptorWriteBuffer.descriptorType = object->pipeline->descriptorLayout[j].first;
				descriptorWriteBuffer.descriptorCount = 1;
				descriptorWriteBuffer.pBufferInfo = object->pipeline->descriptionBufferInfo[j].first;
				descriptorWrites.push_back(descriptorWriteBuffer);
				binding++;
			}
			// Use the same safe count for descriptor writes to match the textures we actually have
			for (size_t j = 0; j < textureCount; j++)
			{
				auto& info = object->pipeline->descriptionSampleInfo[j];
				VkWriteDescriptorSet descriptorWriteSample{};
				descriptorWriteSample.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWriteSample.dstSet = object->descriptorSets[i];
				descriptorWriteSample.dstBinding = binding;
				descriptorWriteSample.dstArrayElement = 0;
				descriptorWriteSample.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorWriteSample.descriptorCount = 1;
				descriptorWriteSample.pImageInfo = &info;
				descriptorWrites.push_back(descriptorWriteSample);
				binding++;
			}
			std::cout << "writes size: " << descriptorWrites.size() << std::endl;
			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}
	}

	void VulkanGraphicsPipeline::renderObject(VkCommandBuffer& commandBuffer, Pipeline* pipeline, RenderPassInfo& renderPassInfo, VulkanObject& object, uint32_t currentFrame) {
		// Validate inputs
		if (!pipeline || !pipeline->pipeline) {
			throw std::runtime_error("Invalid pipeline in renderObject");
		}
		if (currentFrame >= MAX_FRAMES_IN_FLIGHT) {
			throw std::runtime_error("Invalid currentFrame in renderObject");
		}
		if (currentFrame >= object.descriptorSets.size() || object.descriptorSets[currentFrame] == VK_NULL_HANDLE) {
			throw std::runtime_error("Invalid descriptor set in renderObject");
		}

		VkDeviceSize offsets[] = { 0 };
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipeline);
		vkCmdSetViewport(commandBuffer, 0, 1, &renderPassInfo.viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &renderPassInfo.scissor);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipelineLayout, 0, 1, &object.descriptorSets[currentFrame], 0, nullptr);
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &object.vertexBuffer, offsets);
		vkCmdBindIndexBuffer(commandBuffer, object.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(object.indexData.size()), 1, 0, 0, 0);
	}
	//we need to read the shader files
	VkShaderModule VulkanGraphicsPipeline::createShaderModule(const std::vector<char>& code) {
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("failed to create shader module!");
		}

		return shaderModule;
	}

	VkVertexInputBindingDescription VulkanGraphicsPipeline::getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	std::array<VkVertexInputAttributeDescription, 4> VulkanGraphicsPipeline::getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, Position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, Normal);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, TexCoords);

		attributeDescriptions[3].binding = 0;
		attributeDescriptions[3].location = 3;
		attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[3].offset = offsetof(Vertex, Tangent);
		return attributeDescriptions;
	}
}