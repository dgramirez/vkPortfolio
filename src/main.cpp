int main() {
	printf("Hello, World!\n");
	return 0;
}

//Additional Implementations provided by single header libraries
#define STB_IMAGE_IMPLEMENTATION
#include "../dep/stbimage/stb_image.h"
#define VMA_IMPLEMENTATION
#include "../dep/vmaAllocator/vk_mem_alloc.h"