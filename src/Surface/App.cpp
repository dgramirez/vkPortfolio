#include "../VkGlobals.h"
#include "../Scenes/Scenes.h"
#include "VkCore.h"
#include <chrono>

namespace {
	//Initialization - Window
	GW::SYSTEM::GWindow GWindow;
	GW::CORE::GEventReceiver GWindowReceiver;
	
	//Initialization - Scenes
	SceneMenu* Menu = nullptr;
	Scene* CurrentScene = nullptr;

	//Initialization - Input
	GW::INPUT::GInput GInput;

	bool isRunnable = true;
}

void GetResolution(uint32_t& _x, uint32_t& _y);
void GetScreenSize(uint32_t& _x, uint32_t& _y, uint32_t& _w, uint32_t& _h);
void GWindowEvent();

namespace App {

	void Init() {
		//Create the Window Surface
		uint32_t x, y, w, h;
		GetResolution(x, y);
		GetScreenSize(x, y, w, h);
		if (-GWindow.Create(x, y, w, h, GW::SYSTEM::GWindowStyle::WINDOWEDBORDERED)) {
			isRunnable = false;
			return;
		}
		GWindow.SetWindowName("Derrick Ramirez's VkPortfolio");
		GW::SYSTEM::UNIVERSAL_WINDOW_HANDLE uwh;
		GWindow.GetWindowHandle(uwh);

		//Setup the GWindow Receiver
		if (-GWindowReceiver.Create(GWindow, [&]() { GWindowEvent(); }))
		{
			isRunnable = false;
			return;
		}
		
		//Create Vulkan Core
		VkCore::vkInit(uwh);

		//Setup ImGui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.WantCaptureMouse = true;
		io.WantCaptureKeyboard = true;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		
		//Setup Dear ImGui style
		ImGui::StyleColorsDark();
		ImGui_ImplGateware_Init(&GWindow);
	}
	
	void Run() {
		//Ensure that its runnable
		if (isRunnable) {
			//Setup Menus
			CurrentScene = new StartScene();
			Menu = new SceneMenu(CurrentScene);
			ImGuiGlobal::Init_vkImGui();

			//Setup Time
			double t = 0.0;
			const double dt = 1.0/60.0;
			double ratio = 0.0;

			auto current_time = std::chrono::high_resolution_clock::now();
			double accumulator = 0;

			//Process the Window Events
			while (+GWindow.ProcessWindowEvents()) {
				//Get Frame Time
				auto new_time = std::chrono::high_resolution_clock::now();
				double frameTime = (new_time - current_time).count() * 1e-9;
				if (frameTime > 0.25)
					frameTime = 0.25;
				current_time = new_time;

				//Increment Accumulator
				accumulator += frameTime;

				//Input
				

				//Delta Time Loop
				while (accumulator >= dt) {
					//Update
					ImGui_ImplGateware_NewFrame(dt);

					//Decrement Accumulator
					accumulator -= dt;

					//Increase Time
					t += dt;
				}

				//Get ratio [For Future Physics]
				ratio = accumulator / dt;

				//Render
				CurrentScene->Render(ratio);
			}
		}
	}

	void Cleanup() {
		delete Menu;
		delete CurrentScene;
		GWindow = nullptr;
	}
}

void GetResolution(uint32_t& _x, uint32_t& _y) {
#ifdef _WIN32
	RECT MyWindow;
	HWND dWin = GetDesktopWindow();
	GetWindowRect(dWin, &MyWindow);
	_x = MyWindow.right;
	_y = MyWindow.bottom;
#else
	Display* d = XOpenDisplay(NULL);
	Screen*  s = DefaultScreenOfDisplay(d);
	_x = s->width;
	_y = s->height;
	XCloseDisplay(d);
#endif
}
void GetScreenSize(uint32_t& _x, uint32_t& _y, uint32_t& _w, uint32_t& _h) {
	//Get the Middle Point.
	int32_t new_x = static_cast<int32_t>(_x >> 1);
	int32_t new_y = static_cast<int32_t>(_y >> 1);

	//Get the Window Dimension [Future]
	_w = 1280;
	_h = 720;

	//Offset the X any Y by half
	new_x -= _w >> 1;
	new_y -= _h >> 1;

	//Clamp Time!
	_w = (_w > _x) ? _x : _w;
	_h = (_h > _y) ? _y : _h;
	_x = (new_x < 0) ? 0 : new_x;
	_y = (new_y < 0) ? 0 : new_y;
}
void GWindowEvent() {
	//GEvent Setup [Getting Events]
	GW::GEvent gEvent;
	GWindowReceiver.Pop(gEvent);

	//Reading GWindow's Events & Data 
	GW::SYSTEM::GWindow::Events winEvent;
	GW::SYSTEM::GWindow::EVENT_DATA winEventData;
	gEvent.Read(winEvent, winEventData);

	switch (winEvent) {
	case GW::SYSTEM::GWindow::Events::DESTROY:
		ImGui_ImplGateware_Shutdown();
		CurrentScene->Cleanup();
		VkCore::vkCleanup();

		break;
	case GW::SYSTEM::GWindow::Events::DISPLAY_CLOSED:
		//Destroy Instance
		if (vkGlobal.instance) { vkDestroyInstance(vkGlobal.instance, nullptr); vkGlobal.instance = {}; }
		break;
	}
}
