#include "StartScene.h"
#include "../../Vulkan/VkGlobals.h"
#include "../../ImGui/ImGuiGlobals.h"

/////////////////////////
// Start Scene Methods //
/////////////////////////
StartScene::StartScene() {
	//Initialize the Start Scene
	Initialize();
}
StartScene::~StartScene() {
	Cleanup();
}

void StartScene::Render(const float& _dtRatio) {
	//Viewport State
	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = VkGlobal::surfaceCapabilities.currentExtent.width;
	viewport.height = VkGlobal::surfaceCapabilities.currentExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0,0 };
	scissor.extent = VkGlobal::surfaceCapabilities.currentExtent;

	if (canRender)
	{
//		//Wait for Queue to be ready
//		vkWaitForFences(VkGlobal::device, 1, &sceneFence[frameCurrent], VK_TRUE, 0xFFFFFFFFFFFFFFFF);
//
//		//Get the Frame Result
//		VkResult frame_result = vkAcquireNextImageKHR(VkGlobal::device, swapchain, 0xFFFFFFFFFFFFFFFF, sceneSemaphoreRF[frameCurrent], VK_NULL_HANDLE, &frameCurrent);
//
//		//Render to Texture ImGui
//		FrameStart(VkImGui::commandBuffer[frameCurrent], VkImGui::renderPass, VkImGui::frameBuffer, VkImGui::clearColor);
//		RenderImGui();
//		FrameEnd(VkImGui::commandBuffer[frameCurrent], sceneSemaphoreRF[frameCurrent], VkImGui::semaphore[frameCurrent], VkImGui::fence[frameCurrent]);
//
//		//Render to Swapchain
//		FrameStart(commandBuffer[frameCurrent], renderPass, swapchainFramebuffer[frameCurrent], clearColor);
//		vkCmdSetViewport(commandBuffer[frameCurrent], 0, 1, &viewport);
//		vkCmdSetScissor(commandBuffer[frameCurrent], 0, 1, &scissor);
//		vkCmdBindDescriptorSets(commandBuffer[frameCurrent], VK_PIPELINE_BIND_POINT_GRAPHICS, VkImGui::pipelineLayout, 0, 1, &VkImGui::descriptorSet[frameCurrent], 0, nullptr);
//		vkCmdBindPipeline(commandBuffer[frameCurrent], VK_PIPELINE_BIND_POINT_GRAPHICS, VkImGui::graphicsPipeline);
//		vkCmdDraw(commandBuffer[frameCurrent], 3, 1, 0, 0);
//		FrameEnd(commandBuffer[frameCurrent], VkImGui::semaphore[frameCurrent], sceneSemaphoreRF[frameCurrent], sceneFence[frameCurrent]);
//
//		//Present
//		Present();
	}
}
void StartScene::RenderImGui() {
	// Start the Dear ImGui frame
	ImGui_ImplVulkan_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Test Menu");
	ImGui::Checkbox("Show About Window", &yes);
	ImGui::ShowMetricsWindow(&yes);

	bool leave = ImGui::Button("Exit");
	if (leave)
		ImGui_ImplGateware_Shutdown();
	ImGui::End();

	//Set to Render
	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), VkImGui::commandBuffer[frameCurrent]);
}

void StartScene::Initialize() {
	//1: Update the Surface Information based on current configurations
	UpdateSurfaceData();

	//2: Setup Default Parameters
	surfaceCapabilities = VkGlobal::surfaceCapabilities;
	surfacePresentMode = VK_PRESENT_MODE_FIFO_KHR; //Default
	surfaceExtent2D = surfaceCapabilities.currentExtent;
	surfaceExtent3D = { surfaceExtent2D.width, surfaceExtent2D.height, 1 };
	surfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
	surfaceFormat.colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
	frameMax = 2;

	//3: Create Swapchain, Renderpass & Framebuffers
	CreateSwapchainPresetBasic();
	VkGlobal::frameMax = frameMax;
	VkGlobal::swapchain = swapchain;
	VkGlobal::renderPass = renderPass;
	clearColor.resize(1);
	clearColor[0] = { 0.098f, 0.098f, 0.439f, 1.0f };

	//4: Create Command Pool & Buffers
	CreateCommandPreset();

	//5: Create Semaphores and Fences
	CreateSyncPreset();
}

void StartScene::Cleanup() {
	//Wait for device
	if (VkGlobal::device)
		vkDeviceWaitIdle(VkGlobal::device);

	//Remove Fence
	if (sceneFence.size()) {
		for (auto fence : sceneFence) {
			vkDestroyFence(VkGlobal::device, fence, VK_NULL_HANDLE);
		}
		sceneFence.clear();
		sceneFence.shrink_to_fit();
	}

	//Remove Semaphore Render Finished
	if (sceneSemaphoreRF.size()) {
		for (auto semaphore : sceneSemaphoreRF) {
			vkDestroySemaphore(VkGlobal::device, semaphore, VK_NULL_HANDLE);
		}
		sceneSemaphoreRF.clear();
		sceneSemaphoreRF.shrink_to_fit();
	}

	//Remove Semaphore Image Available
	if (sceneSemaphoreIA.size()) {
		for (auto semaphore : sceneSemaphoreIA) {
			vkDestroySemaphore(VkGlobal::device, semaphore, VK_NULL_HANDLE);
		}
		sceneSemaphoreIA.clear();
		sceneSemaphoreIA.shrink_to_fit();
	}

	//Remove Command Pool
	if (commandPool) {
		vkDestroyCommandPool(VkGlobal::device, commandPool, VK_NULL_HANDLE);
		commandPool = {};
		commandBuffer.clear();
		commandBuffer.shrink_to_fit();
	}

	//Remove Framebuffer
	if (swapchainFramebuffer.size()) {
		for (auto framebuffer : swapchainFramebuffer) {
			vkDestroyFramebuffer(VkGlobal::device, framebuffer, VK_NULL_HANDLE);
		}
		swapchainFramebuffer.clear();
		swapchainFramebuffer.shrink_to_fit();
	}

	//Remove Renderpass
	if (renderPass) {
		vkDestroyRenderPass(VkGlobal::device, renderPass, VK_NULL_HANDLE);
		renderPass = {};
	}

	//Remove Swapchain Image Views
	if (swapchainImageView.size()) {
		for (auto imageView : swapchainImageView) {
			vkDestroyImageView(VkGlobal::device, imageView, VK_NULL_HANDLE);
		}
		swapchainImageView.clear();
		swapchainImageView.shrink_to_fit();
	}

	//Remove Swapchain
	if (swapchain) {
		vkDestroySwapchainKHR(VkGlobal::device, swapchain, VK_NULL_HANDLE);
		swapchain = {};
		swapchainImage.clear();
		swapchainImage.shrink_to_fit();
	}

	canRender = false;
}
