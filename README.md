# Gamepad As Pen

As the description says, this app converts gamepad raw input (tested only on xbox 360 controller) to pen input using WinApi (obviously only Windows is supported).  
This means, you can use your gamepad analog input to emulate drawing pen's pressure sensitivity, tilt and rotation (which is unused mostly).  
upd. Manifest now specifies that this executable now has Windows 8 and above features, so it may not work on anything older than that.

I haven't found ANY alternatives of such program in a month time span. The only things I've found that emulate pen input are drivers that convert mobile input to drawing tablet input and some random laggy app written with SDL which does things similar to my program, except that it does so only in it's own app window (does not inject input to other apps at all), that thing was essentially a weird featureless drawing-with-gamepad app.  
So I just learned C++ and WinApi a little and made this (don't try this at home the documentation and the API itself is a Machiavellian horror).

## Launch

Simply launch the executable.  
Note that Windows Defender is wary of this app.

### !IMPORTANT! Injected pen input from InjectSyntheticPointerInput function won't be captured by applications with higher integrity levels. That's why it's better to launch the program as an administrator (same as for SendInput see https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-sendinput#remarks).

## Setup

To setup repo to work in Visual Studio just cloning and launching .sln file should be enough.

## Gamepad input map

![GamepadInputMap](https://user-images.githubusercontent.com/24192051/228993390-bfc2a2a4-e39b-409d-8fe8-0711ea822a44.png)

If you want to map your own shortcuts, you can simply modify ShortcutInputHandlers.hpp by adding key and mouse presses in the corresponding handlers and if clauses   (changing LT, RT, RB, LS, RS and stick buttons behaviour would require a more significant rewriting and understanding of the code structure).  

You can also easily make assigntments to unused buttons, such as Y, A, BACK and START (DPAD diagonal buttons can be used too, but you would press them accidentally a lot).  
Note that big xbox guide button in the center is not open to the public API and isn't being sent anywhere in raw data byte array; however, it still sends WM_INPUT messages, just like normal buttons.

## Structure

No idea how to structure C++ code, pretty new to this language.  
1. Lots of headers because I saw that importing a library using only a header was good practice, so I did the structure how I thought it was implemented there.  
1. Lots of globals for zero allocations and near zero stack pushing popping when program starts message handling for maximum performance which is important for user input applcications such as this one
1. Lots of inlines because the IDE told me to do it and also it might reduce stack usage overhead as well.

## Viruses?

Some VirusTotal results with the first release:  
  
https://www.virustotal.com/gui/file/c6940e7161606d43d86013fa2e0cec61ffdfad0a00271220990592786d9bd57c/behavior
  
### Detection

![image](https://user-images.githubusercontent.com/24192051/229059236-955924c8-fd82-4c01-bfb4-6b80438458a8.png)

### Relations

![image](https://user-images.githubusercontent.com/24192051/229059507-271784a1-aea3-49fa-afdb-75d3fdb2ae30.png)

### Behaviour

![image](https://user-images.githubusercontent.com/24192051/229059872-3bcda221-892b-499e-b522-c8b109837d0c.png)

It seems that the main reasons causing this sometimes to be treated as a virus are:
1. Removed non-client area
2. Transparent client area
3. Obviously, reading and injecting input  

Above suspicious behaviour is pretty much required considering the intended purpose of the application.
If you have .exe phobia then you can read the source code and build it yourself anyhow.
