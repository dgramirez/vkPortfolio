//NO MINMAX WINDOWS >=O
#define NOMINMAX

//Console Color
#ifdef _DEBUG
#include "concolors.h"
#endif 

//C++ Headers Only!
#ifdef __cplusplus
//Unordered Maps
#include <unordered_map>

//(My) SSE Math Library
#include "ssemath.h"

//ImGui
#include "imgui.h"

//Vulkan Memory Allocator
#include "vk_mem_alloc.h"

//STB Image
#define STBI_NO_JPEG
#define STBI_NO_BMP
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_GIF
#define STBI_NO_PIC
#define STBI_NO_PNM
#include "stb_image.h"

//Gateware Include
#define GATEWARE_ENABLE_CORE
#define GATEWARE_ENABLE_SYSTEM
#define GATEWARE_ENABLE_INPUT
#include "Gateware.h"

//Gateware + ImGui
#include "ImGui/imgui_impl_gateware.h"
#endif
