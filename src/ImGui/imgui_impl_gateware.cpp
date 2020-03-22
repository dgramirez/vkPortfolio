// dear imgui: Platform Binding for Gateware (64 bit applications only)
// This needs to be used along with a Renderer (e.g. DirectX11, OpenGL3, Vulkan..)

// Implemented features:
// [X] NOTHING! :D

#include "imgui.h"
#include "imgui_impl_gateware.h"
#include "gateware.h"

// Gateware Data
static GW::SYSTEM::GWindow			g_GWindow;
static GW::INPUT::GBufferedInput	g_GBufferedInput;
static GW::CORE::GEventReceiver		g_GBIReceiver;
static unsigned long long			g_Time = 0;
static unsigned long long			g_TicksPerSecond = 0;
static ImGuiMouseCursor				g_LastMouseCursor = ImGuiMouseCursor_COUNT;
static bool							g_HasGamepad = false;
static bool							g_WantUpdateHasGamepad = true;

//Helper Functions
void AddCharacter(const uint32_t& _data);

//Functions
bool	ImGui_ImplGateware_Init(void* gwindow)
{
	g_GWindow = *(static_cast<GW::SYSTEM::GWindow*>(gwindow));

	//Setup back-end capabilities flag
	ImGuiIO& io = ImGui::GetIO();
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
	io.BackendPlatformName = "imgui_impl_gateware";

	//Get Width and Heigh
	uint32_t w, h;
	g_GWindow.GetClientWidth(w);
	g_GWindow.GetClientHeight(h);
	io.DisplaySize = ImVec2((float)w, (float)h);

	// Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array that we will update during the application lifetime.
	io.KeyMap[ImGuiKey_Tab] = G_KEY_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = G_KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = G_KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = G_KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow] = G_KEY_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = G_KEY_PAGEUP;
	io.KeyMap[ImGuiKey_PageDown] = G_KEY_PAGEDOWN;
	io.KeyMap[ImGuiKey_Home] = G_KEY_HOME;
	io.KeyMap[ImGuiKey_End] = G_KEY_END;
	io.KeyMap[ImGuiKey_Insert] = G_KEY_INSERT;
	io.KeyMap[ImGuiKey_Delete] = G_KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = G_KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Space] = G_KEY_SPACE;
	io.KeyMap[ImGuiKey_Enter] = G_KEY_ENTER;
	io.KeyMap[ImGuiKey_Escape] = G_KEY_ESCAPE;
	io.KeyMap[ImGuiKey_KeyPadEnter] = G_KEY_NUMPAD_PLUS;
	io.KeyMap[ImGuiKey_A] = G_KEY_A;
	io.KeyMap[ImGuiKey_C] = G_KEY_C;
	io.KeyMap[ImGuiKey_V] = G_KEY_V;
	io.KeyMap[ImGuiKey_X] = G_KEY_X;
	io.KeyMap[ImGuiKey_Y] = G_KEY_Y;
	io.KeyMap[ImGuiKey_Z] = G_KEY_Z;

#if _WIN32
	GW::SYSTEM::UNIVERSAL_WINDOW_HANDLE uwh;
	g_GWindow.GetWindowHandle(uwh);
	io.ImeWindowHandle = uwh.window;
#endif

	//Create Callback for GBufferedInput
	g_GBufferedInput.Create(g_GWindow);
	g_GBIReceiver.Create(g_GBufferedInput, [&]() {
		//GEvent Setup [Getting Events]
		GW::GEvent gEvent;
		g_GBIReceiver.Pop(gEvent);

		//Reading GWindow's Events & Data
		GW::INPUT::GBufferedInput::Events Event;
		GW::INPUT::GBufferedInput::EVENT_DATA EventData;
		gEvent.Read(Event, EventData);

		//Do Mouse
		io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
		bool isFocus;
		g_GWindow.IsFocus(isFocus);
		if (isFocus)
			io.MousePos = ImVec2(EventData.x, EventData.y);

		switch (Event) {
		case GW::INPUT::GBufferedInput::Events::KEYPRESSED:
			if (EventData.data < 256)
				io.KeysDown[EventData.data] = 1;
			io.KeyCtrl = io.KeysDown[G_KEY_CONTROL];
			io.KeyShift = io.KeysDown[G_KEY_LEFTSHIFT] || io.KeysDown[G_KEY_RIGHTSHIFT];
			io.KeyAlt = io.KeysDown[G_KEY_LEFTALT] || io.KeysDown[G_KEY_RIGHTALT];
			AddCharacter(EventData.data);
			break;
		case GW::INPUT::GBufferedInput::Events::KEYRELEASED:
			if (EventData.data < 256)
				io.KeysDown[EventData.data] = 0;
			io.KeyCtrl = io.KeysDown[G_KEY_CONTROL];
			io.KeyShift = io.KeysDown[G_KEY_LEFTSHIFT] || io.KeysDown[G_KEY_RIGHTSHIFT];
			io.KeyAlt = io.KeysDown[G_KEY_LEFTALT] || io.KeysDown[G_KEY_RIGHTALT];
			break;
		case GW::INPUT::GBufferedInput::Events::BUTTONPRESSED:
			if (EventData.data == G_BUTTON_LEFT)   io.MouseDown[0] = true;
			if (EventData.data == G_BUTTON_RIGHT)  io.MouseDown[1] = true;
			if (EventData.data == G_BUTTON_MIDDLE) io.MouseDown[2] = true;
			break;
		case GW::INPUT::GBufferedInput::Events::BUTTONRELEASED:
			if (EventData.data == G_BUTTON_LEFT)   io.MouseDown[0] = false;
			if (EventData.data == G_BUTTON_RIGHT)  io.MouseDown[1] = false;
			if (EventData.data == G_BUTTON_MIDDLE) io.MouseDown[2] = false;
			break;
		case GW::INPUT::GBufferedInput::Events::MOUSESCROLL:
			if (EventData.data == G_MOUSE_SCROLL_UP)
				io.MouseWheel += 1;
			else
				io.MouseWheel -= 1;
			break;
		}
		});

	return true;
}

IMGUI_IMPL_API void ImGui_ImplGateware_Shutdown()
{
	g_GBIReceiver = nullptr;
	g_GBufferedInput = nullptr;
	g_GWindow = nullptr;
}

IMGUI_IMPL_API void ImGui_ImplGateware_NewFrame(const float& dt)
{
	ImGuiIO& io = ImGui::GetIO();
	IM_ASSERT(io.Fonts->IsBuilt() && "Font atlas not built! It is generally built by the renderer back-end. Missing call to renderer _NewFrame() function? e.g. ImGui_ImplOpenGL3_NewFrame().");

	// Setup display size (every frame to accommodate for window resizing)
	uint32_t w, h;
	g_GWindow.GetClientWidth(w);
	g_GWindow.GetClientHeight(h);
	io.DisplaySize = ImVec2((float)w, (float)h);

	// Setup time step
	io.DeltaTime = dt;

	// Update OS Mouse Position
}

void AddCharacter(const uint32_t& _data) {
	ImGuiIO& io = ImGui::GetIO();
	uint32_t capital = 32 * io.KeyShift;
	switch (_data) {
	case G_KEY_A:
		io.AddInputCharacter('a' ^ capital);
		break;
	case G_KEY_B:
		io.AddInputCharacter('b' ^ capital);
		break;
	case G_KEY_C:
		io.AddInputCharacter('c' ^ capital);
		break;
	case G_KEY_D:
		io.AddInputCharacter('d' ^ capital);
		break;
	case G_KEY_E:
		io.AddInputCharacter('e' ^ capital);
		break;
	case G_KEY_F:
		io.AddInputCharacter('f' ^ capital);
		break;
	case G_KEY_G:
		io.AddInputCharacter('g' ^ capital);
		break;
	case G_KEY_H:
		io.AddInputCharacter('h' ^ capital);
		break;
	case G_KEY_I:
		io.AddInputCharacter('i' ^ capital);
		break;
	case G_KEY_J:
		io.AddInputCharacter('j' ^ capital);
		break;
	case G_KEY_K:
		io.AddInputCharacter('k' ^ capital);
		break;
	case G_KEY_L:
		io.AddInputCharacter('l' ^ capital);
		break;
	case G_KEY_M:
		io.AddInputCharacter('m' ^ capital);
		break;
	case G_KEY_N:
		io.AddInputCharacter('n' ^ capital);
		break;
	case G_KEY_O:
		io.AddInputCharacter('o' ^ capital);
		break;
	case G_KEY_P:
		io.AddInputCharacter('p' ^ capital);
		break;
	case G_KEY_Q:
		io.AddInputCharacter('q' ^ capital);
		break;
	case G_KEY_R:
		io.AddInputCharacter('r' ^ capital);
		break;
	case G_KEY_S:
		io.AddInputCharacter('s' ^ capital);
		break;
	case G_KEY_T:
		io.AddInputCharacter('t' ^ capital);
		break;
	case G_KEY_U:
		io.AddInputCharacter('u' ^ capital);
		break;
	case G_KEY_V:
		io.AddInputCharacter('v' ^ capital);
		break;
	case G_KEY_W:
		io.AddInputCharacter('w' ^ capital);
		break;
	case G_KEY_X:
		io.AddInputCharacter('x' ^ capital);
		break;
	case G_KEY_Y:
		io.AddInputCharacter('y' ^ capital);
		break;
	case G_KEY_Z:
		io.AddInputCharacter('z' ^ capital);
		break;
	case G_KEY_0:
		if (capital)
			io.AddInputCharacter(')');
		else
			io.AddInputCharacter('0');
		break;
	case G_KEY_1:
		if (capital)
			io.AddInputCharacter('!');
		else
			io.AddInputCharacter('1');
		break;
	case G_KEY_2:
		if (capital)
			io.AddInputCharacter('@');
		else
			io.AddInputCharacter('2');
		break;
	case G_KEY_3:
		if (capital)
			io.AddInputCharacter('#');
		else
			io.AddInputCharacter('3');
		break;
	case G_KEY_4:
		if (capital)
			io.AddInputCharacter('$');
		else
			io.AddInputCharacter('4');
		break;
	case G_KEY_5:
		if (capital)
			io.AddInputCharacter('%');
		else
			io.AddInputCharacter('5');
		break;
	case G_KEY_6:
		if (capital)
			io.AddInputCharacter('^');
		else
			io.AddInputCharacter('6');
		break;
	case G_KEY_7:
		if (capital)
			io.AddInputCharacter('&');
		else
			io.AddInputCharacter('7');
		break;
	case G_KEY_8:
		if (capital)
			io.AddInputCharacter('*');
		else
			io.AddInputCharacter('8');
		break;
	case G_KEY_9:
		if (capital)
			io.AddInputCharacter('(');
		else
			io.AddInputCharacter('9');
		break;
	case G_KEY_ADD:
		io.AddInputCharacter('+');
		break;
	case G_KEY_MULTIPLY:
		io.AddInputCharacter('*');
		break;
	case G_KEY_DIVIDE:
		io.AddInputCharacter('/');
		break;
	case G_KEY_MINUS:
		if (capital)
			io.AddInputCharacter('_');
		else
			io.AddInputCharacter('-');
		break;
	case G_KEY_EQUALS:
		if (capital)
			io.AddInputCharacter('+');
		else
			io.AddInputCharacter('=');
		break;
	case G_KEY_BRACKET_OPEN:
		if (capital)
			io.AddInputCharacter('{');
		else
			io.AddInputCharacter('[');
		break;
	case G_KEY_BRACKET_CLOSE:
		if (capital)
			io.AddInputCharacter('}');
		else
			io.AddInputCharacter(']');
		break;
	case G_KEY_COLON:
		if (capital)
			io.AddInputCharacter(':');
		else
			io.AddInputCharacter(';');
		break;
	case G_KEY_QUOTE:
		if (capital)
			io.AddInputCharacter('"');
		else
			io.AddInputCharacter('\'');
		break;
	case G_KEY_TILDE:
		if (capital)
			io.AddInputCharacter('~');
		else
			io.AddInputCharacter('`');
		break;
	case G_KEY_BACKSLASH:
		if (capital)
			io.AddInputCharacter('|');
		else
			io.AddInputCharacter('\\');
		break;
	case G_KEY_COMMA:
		if (capital)
			io.AddInputCharacter('<');
		else
			io.AddInputCharacter(',');
		break;
	case G_KEY_PERIOD:
		if (capital)
			io.AddInputCharacter('>');
		else
			io.AddInputCharacter('.');
		break;
	case G_KEY_FOWARDSLASH:
		if (capital)
			io.AddInputCharacter('?');
		else
			io.AddInputCharacter('/');
		break;
	case G_KEY_SPACE:
		io.AddInputCharacter(' ');
		break;
	case G_KEY_NUMPAD_MINUS:
		io.AddInputCharacter('-');
		break;
	case G_KEY_NUMPAD_PLUS:
		io.AddInputCharacter('+');
		break;
	}
}
