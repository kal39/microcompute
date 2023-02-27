#include "microcompute_internal.h"

mcResult mc_buffer_create(mcBufferH* bufferRef, mcStateH state, uint64_t size) {
	mcBuffer buffer;
	buffer.device = state->device;

	VkBufferCreateInfo buffCreateInfo = (VkBufferCreateInfo){
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = size,
		.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 1,
		.pQueueFamilyIndices = &state->queueIndex,
	};

	if (vkCreateBuffer(state->device, &buffCreateInfo, NULL, &buffer.buffer)
		!= VK_SUCCESS)
		return ERROR("failed to crate buffer");

	VkMemoryRequirements memoryReqs;
	vkGetBufferMemoryRequirements(state->device, buffer.buffer, &memoryReqs);

	VkPhysicalDeviceMemoryProperties memoryProps;
	vkGetPhysicalDeviceMemoryProperties(state->physicalDevice, &memoryProps);

	uint32_t memTypeIndex = UINT32_MAX;
	for (uint32_t i = 0; i < memoryProps.memoryTypeCount; i++) {
		VkMemoryType memType = memoryProps.memoryTypes[i];
		if (memType.propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			&& memType.propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
			memTypeIndex = i;
	}

	if (memTypeIndex == UINT32_MAX)
		return ERROR("failed to find valid memory type");

	VkMemoryAllocateInfo memAllocInfo = (VkMemoryAllocateInfo){
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = memoryReqs.size,
		.memoryTypeIndex = memTypeIndex,
	};

	if (vkAllocateMemory(state->device, &memAllocInfo, NULL, &buffer.memory)
		!= VK_SUCCESS)
		return ERROR("failed to allocate memory");

	if (vkBindBufferMemory(state->device, buffer.buffer, buffer.memory, 0)
		!= VK_SUCCESS)
		return ERROR("failed to bind buffer to memory");

	*bufferRef = malloc(sizeof(mcBuffer));
	(**bufferRef) = buffer;
	return OK;
}

mcResult mc_buffer_destroy(mcBufferH* buffer) {
	if (buffer == NULL) return ERROR("`buffer` is NULL");

	vkDestroyBuffer((*buffer)->device, (*buffer)->buffer, NULL);
	vkFreeMemory((*buffer)->device, (*buffer)->memory, NULL);

	free(*buffer);
	*buffer = NULL;
	return OK;
}

uint64_t mc_buffer_get_size(mcBufferH buffer) {
	return buffer->size;
}

mcResult mc_buffer_write(
	mcBufferH buffer,
	uint64_t offset,
	uint64_t size,
	void* data
) {
	if (offset + size > buffer->size)
		return ERROR("`offset` + `size` is larger than buffer size");

	void* map;
	if (vkMapMemory(buffer->device, buffer->memory, offset, size, 0, &map)
		!= VK_SUCCESS)
		return ERROR("failed to map memory");

	memcpy(map, data, size);
	vkUnmapMemory(buffer->device, buffer->memory);

	return OK;
}

mcResult mc_buffer_read(
	mcBufferH buffer,
	uint64_t offset,
	uint64_t size,
	void* data
) {
	if (offset + size > buffer->size)
		return ERROR("`offset` + `size` is larger than buffer size");

	void* map;
	if (vkMapMemory(buffer->device, buffer->memory, offset, size, 0, &map)
		!= VK_SUCCESS)
		return ERROR("failed to map memory");

	memcpy(data, map, size);
	vkUnmapMemory(buffer->device, buffer->memory);

	return OK;
}