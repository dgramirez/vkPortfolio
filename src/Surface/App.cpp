
namespace {
	//Initialization
	GW::SYSTEM::GWindow GWindow;
	void GetResolution(uint32_t& _x, uint32_t& _y);
	void GetScreenSize(uint32_t& _x, uint32_t& _y, uint32_t& _w, uint32_t& _h);

	bool isRunnable = true;
}

namespace App {

	void Init() {
		//Create the Window Surface
		uint32_t x, y, w, h;
		::GetResolution(x, y);
		::GetScreenSize(x, y, w, h);
		if (-GWindow.Create(x, y, w, h, GW::SYSTEM::GWindowStyle::WINDOWEDBORDERED))
		{
			isRunnable = false;
			return;
		}
	}
	
	void Run() {
		//Ensure that its runnable
		if (isRunnable) {
			//Process the Window Events
			while (+GWindow.ProcessWindowEvents()) {

			}
		}
	}

	void Cleanup() {

	}
}

void ::GetResolution(uint32_t& _x, uint32_t& _y) {
#ifdef _WIN32
	RECT MyWindow;
	HWND dWin = GetDesktopWindow();
	GetWindowRect(dWin, &MyWindow);
	_x = MyWindow.right;
	_y = MyWindow.bottom;
#else
	_x = 0;
	_y = 0;
#endif
}

void ::GetScreenSize(uint32_t& _x, uint32_t& _y, uint32_t& _w, uint32_t& _h) {
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