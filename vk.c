#include <stdio.h>

// gcc vk.c -o vk -lvulkan -g
// glslangValidator -V program.glsl -S comp -o program.spv

#include "src/microcompute.h"

uint32_t device_selector(uint32_t count, mcDevice* devices) {
	printf("devices:\n");
	for (uint32_t i = 0; i < count; i++) {
		printf(
			"- %s (%s): %d bytes memory\n",
			devices[i].name,
			devices[i].type,
			devices[i].memorySize
		);
	}

	return 1;
}

int main(void) {
	mcResult res;

	mcStateH state;
	res = mc_state_create(&state, device_selector);
	mc_result_print(res);
	if (!res.ok) return 1;

	mcBufferH buffer;
	res = mc_buffer_create(&buffer, state, sizeof(float) * 100);
	mc_result_print(res);
	if (!res.ok) return 1;

	mcProgramH program;
	res = mc_program_create_from_file(
		&program,
		state,
		1,
		(mcBufferH[]){buffer},
		"program.spv",
		"main"
	);
	mc_result_print(res);
	if (!res.ok) return 1;

	mc_buffer_destroy(&buffer);
	mc_state_destroy(&state);

	return 0;
}