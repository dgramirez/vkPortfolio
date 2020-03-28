#ifdef _WIN32
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#endif
#include "App.h"

int main() {
	//Memory Leak Detection (Win32)
	#ifdef _WIN32
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		_CrtSetBreakAlloc(-1);
	#endif

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
