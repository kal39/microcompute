#include "microcompute_internal.h"

void mc_result_print(mcResult result) {
	if (result.ok) {
		printf(
			"\e[1m\e[35m%s\e[39m, \e[4m%s:%d\e[24m\e[39m, \e[32mok\e[39m: "
			"\e[0m%s\n",
			result.func,
			result.file,
			result.line,
			result.message
		);
	} else {
		printf(
			"\e[1m\e[35m%s\e[39m, \e[4m%s:%d\e[24m\e[39m, \e[31mERROR\e[39m: "
			"\e[0m%s\n",
			result.func,
			result.file,
			result.line,
			result.message
		);
	}
}

mcResult mc_read_file(uint32_t* size, void* data, const char* path) {
	FILE* fp = fopen(path, "rb");
	if (fp == NULL) return ERROR("failed to open file");

	fseek(fp, 0, SEEK_END);
	long length = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	if (data != NULL) fread(data, 1, length, fp);
	fclose(fp);

	if (size != NULL) *size = length;

	return OK;
}
