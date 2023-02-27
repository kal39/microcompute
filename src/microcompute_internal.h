#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan.h>

#include "microcompute.h"

typedef struct mcState {
	VkInstance instance;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	uint32_t queueIndex;
} mcState;

typedef struct mcBuffer {
	VkDevice device;
	uint64_t size;
	VkBuffer buffer;
	VkDeviceMemory memory;
} mcBuffer;

typedef struct mcProgram {

} mcProgram;

#define OK                                                                     \
	((mcResult){                                                               \
		.ok = MC_TRUE,                                                         \
		.file = __FILE__,                                                      \
		.line = __LINE__,                                                      \
		.func = __FUNCTION__,                                                  \
		.message = (char*){"no errors here :)"}})

#define ERROR(msg)                                                             \
	((mcResult){                                                               \
		.ok = MC_FALSE,                                                        \
		.file = __FILE__,                                                      \
		.line = __LINE__,                                                      \
		.func = __FUNCTION__,                                                  \
		.message = (char*){msg}})
