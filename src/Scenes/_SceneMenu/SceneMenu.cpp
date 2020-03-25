#include "SceneMenu.h"
#include "../Scenes.h"
#include "../../Vulkan/VkGlobals.h"
#include "../../ImGui/ImGuiGlobals.h"

SceneMenu::SceneMenu(Scene*& _pScene)
	: m_CurrentScene(_pScene), OfCourse(true), tab_bar_flags(ImGuiTabBarFlags_None), index(-1), ExitApplication(false) {
	RegisterScene();
	Init();
}
SceneMenu::~SceneMenu() {
	Cleanup();
}
void SceneMenu::Init() {
	//0.) Cleanup Anything Prior
	Cleanup();

	//1.) Update the Surface Information
	VkSwapchain::UpdateSurfaceData();

	//2.) Setup Surface Information for Swapchain
	VkSwapchain::surfaceCapabilities = VkGlobal::surfaceCapabilities;
	VkSwapchain::surfacePresentMode = VK_PRESENT_MODE_FIFO_KHR;
	VkSwapchain::surfaceExtent2D = VkSwapchain::surfaceCapabilities.currentExtent;
	VkSwapchain::surfaceExtent3D = { VkSwapchain::surfaceExtent2D.width, VkSwapchain::surfaceExtent2D.height, 1 };
	VkSwapchain::surfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
	VkSwapchain::surfaceFormat.colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
	ImGuiWindowFlags = 0;

	//3.) Setup Other Swapchain Proeprties
	VkSwapchain::presetFlags = 0;
	VkSwapchain::frameMax = 2;
	VkSwapchain::depthFormat = {};
	VkSwapchain::clearValue.push_back({ 0.098f, 0.098f, 0.439f, 1.0f });

	//4.) Create the Command Pool, Buffer and Semaphore, Fence [Do this once only!]
	VkSwapchain::CreateCommandAndSyncBuffers();

	//5.) Create a basic Swapchain
	VkSwapchain::CreatePreset(true);

	//6.) Initialize ImGui for Vulkan
	VkImGui::Init();
}

void SceneMenu::Render(const float& _dtRatio) {
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
void SceneMenu::RenderImGui() {
	// Start the Dear ImGui frame
	ImGui_ImplVulkan_NewFrame();
	ImGui::NewFrame();
	ImGui::Begin("Main Menu");

	//Creating Tabs (Scenes, Options, ImGui and Credits)
	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		if (ImGui::BeginTabItem("Scenes"))
		{
			if (ImGui::CollapsingHeader("Isolated Scenes: Simple")) {
					for (uint32_t i = 0; i < m_Scenes.size(); ++i)
						if (ImGui::Selectable(m_Scenes[i].first, false)) {
							index = i;
							Scene::ChangeRoom = true;
							break;
						}
			}
			if (ImGui::CollapsingHeader("Isolated Scenes: Advanced")) {
				ImGui::Text("To be continued...");
			}
			if (ImGui::CollapsingHeader("Complex Scenes")) {
				ImGui::Text("To be continued...");
			}
			ImGui::NewLine();
			if (ImGui::Selectable("Exit Application", false)) {
				ExitApplication = true;
			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Options"))
		{
			ImGui::Text("Note: This is only a setup for the mean time.");
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("ImGui"))
		{
			ImGui::CheckboxFlags("User Guide", &ImGuiWindowFlags, 0x1);
			if (ImGuiWindowFlags & 0x1)
				ImGui::ShowUserGuide();
			ImGui::CheckboxFlags("About Window", &ImGuiWindowFlags, 0x2);
			ImGui::CheckboxFlags("Demo Window", &ImGuiWindowFlags, 0x4);
			ImGui::CheckboxFlags("Metric Window", &ImGuiWindowFlags, 0x8);
			ImGui::CheckboxFlags("About Style Editor", &ImGuiWindowFlags, 0x10);
			if (ImGuiWindowFlags & 0x10)
				ImGui::ShowStyleEditor();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Credits"))
		{
			ImGui::Text("Credits for this system setup: ");
			ImGui::BulletText("7thGate Software .LLC - Gateware");
			ImGui::BulletText("Omar Cornut - Dear ImGui");
			ImGui::BulletText("Advanced Micro Devices, Inc. - VMA Allocator"); 
			ImGui::BulletText("Sean Barrett - stb image");
			ImGui::BulletText("Vladimir V.Markelov - Console Colors [Debug]");
			ImGui::Text("All the credits were obtain from the LICENSE files\nin each directory inside dep.");
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();

		//ImGui Tests
		if (ImGuiWindowFlags & 0x2)
			ImGui::ShowAboutWindow(&OfCourse);
		if (ImGuiWindowFlags & 0x4)
			ImGui::ShowDemoWindow(&OfCourse);
		if (ImGuiWindowFlags & 0x8)
			ImGui::ShowMetricsWindow(&OfCourse);

		//Exit Application
		if (ExitApplication) {
			ImGui::Begin("Exit Application", &ExitApplication, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
			ImGui::SetWindowPos({ VkSwapchain::surfaceExtent2D.width / 2.0f - 170.0f, VkSwapchain::surfaceExtent2D.height / 2.0f - 44.0f });
			ImGui::SetWindowSize({340, 88});
			ImGui::Text("Are you sure you want to exit the application?");
			ImGui::NewLine();
			if (ImGui::Button("No", {75, 0}))
				ExitApplication = false;
			ImGui::SameLine(ImGui::GetWindowWidth() - 90);
			if (ImGui::Button("Yes", {75, 0}))
				Scene::ChangeRoom = true;
			ImGui::End();
		}
	}

	//Set to Render
	ImGui::End();
	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), VkImGui::commandBuffer[VkSwapchain::frameCurrent]);
}
bool SceneMenu::CheckRoomChange() {
	if (Scene::ChangeRoom) {
		Cleanup();

		if (index == -1)
			return true;

		m_CurrentScene = m_Scenes[index].second();
		index = -1;
		Scene::ChangeRoom = false;
	}
	return false;
}

//Registering Scenes
#include "../A_Texture/TextureScene.h"

void SceneMenu::RegisterScene() {
	//Basic Scenes
	RegisterScene<TextureScene>("Texturing");
}
