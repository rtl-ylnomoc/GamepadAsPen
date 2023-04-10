#pragma once
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

// GAMEPAD
// RAWINPUT size, usually 47 for my xbox 360 gamepad on x64 and 39 or 47 on x86, but may throw 122 error if set to those. This value should work for both archs.
inline UINT dwSize = 128;
inline auto pRaw = reinterpret_cast<RAWINPUT*>(malloc(dwSize)); // will contain gamepad data; dwSizeHid = 15, count = 1
// Shortcut buttons
// Every input will hold until ALL buttons are released or when you press and release an individual corresponding input yourself
inline BYTE gpadBtns;				// gamepad button flags on 11th byte
inline BYTE stickDpadBtns;			// stick and dpad button flags on 12th byte
inline INPUT inputs[10];			// inputs per shortcut on all buttons (only 1 shortcut held is possible. Or more if you are fast to do them in one WM_INPUT event).
inline char inpSent = 0;			// amount of inputs sent also acts as a pointer to the next available array index
// flag byte indicating stick buttons being previously pressed to prevent double inputs:
inline BYTE stickBtnPressed = 0;	// 0b01 - for tilt lock; 0b10 - for rotation lock.
// Shortcut states togglers
inline WORD CtrAlt = VK_LCONTROL;	// either equal to VK_LCONTROL or VK_LMENU, depends on the checkbox.
inline bool shiftPressed = false;	// this defines shift toggle state. Exists to de/press shift with keybd_event which works better with it.

// POINTER
inline POINT curPos;
inline UINT32 curPressure;
inline float rotationX, rotationY, rotationLength;
inline bool rotationLocked = false, tiltLocked = false;
inline POINTER_FLAGS pointerFlags;
inline POINTER_BUTTON_CHANGE_TYPE ButtonChangeType;	// buttons state change
inline DWORD dwKeyStates;							// modifiers state
inline HSYNTHETICPOINTERDEVICE hPen;

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