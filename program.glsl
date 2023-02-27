#version 450

layout (local_size_x = 1) in;

layout(set = 0, binding = 1) buffer buff2 {
    int arr[];
} data1;

layout(set = 0, binding = 2) buffer buff1 {
    int arr[];
} data2;


void main() {
	uint idx = gl_GlobalInvocationID.x;
	data2.arr[idx] = data1.arr[idx] * 3;
}
