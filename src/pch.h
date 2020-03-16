//(My) SSE Math Library
#include "../dep/sse_math/ssemath.h"

//ImGui
#include "../dep/imgui/imgui.h"

//Vulkan Memory Allocator
#include "../dep/vmaAllocator/vk_mem_alloc.h"

//STB Image
#define STBI_NO_JPEG
#define STBI_NO_BMP
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_GIF
#define STBI_NO_PIC
#define STBI_NO_PNM
#include "../dep/stbimage/stb_image.h"

//Gateware Include
#define GATEWARE_ENABLE_CORE
#define GATEWARE_ENABLE_SYSTEM
#define GATEWARE_ENABLE_INPUT
#define GATEWARE_ENABLE_AUDIO
#define NOMINMAX
#include "../dep/Gateware/Gateware.h"
