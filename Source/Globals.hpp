#pragma once
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "UxTheme.lib")
#include <chrono>
#include <Windows.h>


#ifdef _DEBUG
int counter = 0;
std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
constexpr size_t BUF_SIZE = 200;	// size for buffer
char buffer[BUF_SIZE];				// buffer for some debug info
size_t bufPtr = 0;					// last unused index
#endif

// WINDOW
inline WNDCLASSEX wc{ sizeof(WNDCLASSEX) }; // defined as global for button windows creation
inline auto wndPlacement = new WINDOWPLACEMENT{ sizeof(WINDOWPLACEMENT) };
HDC hdcStatic;

// GAMEPAD
inline UINT cbSize;					// RAWINPUT size returned by GetRawInputData()
inline UINT dwSize = 47;			// RAWINPUT size, const 47 for my xbox gamepad on x64 and 39 or 47 on x86. This value works for both archs.
inline auto pRaw = reinterpret_cast<RAWINPUT*>(malloc(dwSize)); // will contain gamepad data; dwSizeHid = 15, count = 1
// Shortcut buttons
// Every input will hold until ALL buttons are released or when you press and release an individual corresponding input yourself
inline BYTE gpadBtns;
inline BYTE stickDpadBtns;			// stick and dpad button flags byte
inline INPUT inputs[5];				// inputs per press on all buttons (only 1 press allowed).
inline char inpSent = 0;			// amount of inputs sent also acts as a pointer to the next available array index
// flag byte indicating stick buttons being previously pressed to prevent double inputs:
inline BYTE stickBtnPressed = 0;	// 0b01 - for tilt lock; 0b10 - for rotation lock.
inline WORD CtrAlt = VK_LCONTROL;	// either equals to VK_LCONTROL or VK_LMENU, depends on the checkbox.

// POINTER
inline POINT curPos;
inline UINT32 curPressure;
inline float rotationX, rotationY, rotationLength;
inline bool rotationLocked = false, tiltLocked = false;
inline POINTER_FLAGS pointerFlags;
inline POINTER_BUTTON_CHANGE_TYPE ButtonChangeType; // buttons state change
inline DWORD dwKeyStates;			// modifiers state
inline HSYNTHETICPOINTERDEVICE hPen = CreateSyntheticPointerDevice(PT_PEN, 1, POINTER_FEEDBACK_DEFAULT);

inline auto ptInfo = new POINTER_TYPE_INFO{
	.type = PT_PEN,
	.penInfo{
		.pointerInfo{
			.pointerType = PT_PEN,
			.pointerId = 19171921,	// probably shouldn't change it in runtime and it also might interfere with other ids?
			.pointerFlags = POINTER_FLAG_NONE,
			.sourceDevice = hPen,
			.historyCount = 1,		// in my case it's an amount of pointer infos per inject
			.dwKeyStates = 0,		// ctrl and shift modifiers state
			.ButtonChangeType = POINTER_CHANGE_NONE
		},
		.penFlags = PEN_FLAG_NONE,
		.penMask = PEN_MASK_PRESSURE | PEN_MASK_ROTATION | PEN_MASK_TILT_X | PEN_MASK_TILT_Y,
		.pressure = 0,
		.rotation = 0,
		.tiltX = 0,
		.tiltY = 0
	}
};