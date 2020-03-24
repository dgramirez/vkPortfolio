#include "App.h"

int main() {
	//Initialize the Application (Window, Vulkan, ImGui)
	App::Init();

	//Main Loop
	App::Run();

	//Clean up the application
	App::Cleanup();

	//Program ran successful
	return 0;
}

//Additional Implementations provided by single header libraries
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"
