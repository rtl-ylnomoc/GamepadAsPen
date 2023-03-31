# Gamepad As Pen

As the description says, this app converts gamepad raw input (tested only on xbox 360 controller) to pen input using WinApi (obviously only Windows is supported).  
This means, you can use your gamepad analog input to emulate drawing pen's pressure sensitivity, tilt and rotation (which is unused mostly).  

## Launch

Simply launch the executable.  
Note that Windows Defender is wary of this app.

### !IMPORTANT! Injected pen input from InjectSyntheticPointerInput function won't be captured by applications with higher integrity levels. That's why it's better to launch the program as an administrator (same as for SendInput see https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-sendinput#remarks).

## Setup

To setup repo to work in Visual Studio just cloning and launching .sln file should be enough.

## Gamepad input map

![GamepadInputMap](https://user-images.githubusercontent.com/24192051/228993390-bfc2a2a4-e39b-409d-8fe8-0711ea822a44.png)

If you want to map your own shortcuts, you can simply modify ShortcutInputHandlers.hpp by adding key and mouse presses in the corresponding handlers and if clauses   (changing LT, RT, RB, LS, RS and stick buttons behaviour would require a more significant rewriting and understanding of the code structure).  
You can also easily make assigntments to unused buttons, such as Y, A, BACK and START.  
Note that big xbox guide button in the center is not open to the public API and isn't being sent as RAWINPUT; however, it still sends WM_INPUT messages, just like normal buttons

## Structure

No idea how to structure C++ code, pretty new to this language.  
1. Lots of headers because I saw that importing a library using only a header was good practice, so I did the structure how I thought it was implemented there.  
1. Lots of globals for zero allocations and near zero stack pushing popping when program starts message handling for maximum performance which is important for user input applcications such as this one
1. Lots of inlines because the IDE told me to do it and also it might reduce stack usage overhead as well.
