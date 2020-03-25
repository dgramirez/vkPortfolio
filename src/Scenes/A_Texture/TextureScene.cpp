#include "TextureScene.h"
#include "../../Vulkan/VkGlobals.h"
#include "../../ImGui/ImGuiGlobals.h"

TextureScene::TextureScene() {
	//1.) Update the Surface Information
	VkSwapchain::UpdateSurfaceData();

	//2.) Setup Surface Information for Swapchain
	VkSwapchain::surfaceCapabilities = VkGlobal::surfaceCapabilities;
	VkSwapchain::surfacePresentMode = VK_PRESENT_MODE_FIFO_KHR;
	VkSwapchain::surfaceExtent2D = VkSwapchain::surfaceCapabilities.currentExtent;
	VkSwapchain::surfaceExtent3D = { VkSwapchain::surfaceExtent2D.width, VkSwapchain::surfaceExtent2D.height, 1 };
	VkSwapchain::surfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
	VkSwapchain::surfaceFormat.colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;

	//3.) Setup Other Swapchain Proeprties	
	VkSwapchain::presetFlags = 0x1;
	VkSwapchain::frameMax = 2;
	VkSwapchain::depthFormat = VK_FORMAT_D32_SFLOAT;
	VkSwapchain::clearValue.push_back({ 0.098f, 0.098f, 0.439f, 1.0f });

	//4.) Create a basic Swapchain
	VkSwapchain::CreateCommandAndSyncBuffers();
	VkSwapchain::CreatePreset();
	VkImGui::Init();
}

TextureScene::~TextureScene() {

}

void TextureScene::Update(const float& _dt) {

}

void TextureScene::Render(const float& _dtRatio) {
	//Wait for Queue to be ready
	vkWaitForFences(VkGlobal::device, 1, &VkSwapchain::fence[VkSwapchain::frameCurrent], VK_TRUE, 0xFFFFFFFFFFFFFFFF);

	//Get the Frame Result
	VkResult frame_result = vkAcquireNextImageKHR(VkGlobal::device, VkSwapchain::swapchain, 0xFFFFFFFFFFFFFFFF, VkSwapchain::presentSemaphore[VkSwapchain::frameCurrent], VK_NULL_HANDLE, &VkSwapchain::frameCurrent);

	//Render to Texture ImGui
	FrameStart(VkImGui::commandBuffer[VkSwapchain::frameCurrent], VkImGui::renderPass, VkSwapchain::surfaceExtent2D, VkImGui::frameBuffer, VkImGui::clearColor);
	RenderImGui();
	FrameEnd(VkImGui::commandBuffer[VkSwapchain::frameCurrent], VkSwapchain::presentSemaphore[VkSwapchain::frameCurrent], VkImGui::semaphore[VkSwapchain::frameCurrent], VkImGui::fence[VkSwapchain::frameCurrent]);

	//Render to Swapchain
	FrameStart(VkSwapchain::commandBuffer[VkSwapchain::frameCurrent], VkSwapchain::renderPass, VkSwapchain::surfaceExtent2D, VkSwapchain::frameBuffer[VkSwapchain::frameCurrent], VkSwapchain::clearValue);
	vkCmdSetViewport(VkSwapchain::commandBuffer[VkSwapchain::frameCurrent], 0, 1, &VkSwapchain::viewport);
	vkCmdSetScissor(VkSwapchain::commandBuffer[VkSwapchain::frameCurrent], 0, 1, &VkSwapchain::scissor);
	vkCmdBindDescriptorSets(VkSwapchain::commandBuffer[VkSwapchain::frameCurrent], VK_PIPELINE_BIND_POINT_GRAPHICS, VkImGui::pipelineLayout, 0, 1, &VkImGui::descriptorSet[VkSwapchain::frameCurrent], 0, nullptr);
	vkCmdBindPipeline(VkSwapchain::commandBuffer[VkSwapchain::frameCurrent], VK_PIPELINE_BIND_POINT_GRAPHICS, VkImGui::graphicsPipeline);
	vkCmdDraw(VkSwapchain::commandBuffer[VkSwapchain::frameCurrent], 3, 1, 0, 0);
	FrameEnd(VkSwapchain::commandBuffer[VkSwapchain::frameCurrent], VkImGui::semaphore[VkSwapchain::frameCurrent], VkSwapchain::renderSemaphore[VkSwapchain::frameCurrent], VkSwapchain::fence[VkSwapchain::frameCurrent]);

	//Present
	Present();
}

void TextureScene::Reset() {
	//Cleanup
	VkImGui::CleanupImage();
	VkSwapchain::Cleanup(false);

	//Gather Info
	VkSwapchain::UpdateSurfaceData();
	VkSwapchain::surfaceCapabilities = VkGlobal::surfaceCapabilities;
	VkSwapchain::surfacePresentMode = VK_PRESENT_MODE_FIFO_KHR;
	VkSwapchain::surfaceExtent2D = VkSwapchain::surfaceCapabilities.currentExtent;
	VkSwapchain::surfaceExtent3D = { VkSwapchain::surfaceExtent2D.width, VkSwapchain::surfaceExtent2D.height, 1 };
	VkSwapchain::surfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
	VkSwapchain::surfaceFormat.colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
	VkSwapchain::depthFormat = VK_FORMAT_D32_SFLOAT;

	//Reset
	VkSwapchain::CreatePreset(false);
	VkImGui::ResetImage();
}

void TextureScene::Cleanup() {
	Scene::Cleanup();
}

bool TextureScene::CheckRoomChange() {
	if (Scene::ChangeRoom) {
		Cleanup();
		Scene::ChangeRoom = false;
		return true;
	}
	return false;
}

void TextureScene::RenderImGui() {
	// Start the Dear ImGui frame
	ImGui_ImplVulkan_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Test Menu");
	if (ImGui::Button("<-"))
		Scene::ChangeRoom = true;

	ImGui::End();

	//Set to Render
	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), VkImGui::commandBuffer[VkSwapchain::frameCurrent]);
}