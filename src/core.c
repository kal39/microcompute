#include "microcompute_internal.h"

mcResult mc_state_create(mcStateH* stateRef, mcDeviceSelector device_selector) {
	mcState state;

	VkApplicationInfo appInfo = (VkApplicationInfo){
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "Vulkan test",
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.engineVersion = VK_MAKE_VERSION(1, 0, 0),
		.apiVersion = VK_API_VERSION_1_0,
	};

	VkInstanceCreateInfo iCreateInfo = (VkInstanceCreateInfo){
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &appInfo,
		.enabledLayerCount = 1,
		.ppEnabledLayerNames = (const char *[]){
			"VK_LAYER_KHRONOS_validation",
		},
	};

	if (vkCreateInstance(&iCreateInfo, NULL, &state.instance) != VK_SUCCESS)
		return ERROR("failed to create vulkan state");

	uint32_t deviceCount;
	vkEnumeratePhysicalDevices(state.instance, &deviceCount, NULL);
	VkPhysicalDevice physicalDevices[deviceCount];
	VkPhysicalDeviceProperties physicalDeviceProps[deviceCount];
	mcDevice devices[deviceCount];
	vkEnumeratePhysicalDevices(state.instance, &deviceCount, physicalDevices);

	if (deviceCount == 0) return ERROR("could not find any vulkan devices");

	for (uint32_t i = 0; i < deviceCount; i++) {
		vkGetPhysicalDeviceProperties(
			physicalDevices[i],
			&physicalDeviceProps[i]
		);
		devices[i] = (mcDevice) {
			.name = physicalDeviceProps[i].deviceName,
			.type = (char *[]){
				"other",
				"integrated gpu",
				"discrete gpu",
				"virtual gpu",
				"cpu",
			}[physicalDeviceProps[i].deviceType],
			.memorySize
			= physicalDeviceProps[i].limits.maxComputeSharedMemorySize,
		};
	}

	uint32_t deviceChoice = device_selector(deviceCount, devices);

	if (deviceChoice >= deviceCount)
		return ERROR("`deviceChoice` is larger than total device count");

	state.physicalDevice = physicalDevices[deviceChoice];

	uint32_t queueCount;
	vkGetPhysicalDeviceQueueFamilyProperties(
		state.physicalDevice,
		&queueCount,
		NULL
	);
	VkQueueFamilyProperties queues[queueCount];
	vkGetPhysicalDeviceQueueFamilyProperties(
		state.physicalDevice,
		&queueCount,
		queues
	);

	state.queueIndex = UINT32_MAX;
	for (uint32_t i = 0; i < queueCount; i++) {
		if (queues[i].queueFlags & VK_QUEUE_COMPUTE_BIT) state.queueIndex = i;
	}

	if (state.queueIndex == UINT32_MAX)
		return ERROR("failed to find compute queue that supports compute");

	VkDeviceQueueCreateInfo queueCreateInfo = (VkDeviceQueueCreateInfo){
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.queueFamilyIndex = state.queueIndex,
		.queueCount = 1,
		.pQueuePriorities = &(float){1.0},
	};

	VkDeviceCreateInfo devCreateInfo = (VkDeviceCreateInfo){
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &queueCreateInfo,
		.pEnabledFeatures = &(VkPhysicalDeviceFeatures){},
	};

	if (vkCreateDevice(
			state.physicalDevice,
			&devCreateInfo,
			NULL,
			&state.device
		)
		!= VK_SUCCESS) {
		return ERROR("failed to create vulkan device");
	}

	*stateRef = malloc(sizeof(mcState));
	(**stateRef) = state;
	return OK;
}

mcResult mc_state_destroy(mcStateH* state) {
	if (state == NULL) return ERROR("`state` is NULL");

	vkDestroyDevice((*state)->device, NULL);
	vkDestroyInstance((*state)->instance, NULL);

	free(*state);
	*state = NULL;
	return OK;
}