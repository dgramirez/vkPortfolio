// dear imgui: Platform Binding for Gateware (64 bit applications only)
// This needs to be used along with a Renderer (e.g. DirectX11, OpenGL3, Vulkan..)

// Implemented features:
// [X] NOTHING! :D

#pragma once

IMGUI_IMPL_API bool	ImGui_ImplGateware_Init(void* gwindow);
IMGUI_IMPL_API void	ImGui_ImplGateware_Shutdown();
IMGUI_IMPL_API void	ImGui_ImplGateware_NewFrame(const float& dt);