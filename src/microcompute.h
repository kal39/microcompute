#pragma once

#include <stdint.h>

typedef enum mcBool {
	MC_FALSE,
	MC_TRUE,
} mcBool;

typedef struct mcResult {
	mcBool ok;
	const char* file;
	uint32_t line;
	const char* func;
	const char* message;
} mcResult;

typedef struct mcDevice {
	char* name;
	char* type;
	uint32_t memorySize;
} mcDevice;

typedef struct mcState* mcStateH;
typedef struct mcBuffer* mcBufferH;
typedef struct mcProgram* mcProgramH;

typedef uint32_t(mcDeviceSelector)(uint32_t deviceCount, mcDevice* devices);

void mc_result_print(mcResult result);
mcResult mc_read_file(uint32_t* size, void* data, const char* path);

mcResult mc_state_create(mcStateH* stateRef, mcDeviceSelector device_selector);
mcResult mc_state_destroy(mcStateH* stateRef);

mcResult mc_buffer_create(mcBufferH* bufferRef, mcStateH state, uint64_t size);
mcResult mc_buffer_destroy(mcBufferH* bufferRef);
uint64_t mc_buffer_get_size(mcBufferH buffer);

mcResult mc_buffer_write(
	mcBufferH buffer,
	uint64_t offset,
	uint64_t size,
	void* data
);

mcResult mc_buffer_read(
	mcBufferH buffer,
	uint64_t offset,
	uint64_t size,
	void* data
);

mcResult mc_program_create_from_string(
	mcProgramH* programRef,
	mcStateH state,
	uint32_t bufferCount,
	mcBufferH* buffers,
	uint32_t codeSize,
	void* code,
	char* entryPoint
);

mcResult mc_program_create_from_file(
	mcProgramH* programRef,
	mcStateH state,
	uint32_t bufferCount,
	mcBufferH* buffers,
	char* path,
	char* entryPoint
);