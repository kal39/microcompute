#include "microcompute_internal.h"

mcResult mc_program_create_from_string(
	mcProgramH* programRef,
	mcStateH state,
	uint32_t bufferCount,
	mcBufferH* buffers,
	uint32_t codeSize,
	void* code,
	char* entryPoint
) {
	mcProgram program;

	VkShaderModuleCreateInfo shaderCreateInfo = (VkShaderModuleCreateInfo){
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = codeSize,
		.pCode = (uint32_t*)code,
	};

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(
			state->device,
			&shaderCreateInfo,
			NULL,
			&shaderModule
		)
		!= VK_SUCCESS)
		return ERROR("failed to crate shader");

	VkDescriptorSetLayoutBinding layoutBindings[bufferCount];
	for (uint32_t i = 0; i < bufferCount; i++) {
		layoutBindings[i] = (VkDescriptorSetLayoutBinding){
			.binding = i,
			.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
		};
	}

	VkDescriptorSetLayoutCreateInfo layoutCreateInfo
		= (VkDescriptorSetLayoutCreateInfo){
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.bindingCount = bufferCount,
			.pBindings = layoutBindings,
		};

	VkDescriptorSetLayout descriptorLayout;
	if (vkCreateDescriptorSetLayout(
			state->device,
			&layoutCreateInfo,
			NULL,
			&descriptorLayout
		)
		!= VK_SUCCESS)
		return ERROR("failed to crate descriptor set layout");

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo
		= (VkPipelineLayoutCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.setLayoutCount = 1,
			.pSetLayouts = &descriptorLayout,
		};

	VkPipelineLayout pipelineLayout;
	if (vkCreatePipelineLayout(
			state->device,
			&pipelineLayoutCreateInfo,
			NULL,
			&pipelineLayout
		)
		!= VK_SUCCESS)
		return ERROR("failed to crate pipeline layout");

	VkPipelineCacheCreateInfo cacheCreateInfo = (VkPipelineCacheCreateInfo){
		.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
	};

	VkPipelineCache pipelineCache;
	if (vkCreatePipelineCache(
			state->device,
			&cacheCreateInfo,
			NULL,
			&pipelineCache
		)
		!= VK_SUCCESS)
		return ERROR("failed to crate pipeline cache");

	VkPipelineShaderStageCreateInfo shaderStageCreateInfo
		= (VkPipelineShaderStageCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = VK_SHADER_STAGE_COMPUTE_BIT,
			.module = shaderModule,
			.pName = entryPoint,
		};

	VkComputePipelineCreateInfo pipelineCreateInfo
		= (VkComputePipelineCreateInfo){
			.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
			.stage = shaderStageCreateInfo,
			.layout = pipelineLayout,
		};

	VkPipeline pipeline;
	if (vkCreateComputePipelines(
			state->device,
			pipelineCache,
			1,
			&pipelineCreateInfo,
			NULL,
			&pipeline
		)
		!= VK_SUCCESS)
		return ERROR("failed to create pipeline");

	VkDescriptorPoolSize poolSize = (VkDescriptorPoolSize){
		.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		.descriptorCount = bufferCount,
	};

	VkDescriptorPoolCreateInfo poolCreateInfo = (VkDescriptorPoolCreateInfo){
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.maxSets = bufferCount,
		.poolSizeCount = 1,
		.pPoolSizes = &poolSize,
	};

	VkDescriptorPool pool;
	if (vkCreateDescriptorPool(state->device, &poolCreateInfo, NULL, &pool)
		!= VK_SUCCESS)
		return ERROR("failed to crate descriptor pool");

	VkDescriptorSetAllocateInfo allocInfo = (VkDescriptorSetAllocateInfo){
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.descriptorPool = pool,
		.descriptorSetCount = 1,
		.pSetLayouts = &descriptorLayout,
	};

	VkDescriptorSet descriptorSet;
	if (vkAllocateDescriptorSets(state->device, &allocInfo, &descriptorSet)
		!= VK_SUCCESS)
		return ERROR("failed to allocate descriptor sets");

	VkDescriptorBufferInfo bufferInfos[bufferCount];
	VkWriteDescriptorSet writeDescriptorSets[bufferCount];

	for (uint32_t i = 0; i < bufferCount; i++) {
		bufferInfos[i] = (VkDescriptorBufferInfo){
			.buffer = buffers[i]->buffer,
			.offset = 0,
			.range = buffers[i]->size,
		};

		writeDescriptorSets[i] = (VkWriteDescriptorSet){
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = descriptorSet,
			.dstBinding = i,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			.pBufferInfo = &bufferInfos[i],
		};
	}

	vkUpdateDescriptorSets(state->device, 1, writeDescriptorSets, 0, NULL);

	*programRef = malloc(sizeof(mcProgram));
	(**programRef) = program;

	return OK;
}

mcResult mc_program_create_from_file(
	mcProgramH* programRef,
	mcStateH state,
	uint32_t bufferCount,
	mcBufferH* buffers,
	char* path,
	char* entryPoint
) {
	mcResult res;

	uint32_t fileSize;
	res = mc_read_file(&fileSize, NULL, path);
	if (!res.ok) return res;

	uint8_t fileData[fileSize];
	res = mc_read_file(&fileSize, fileData, path);
	if (!res.ok) return res;

	return mc_program_create_from_string(
		programRef,
		state,
		bufferCount,
		buffers,
		fileSize,
		fileData,
		entryPoint
	);
}