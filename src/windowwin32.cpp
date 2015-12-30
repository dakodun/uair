/* **************************************************************** **
**	Uair Engine
**	Copyright (c) 20XX Iain M. Crawford
**
**	This software is provided 'as-is', without any express or
**	implied warranty. In no event will the authors be held liable
**	for any damages arising from the use of this software.
**
**	Permission is granted to anyone to use this software for any
**	purpose, including commercial applications, and to alter it
**	and redistribute it freely, subject to the following
**	restrictions:
** 
**		1. The origin of this software must not be misrepresented;
**		   you must not claim that you wrote the original
**		   software. If you use this software in a product, an
**		   acknowledgment in the product documentation would be
**		   appreciated but is not required.
**
**		2. Altered source versions must be plainly marked as such,
**		   and must not be misrepresented as being the original
**		   software.
**
**		3. This notice may not be removed or altered from any
**		   source distribution.
** **************************************************************** */

#include "windowwin32.hpp"

#include <iostream>

#include "openglcontext.hpp"
#include "exception.hpp"
#include "inputenums.hpp"

namespace uair {
unsigned int WindowWin32::mWindowCount = 0u;

WindowWin32::WindowWin32() : WindowWin32("Uair Window", WindowDisplaySettings()) {
	
}

WindowWin32::WindowWin32(const std::string & windowTitle, const WindowDisplaySettings & settings,
		const unsigned long & windowStyle) : mWinTitle(windowTitle), mWinDS(settings), mWinStyle(windowStyle) {
	
	try {
		SetUpWindow(); // set up the window
	} catch (UairException& e) {
		std::cout << e.what() << std::endl;
		throw;
	}
}

WindowWin32::WindowWin32(WindowWin32&& other) : WindowWin32() {
	swap(*this, other);
}

WindowWin32::~WindowWin32() {
	if (mWindowHandle) { // if we have a valid window handle
		if (mDeviceContext) { // if we have a valid device context
			if (wglGetCurrentDC() == mDeviceContext) {
				wglMakeCurrent(0, 0);
				CURRENTCONTEXT = nullptr;
				CURRENTCONTEXTWIN32 = nullptr;
			}
			
			ReleaseDC(mWindowHandle, mDeviceContext); // release our dc
		}
		
		DestroyWindow(mWindowHandle); // destroy our window
		
		mWindowHandle = 0; // reset handle to 0
		mDeviceContext = 0; // reset handle to 0
		mOpen = false; // indicate window is now closed
		
		--mWindowCount;
		if (mWindowCount == 0u) {
			UnregisterClassW(L"UairGLWindow", GetModuleHandle(0)); // unregister our window class
		}
	}
}

WindowWin32& WindowWin32::operator=(WindowWin32 other) {
	swap(*this, other);
	
	return *this;
}

void swap(WindowWin32& first, WindowWin32& second) {
	std::swap(first.mWindowHandle, second.mWindowHandle);
	std::swap(first.mDeviceContext, second.mDeviceContext);
	
	std::swap(first.mEventQueue, second.mEventQueue);
	
	std::swap(first.mOpen, second.mOpen);
	std::swap(first.mWinTitle, second.mWinTitle);
	std::swap(first.mWinDS, second.mWinDS);
	std::swap(first.mWinSize, second.mWinSize);
	std::swap(first.mActualSize, second.mActualSize);
	std::swap(first.mWinPos, second.mWinPos);
	std::swap(first.mWinStyle, second.mWinStyle);
	
	std::swap(first.mHasFocus, second.mHasFocus);
	std::swap(first.mCaptureCount, second.mCaptureCount);
	
	std::swap(first.storedGlobalMouse, second.storedGlobalMouse);
	std::swap(first.storedLocalMouse, second.storedLocalMouse);
}

void WindowWin32::Process() {
	MSG msg;
	
	while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) { // while there are messages waiting
		TranslateMessage(&msg); // attempt to translate message
		DispatchMessage(&msg); // dispatch message
	}
	
	{ // check if the mouse cursor has moved
		POINT globalMouse;
		GetCursorPos(&globalMouse);
		
		POINT localMouse;
		localMouse.x = globalMouse.x;
		localMouse.y = globalMouse.y;
		ScreenToClient(mWindowHandle, &localMouse);
		
		if ((globalMouse.x != storedGlobalMouse.x || globalMouse.y != storedGlobalMouse.y) ||
				(localMouse.x != storedLocalMouse.x || localMouse.y != storedLocalMouse.y)) {
			
			WindowEvent e;
			e.mType = WindowEvent::MouseMoveType;
			e.mMouseMove.mGlobalX = globalMouse.x; e.mMouseMove.mGlobalY = globalMouse.y;
			e.mMouseMove.mLocalX = localMouse.x; e.mMouseMove.mLocalY = localMouse.y;
			mEventQueue.push_back(e);
			
			storedGlobalMouse.x = globalMouse.x; storedGlobalMouse.y = globalMouse.y;
			storedLocalMouse.x = localMouse.x; storedLocalMouse.y = localMouse.y;
		}
	}
}

bool WindowWin32::Display() const {
	if (mDeviceContext) { // if we have a valid device context
		return SwapBuffers(mDeviceContext); // swap the buffers
	}
	
	return false;
}

void WindowWin32::Quit() {
	mOpen = false;
}

bool WindowWin32::IsOpen() const {
	return mOpen;
}

WindowDisplaySettings WindowWin32::GetCurrentDS() const {
	DEVMODE deviceMode;
	deviceMode.dmSize = sizeof(DEVMODE);
	deviceMode.dmDriverExtra = 32;
	
	EnumDisplaySettings(0, ENUM_CURRENT_SETTINGS, &deviceMode); // get current display settings
	
	WindowDisplaySettings ds;
	ds.mWidth = deviceMode.dmPelsWidth; ds.mHeight = deviceMode.dmPelsHeight; // get the current resolution
	ds.mColourDepth = deviceMode.dmBitsPerPel; // get the current colour depth
	
	return ds;
}

std::set<WindowDisplaySettings> WindowWin32::GetSupportedDS() const {
	DEVMODE deviceMode;
	deviceMode.dmSize = sizeof(DEVMODE);
	deviceMode.dmDriverExtra = 32;
	
	int i = 0;
	std::set<WindowDisplaySettings> supp; // set containing all supported display settings
	
	while (EnumDisplaySettings(0, i, &deviceMode) != 0) { // while there are still supported display settings
		WindowDisplaySettings ds;
		ds.mWidth = deviceMode.dmPelsWidth; ds.mHeight = deviceMode.dmPelsHeight; // get the resolution
		ds.mColourDepth = deviceMode.dmBitsPerPel; // get the colour depth
		
		supp.insert(ds); // add to the list
		
		++i; // go to next supported display settings
	}
	
	return supp;
}

unsigned int WindowWin32::GetWidth() const {
	return mWinDS.mWidth; // return the width of the window's client area
}

unsigned int WindowWin32::GetHeight() const {
	return mWinDS.mHeight; // return the height of the window's client area
}

unsigned int WindowWin32::GetActualWidth() const {
	return mActualSize.x; // return the actual width of the window (including borders)
}

unsigned int WindowWin32::GetActualHeight() const {
	return mActualSize.y; // return the actual height of the window (including borders and titlebar)
}

void WindowWin32::MakeCurrent(OpenGLContext& context) {
	wglMakeCurrent(mDeviceContext, context.mRenderContext);
	CURRENTCONTEXT = &context.mGlewContext;
	CURRENTCONTEXTWIN32 = &context.mGlewContextWin32;
}

void WindowWin32::SetUpWindow() {
	if (mWindowCount == 0u) {
		// create window attributes
		WNDCLASSEXW winClassExW;
		winClassExW.cbSize		  = sizeof(WNDCLASSEXW);
		winClassExW.style         = CS_OWNDC | CS_DBLCLKS;
		winClassExW.lpfnWndProc   = &WindowWin32::WndProc;
		winClassExW.cbClsExtra    = 0;
		winClassExW.cbWndExtra    = 0;
		winClassExW.hInstance     = GetModuleHandle(0);
		winClassExW.hIcon         = 0;
		winClassExW.hCursor       = ::LoadCursor(0, IDC_ARROW);
		winClassExW.hbrBackground = 0;
		winClassExW.lpszMenuName  = 0;
		winClassExW.lpszClassName = L"UairGLWindow";
		winClassExW.hIconSm		  = 0;
		
		RegisterClassExW(&winClassExW); // register window class
		++mWindowCount;
	}
	
	DWORD windowsStyle = 0; // no initial style
	
	{ // set the basic window styles
		if (mWinStyle & WindowStyles::Visible) {
			windowsStyle |= WS_VISIBLE;
		}
		
		if (mWinStyle & WindowStyles::Titlebar) {
			windowsStyle |= WS_CAPTION;
		}
		
		if (mWinStyle & WindowStyles::Close) {
			windowsStyle |= WS_CAPTION | WS_SYSMENU;
		}
		
		if (mWinStyle & WindowStyles::Minimise) {
			windowsStyle |= WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		}
		
		if (mWinStyle & WindowStyles::Maximise) {
			windowsStyle |= WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX;
		}
		
		if (mWinStyle & WindowStyles::Resize) {
			windowsStyle |= WS_SIZEBOX;
		}
	}
	
	DWORD windowsStyleEx = 0; // set up extended window styles
	
	// create window rectangle (width and height)
	RECT winRect;
	winRect.left = static_cast<long>(0); winRect.right = static_cast<long>(mWinDS.mWidth);
	winRect.top = static_cast<long>(0); winRect.bottom = static_cast<long>(mWinDS.mHeight);
	
	// get the actual size needed to create proper sized window (due to borders, titlebar, etc)
	AdjustWindowRectEx(&winRect, windowsStyle, false, windowsStyleEx);
	mActualSize.x = winRect.right - winRect.left;
	mActualSize.y = winRect.bottom - winRect.top;
	
	// create wide char version of window title
	DWORD dwStringSize = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, mWinTitle.c_str(), -1, 0, 0);
	std::vector<wchar_t> wsBuffer(dwStringSize);
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, mWinTitle.c_str(), static_cast<int>(mWinTitle.size()), &wsBuffer[0], dwStringSize);
	
	// create window
	mWindowHandle = CreateWindowExW(windowsStyleEx,
		L"UairGLWindow", &wsBuffer[0], windowsStyle,
		0, 0, winRect.right - winRect.left, winRect.bottom - winRect.top,
		0, 0, GetModuleHandle(0), this);
	
	if (!mWindowHandle) { // if we didn't recieve a valid window handle
		throw UairException("Unable to get a valid window handle."); // throw an exception
	}
	
	mDeviceContext = GetDC(mWindowHandle); // get the device context for our window
	if (!mDeviceContext) { // if we didn't recieve a valid device context
		throw UairException("Unable to get a valid device context."); // throw an exception
	}
	
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR), 								// The size of the PFD data structure
		1,															// The version number of the pfd
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, // Bit flags specifying properties of the PFD
		PFD_TYPE_RGBA,												// The type of pixel data
		static_cast<unsigned char>(mWinDS.mColourDepth),			// The colour-depth of the frame buffer
		0, 0, 0, 0, 0, 0, 											// R, G and B bit number and shift count in the frame buffer
		0, 0,														// Alpha bit number and shift count in the frame buffer (unsupported)
		0,															// Number of bits in the accumulation buffer
		0, 0, 0,													// R, G and B bit number in the accumulation buffer
		0,															// Alpha bit number in the accumulation buffer
		24,															// Number of bits in the depth (z) buffer
		8,															// Number of bits in the stencil buffer
		0,															// Number of auxiliary buffers in the frame buffer (unsupported)
		PFD_MAIN_PLANE,												// Layer type (deprecated)
		0,															// Number of overlay/underlay planes
		0, 0, 0														// Layer mask (deprecated), Visible mask and Damage mask (deprecated)
	};
	
	// find an appropriate pixel format supported by the device context that matches the given PFD
	int indPixelFormat = ChoosePixelFormat(mDeviceContext, &pfd);
	if (indPixelFormat == 0) {
		throw UairException("No appropiate Pixel Format was found."); // an error has occured
	}
	
	// set the pixel format of the device context to the pixel format found previously (indPixelFormat)
	if (SetPixelFormat(mDeviceContext, indPixelFormat, &pfd) == false) {
		throw UairException("Unable to set Pixel Format."); // an error has occured
	}
	
	mOpen = true; // indicate window is now created and open
	
	{
		RECT windowSize;
		GetClientRect(mWindowHandle, &windowSize);
		mWinSize.x = windowSize.right - windowSize.left;
		mWinSize.y = windowSize.bottom - windowSize.top;
		
		POINT clientPos;
		clientPos.x = windowSize.left;
		clientPos.y = windowSize.top;
		ClientToScreen(mWindowHandle, &clientPos);
		mWinPos = glm::ivec2(clientPos.x, clientPos.y);
	}
	
	{ // set initial mouse cursor position
		POINT globalMouse;
		GetCursorPos(&globalMouse);
		
		POINT localMouse;
		localMouse.x = globalMouse.x;
		localMouse.y = globalMouse.y;
		ScreenToClient(mWindowHandle, &localMouse);
		
		{
			WindowEvent e;
			e.mType = WindowEvent::MouseMoveType;
			e.mMouseMove.mGlobalX = globalMouse.x; e.mMouseMove.mGlobalY = globalMouse.y;
			e.mMouseMove.mLocalX = localMouse.x; e.mMouseMove.mLocalY = localMouse.y;
			mEventQueue.push_back(e);
			
			storedGlobalMouse.x = globalMouse.x; storedGlobalMouse.y = globalMouse.y;
			storedLocalMouse.x = localMouse.x; storedLocalMouse.y = localMouse.y;
		}
	}
}

LRESULT CALLBACK WindowWin32::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (message == WM_NCCREATE) {
		LONG_PTR self = reinterpret_cast<LONG_PTR>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, self);
	}
	
	WindowWin32* win = reinterpret_cast<WindowWin32*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	if (win) {
		return win->HandleEvents(hWnd, message, wParam, lParam);
    }
	else {
		return DefWindowProcW(hWnd, message, wParam, lParam);
	}
}

LRESULT CALLBACK WindowWin32::HandleEvents(const HWND& hWnd, const UINT& message, const WPARAM& wParam, const LPARAM& lParam) {
	switch (message) {
		case WM_CREATE : {
			RAWINPUTDEVICE rid[2];
			rid[0] = {
				0x01,				// the game controls usage page
				0x04,				// joystick
				RIDEV_DEVNOTIFY,	// send messages when device is added or removed
				hWnd				// the handle to the window
			};
			
			rid[1] = {
				0x01,
				0x05,				// gamepad
				RIDEV_DEVNOTIFY,
				hWnd
			};
			
			RegisterRawInputDevices(rid, 2, sizeof(rid[0]));
			
			return 0;
		}
		case WM_CLOSE : {
			WindowEvent e;
			e.mType = WindowEvent::CloseType;
			mEventQueue.push_back(e);
			
			return 0;
		}
		case WM_SIZE : {
			WindowEvent e;
			e.mType = WindowEvent::SizeType;
			
			e.mSize.mType = wParam;
			e.mSize.mWidth = LOWORD(lParam);
			e.mSize.mHeight = HIWORD(lParam);
			mEventQueue.push_back(e);
			
			mWinDS.mWidth = e.mSize.mWidth; mWinDS.mHeight = e.mSize.mHeight;
			mWinSize.x = e.mSize.mWidth; mWinSize.y = e.mSize.mHeight;
			
			{
				RECT winRect;
				winRect.left = static_cast<long>(0); winRect.right = static_cast<long>(e.mSize.mWidth);
				winRect.top = static_cast<long>(0); winRect.bottom = static_cast<long>(e.mSize.mHeight);
				
				AdjustWindowRectEx(&winRect, GetWindowLong(mWindowHandle, GWL_STYLE), false,
						GetWindowLong(mWindowHandle, GWL_EXSTYLE));
				
				mActualSize.x = winRect.right - winRect.left;
				mActualSize.y = winRect.bottom - winRect.top;
			}
			
			if (wglGetCurrentContext() != 0) {
				GLint width = static_cast<GLint>(e.mSize.mWidth);
				GLint height = static_cast<GLint>(e.mSize.mHeight);
				if (height == 0) {
					height = 1;
				}
				
				glViewport(0, 0, width, height);
			}
			
			return 0;
		}
		case WM_MOVE : {
			WindowEvent e;
			e.mType = WindowEvent::MoveType;
			
			e.mMove.mX = LOWORD(lParam);
			e.mMove.mY = HIWORD(lParam);
			mEventQueue.push_back(e);
			
			mWinPos = glm::ivec2(e.mMove.mX, e.mMove.mY);
			
			return 0;
		}
		case WM_SETFOCUS : {
			// wglMakeCurrent(mDeviceContext, mRenderContext);
			
			WindowEvent e;
			e.mType = WindowEvent::GainedFocusType;
			mEventQueue.push_back(e);
			
			mHasFocus = true;
			
			return 0;
		}
		case WM_KILLFOCUS : {
			WindowEvent e;
			e.mType = WindowEvent::LostFocusType;
			mEventQueue.push_back(e);
			
			ReleaseCapture();
			mCaptureCount = 0;
			
			mHasFocus = false;
			
			return 0;
		}
		case WM_DISPLAYCHANGE : {
			WindowEvent e;
			e.mType = WindowEvent::DisplayChangeType;
			e.mDisplayChange.mHeight = HIWORD(lParam);
			e.mDisplayChange.mWidth = LOWORD(lParam);
			
			mEventQueue.push_back(e);
			
			return 0;
		}
		case WM_CAPTURECHANGED : {
			WindowEvent e;
			
			if (reinterpret_cast<HWND>(lParam) == mWindowHandle) {
				e.mType = WindowEvent::GainedCaptureType;
			}
			else {
				e.mType = WindowEvent::LostCaptureType;
			}
			
			mEventQueue.push_back(e);
			
			return 0;
		}
	}
	
	// input related events: keyboard
	switch (message) {
		case WM_KEYDOWN :
		case WM_SYSKEYDOWN : {
			Keyboard key = ToKeyboard(wParam, lParam);
			
			WindowEvent e;
			e.mType = WindowEvent::KeyboardKeyType;
			e.mKeyboardKey.mKey = key;
			e.mKeyboardKey.mType = 0;
			
			mEventQueue.push_back(e);
			
			return 0;
		}
		case WM_KEYUP :
		case WM_SYSKEYUP : {
			Keyboard key = ToKeyboard(wParam, lParam);
			
			WindowEvent e;
			e.mType = WindowEvent::KeyboardKeyType;
			e.mKeyboardKey.mKey = key;
			e.mKeyboardKey.mType = 1;
			
			mEventQueue.push_back(e);
			
			return 0;
		}
	}
	
	// input related events: mouse
	switch (message) {
		case WM_LBUTTONDOWN : { // left muse button down
			WindowEvent e;
			e.mType = WindowEvent::MouseButtonType;
			e.mMouseButton.mButton = Mouse::Left;
			e.mMouseButton.mType = 0;
			
			mEventQueue.push_back(e);
			
			SetCapture(mWindowHandle);
			++mCaptureCount;
			
			return 0;
		}
		case WM_LBUTTONUP : {
			WindowEvent e;
			e.mType = WindowEvent::MouseButtonType;
			e.mMouseButton.mButton = Mouse::Left;
			e.mMouseButton.mType = 1;
			
			mEventQueue.push_back(e);
			
			--mCaptureCount;
			if (mCaptureCount == 0u) {
				ReleaseCapture();
			}
			
			return 0;
		}
		case WM_LBUTTONDBLCLK : {
			WindowEvent e;
			e.mType = WindowEvent::MouseButtonType;
			e.mMouseButton.mButton = Mouse::Left;
			e.mMouseButton.mType = 2;
			
			mEventQueue.push_back(e);
			
			SetCapture(mWindowHandle);
			++mCaptureCount;
			
			return 0;
		}
		case WM_MBUTTONDOWN : {
			WindowEvent e;
			e.mType = WindowEvent::MouseButtonType;
			e.mMouseButton.mButton = Mouse::Middle;
			e.mMouseButton.mType = 0;
			
			mEventQueue.push_back(e);
			
			SetCapture(mWindowHandle);
			++mCaptureCount;
			
			return 0;
		}
		case WM_MBUTTONUP : {
			WindowEvent e;
			e.mType = WindowEvent::MouseButtonType;
			e.mMouseButton.mButton = Mouse::Middle;
			e.mMouseButton.mType = 1;
			
			mEventQueue.push_back(e);
			
			--mCaptureCount;
			if (mCaptureCount == 0u) {
				ReleaseCapture();
			}
			
			return 0;
		}
		case WM_MBUTTONDBLCLK : {
			WindowEvent e;
			e.mType = WindowEvent::MouseButtonType;
			e.mMouseButton.mButton = Mouse::Middle;
			e.mMouseButton.mType = 2;
			
			mEventQueue.push_back(e);
			
			SetCapture(mWindowHandle);
			++mCaptureCount;
			
			return 0;
		}
		case WM_RBUTTONDOWN : {
			WindowEvent e;
			e.mType = WindowEvent::MouseButtonType;
			e.mMouseButton.mButton = Mouse::Right;
			e.mMouseButton.mType = 0;
			
			mEventQueue.push_back(e);
			
			SetCapture(mWindowHandle);
			++mCaptureCount;
			
			return 0;
		}
		case WM_RBUTTONUP : {
			WindowEvent e;
			e.mType = WindowEvent::MouseButtonType;
			e.mMouseButton.mButton = Mouse::Right;
			e.mMouseButton.mType = 1;
			
			mEventQueue.push_back(e);
			
			--mCaptureCount;
			if (mCaptureCount == 0u) {
				ReleaseCapture();
			}
			
			return 0;
		}
		case WM_RBUTTONDBLCLK : {
			WindowEvent e;
			e.mType = WindowEvent::MouseButtonType;
			e.mMouseButton.mButton = Mouse::Right;
			e.mMouseButton.mType = 2;
			
			mEventQueue.push_back(e);
			
			SetCapture(mWindowHandle);
			++mCaptureCount;
			
			return 0;
		}
		case WM_XBUTTONDOWN : {
			WindowEvent e;
			e.mType = WindowEvent::MouseButtonType;
			
			if (HIWORD(wParam) == 1) {
				e.mMouseButton.mButton = Mouse::M4;
			}
			else {
				e.mMouseButton.mButton = Mouse::M5;
			}
			
			e.mMouseButton.mType = 0;
			
			mEventQueue.push_back(e);
			
			SetCapture(mWindowHandle);
			++mCaptureCount;
			
			return 0;
		}
		case WM_XBUTTONUP : {
			WindowEvent e;
			e.mType = WindowEvent::MouseButtonType;
			
			if (HIWORD(wParam) == 1) {
				e.mMouseButton.mButton = Mouse::M4;
			}
			else {
				e.mMouseButton.mButton = Mouse::M5;
			}
			
			e.mMouseButton.mType = 1;
			
			mEventQueue.push_back(e);
			
			--mCaptureCount;
			if (mCaptureCount == 0u) {
				ReleaseCapture();
			}
			
			return 0;
		}
		case WM_XBUTTONDBLCLK : {
			WindowEvent e;
			e.mType = WindowEvent::MouseButtonType;
			
			if (HIWORD(wParam) == 1) {
				e.mMouseButton.mButton = Mouse::M4;
			}
			else {
				e.mMouseButton.mButton = Mouse::M5;
			}
			
			e.mMouseButton.mType = 2;
			
			mEventQueue.push_back(e);
			
			SetCapture(mWindowHandle);
			++mCaptureCount;
			
			return 0;
		}
		case WM_MOUSEWHEEL : {
			WindowEvent e;
			e.mType = WindowEvent::MouseWheelType;
			e.mMouseWheel.mAmount = GET_WHEEL_DELTA_WPARAM(wParam);
			
			mEventQueue.push_back(e);
			
			return 0;
		}
	}
	
	// input related events: device
	switch (message) {
		case WM_INPUT : { // input recieve from a connected input device
			if (GET_RAWINPUT_CODE_WPARAM(wParam) == RIM_INPUT) {
				UINT ridBufferSize; // size of the rawinput data buffer
				
				// get the required rawinput data buffer size...
				if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &ridBufferSize, sizeof(RAWINPUTHEADER)) != 0) {
					return DefWindowProcW(hWnd, message, wParam, lParam);
				}
				
				std::unique_ptr<BYTE[]> ridBuffer(new BYTE[ridBufferSize]); // create an appropiately sized array to hold the data
				
				// load the data into the rawinput data buffer and check the size matches the required size returned by the first call...
				if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, ridBuffer.get(), &ridBufferSize, sizeof(RAWINPUTHEADER)) != ridBufferSize) {
					// size mismatch
					return DefWindowProcW(hWnd, message, wParam, lParam);
				}
				
				PRAWINPUT rawInput = (PRAWINPUT)ridBuffer.get(); // convert buffer to RAWINPUT pointer
				if (rawInput->header.dwType == RIM_TYPEHID) { // if input device is not a keyboard or mouse...
					auto inputDevice = mInputDevices.find(rawInput->header.hDevice);
					if (inputDevice == mInputDevices.end()) { // if the device hasn't been registered yet...
						if (!RegisterInputDevice(rawInput->header.hDevice)) { // attempt to register the device...
							// error: unable to register device
							return DefWindowProcW(hWnd, message, wParam, lParam);
						}
						
						inputDevice = mInputDevices.find(rawInput->header.hDevice);
						if (inputDevice == mInputDevices.end()) {
							// error: unable to register device
							return DefWindowProcW(hWnd, message, wParam, lParam);
						}
					}
					
					UINT ppdBufferSize;
					if (GetRawInputDeviceInfo(rawInput->header.hDevice, RIDI_PREPARSEDDATA, NULL, &ppdBufferSize) != 0) {
						// error: 
						return DefWindowProcW(hWnd, message, wParam, lParam);
					}
					
					std::unique_ptr<BYTE[]> ppdBuffer(new BYTE[ppdBufferSize]);
					if (GetRawInputDeviceInfo(rawInput->header.hDevice, RIDI_PREPARSEDDATA, ppdBuffer.get(), &ppdBufferSize) != ppdBufferSize) {
						// error: size mismatch
						return DefWindowProcW(hWnd, message, wParam, lParam);
					}
					
					PHIDP_PREPARSED_DATA preparsedData = (PHIDP_PREPARSED_DATA)ppdBuffer.get();
					HIDP_CAPS caps;
					
					PHIDP_BUTTON_CAPS buttonCaps;
					std::unique_ptr<BYTE[]> buttonCapsBuffer;
					
					PHIDP_VALUE_CAPS valueCaps;
					std::unique_ptr<BYTE[]> valueCapsBuffer;
					
					if (!GetDeviceCapabilities(preparsedData, caps, buttonCaps, buttonCapsBuffer, valueCaps, valueCapsBuffer)) {
						// error: unable to get device capabilities
						return DefWindowProcW(hWnd, message, wParam, lParam);
					}
					
					{ // buttons
						ULONG usageLength = buttonCaps->Range.UsageMax - buttonCaps->Range.UsageMin + 1; // ...
						USAGE usageList[usageLength]; // ...
						
						// ...
						if (HidP_GetUsages(HidP_Input, buttonCaps->UsagePage, 0, usageList, &usageLength, preparsedData,
								(PCHAR)rawInput->data.hid.bRawData, rawInput->data.hid.dwSizeHid) != HIDP_STATUS_SUCCESS) {
							
							// error: unable to get current button values
							return DefWindowProcW(hWnd, message, wParam, lParam);
						}
						
						std::vector<bool> tempButtonStates(inputDevice->second.mButtonCount, false);
						for (unsigned int i = 0u; i < usageLength && i < inputDevice->second.mButtonCount; ++i) {
							tempButtonStates.at(usageList[i] - buttonCaps->Range.UsageMin) = true;
						}
						
						for (unsigned int i = 0u; i < inputDevice->second.mButtonCount; ++i) {
							if (inputDevice->second.mButtonStates.at(i) != tempButtonStates.at(i)) {
								// dispatch an event indicating a button state has changed
								WindowEvent e;
								e.mType = WindowEvent::DeviceButtonType;
								e.mDeviceButton.mButton = i;
								e.mDeviceButton.mID = inputDevice->second.mID;
								
								if (tempButtonStates.at(i)) {
									e.mDeviceButton.mType = 0;
								}
								else {
									e.mDeviceButton.mType = 1;
								}
								
								mEventQueue.push_back(e);
							}
						}
						
						inputDevice->second.mButtonStates = std::move(tempButtonStates);
					}
					
					{ // controls
						for (unsigned int i = 0; i < caps.NumberInputValueCaps; ++i) {
							ULONG value;
							
							if (HidP_GetUsageValue(HidP_Input, valueCaps[i].UsagePage, 0, valueCaps[i].Range.UsageMin, &value, preparsedData,
									(PCHAR)rawInput->data.hid.bRawData, rawInput->data.hid.dwSizeHid) != HIDP_STATUS_SUCCESS) {
								
								// various possible errors
								return DefWindowProcW(hWnd, message, wParam, lParam);
							}
							
							int intValue = static_cast<int>(std::min(value, static_cast<ULONG>(std::numeric_limits<int>::max())));
							Device control = ToDevice(valueCaps[i].Range.UsageMin);
							
							auto oldValue = inputDevice->second.mControlStates.find(control);
							
							if (oldValue != inputDevice->second.mControlStates.end() && oldValue->second != intValue) { // if the value exists and differs from the previous...
								oldValue->second = intValue; // update the previous...
								
								// dispatch an event indicating a control value has changed
								WindowEvent e;
								e.mType = WindowEvent::DeviceControlType;
								e.mDeviceControl.mControl = control;
								e.mDeviceControl.mID = inputDevice->second.mID;
								e.mDeviceControl.mValue = intValue;
								mEventQueue.push_back(e);
							}
						}
					}
				}
			}
			
			return DefWindowProcW(hWnd, message, wParam, lParam);
		}
		case WM_INPUT_DEVICE_CHANGE : { // input device was connected or disconnected
			if (wParam == GIDC_ARRIVAL) { // if a device was connected to the system...
				if (!RegisterInputDevice((HANDLE)lParam)) { // attempt to register the device...
					// error: unable to register device
				}
			}
			else { // otherwise if a device was removed from the system...
				auto result = mInputDevices.find((HANDLE)lParam); // find the device's handle in the device store
				if (result != mInputDevices.end()) { // if the device exists in the store...
					WindowEvent e; // create a window event
					e.mType = WindowEvent::DeviceChangedType; // set the event type
					e.mDeviceChanged.mID = result->second.mID; // set the id to the device's id in the store
					e.mDeviceChanged.mStatus = false; // set the connection status to false (disconnected)
					
					mInputDevices.erase(result); // remove the device from the store
					mEventQueue.push_back(e); // add the window event to the event queue
				}
			}
			
			return 0;
		}
	}
	
	return DefWindowProcW(hWnd, message, wParam, lParam);
}

Keyboard WindowWin32::ToKeyboard(const WPARAM & code, const LPARAM & flags) const {
	switch (code) {
		case VK_SHIFT : {
			static UINT leftShift = MapVirtualKey(VK_LSHIFT, MAPVK_VK_TO_VSC);
			UINT scan = (flags & (0xFF << 16)) >> 16;
			if (scan == leftShift) {
				return Keyboard::LShift;
			}
			else {
				return Keyboard::RShift;
			}
		}
		case VK_MENU :
			return (HIWORD(flags) & KF_EXTENDED) ? Keyboard::RAlt : Keyboard::LAlt;
		case VK_CONTROL :
			return (HIWORD(flags) & KF_EXTENDED) ? Keyboard::RControl : Keyboard::LControl;
		case VK_LWIN :
			return Keyboard::LSystem;
		case VK_RWIN :
			return Keyboard::RSystem;
		case VK_APPS :
			return Keyboard::Menu;
		case VK_OEM_1 :
			return Keyboard::SemiColon;
		case VK_OEM_2 :
			return Keyboard::Slash;
		case VK_OEM_PLUS :
			return Keyboard::Equal;
		case VK_OEM_MINUS :
			return Keyboard::Hyphen;
		case VK_OEM_4 :
			return Keyboard::LBracket;
		case VK_OEM_6 :
			return Keyboard::RBracket;
		case VK_OEM_COMMA :
			return Keyboard::Comma;
		case VK_OEM_PERIOD :
			return Keyboard::FullStop;
		case VK_OEM_7 :
			return Keyboard::Quote;
		case VK_OEM_5 :
			return Keyboard::BackSlash;
		case VK_OEM_3 :
			return Keyboard::Tilde;
		case VK_ESCAPE :
			return Keyboard::Escape;
		case VK_SPACE :
			return Keyboard::Space;
		case VK_RETURN :
			return Keyboard::Return;
		case VK_BACK :
			return Keyboard::Back;
		case VK_TAB :
			return Keyboard::Tab;
		case VK_PRIOR :
			return Keyboard::PageUp;
		case VK_NEXT :
			return Keyboard::PageDown;
		case VK_END :
			return Keyboard::End;
		case VK_HOME :
			return Keyboard::Home;
		case VK_INSERT :
			return Keyboard::Insert;
		case VK_DELETE :
			return Keyboard::Delete;
		case VK_ADD :
			return Keyboard::Add;
		case VK_SUBTRACT :
			return Keyboard::Subtract;
		case VK_MULTIPLY :
			return Keyboard::Multiply;
		case VK_DIVIDE :
			return Keyboard::Divide;
		case VK_PAUSE :
			return Keyboard::Break;
		case VK_F1 :
			return Keyboard::F1;
		case VK_F2 :
			return Keyboard::F2;
		case VK_F3 :
			return Keyboard::F3;
		case VK_F4 :
			return Keyboard::F4;
		case VK_F5 :
			return Keyboard::F5;
		case VK_F6 :
			return Keyboard::F6;
		case VK_F7 :
			return Keyboard::F7;
		case VK_F8 :
			return Keyboard::F8;
		case VK_F9 :
			return Keyboard::F9;
		case VK_F10 :
			return Keyboard::F10;
		case VK_F11 :
			return Keyboard::F11;
		case VK_F12 :
			return Keyboard::F12;
		case VK_F13 :
			return Keyboard::F13;
		case VK_F14 :
			return Keyboard::F14;
		case VK_F15 :
			return Keyboard::F15;
		case VK_LEFT :
			return Keyboard::Left;
		case VK_RIGHT :
			return Keyboard::Right;
		case VK_UP :
			return Keyboard::Up;
		case VK_DOWN :
			return Keyboard::Down;
		case VK_NUMPAD0 :
			return Keyboard::Numpad0;
		case VK_NUMPAD1 :
			return Keyboard::Numpad1;
		case VK_NUMPAD2 :
			return Keyboard::Numpad2;
		case VK_NUMPAD3 :
			return Keyboard::Numpad3;
		case VK_NUMPAD4 :
			return Keyboard::Numpad4;
		case VK_NUMPAD5 :
			return Keyboard::Numpad5;
		case VK_NUMPAD6 :
			return Keyboard::Numpad6;
		case VK_NUMPAD7 :
			return Keyboard::Numpad7;
		case VK_NUMPAD8 :
			return Keyboard::Numpad8;
		case VK_NUMPAD9 :
			return Keyboard::Numpad9;
		case 'Q' :
			return Keyboard::Q;
		case 'W' :
			return Keyboard::W;
		case 'E' :
			return Keyboard::E;
		case 'R' :
			return Keyboard::R;
		case 'T' :
			return Keyboard::T;
		case 'Y' :
			return Keyboard::Y;
		case 'U' :
			return Keyboard::U;
		case 'I' :
			return Keyboard::I;
		case 'O' :
			return Keyboard::O;
		case 'P' :
			return Keyboard::P;
		case 'A' :
			return Keyboard::A;
		case 'S' :
			return Keyboard::S;
		case 'D' :
			return Keyboard::D;
		case 'F' :
			return Keyboard::F;
		case 'G' :
			return Keyboard::G;
		case 'H' :
			return Keyboard::H;
		case 'J' :
			return Keyboard::J;
		case 'K' :
			return Keyboard::K;
		case 'L' :
			return Keyboard::L;
		case 'Z' :
			return Keyboard::Z;
		case 'X' :
			return Keyboard::X;
		case 'C' :
			return Keyboard::C;
		case 'V' :
			return Keyboard::V;
		case 'B' :
			return Keyboard::B;
		case 'N' :
			return Keyboard::N;
		case 'M' :
			return Keyboard::M;
		case '0' :
			return Keyboard::Num0;
		case '1' :
			return Keyboard::Num1;
		case '2' :
			return Keyboard::Num2;
		case '3' :
			return Keyboard::Num3;
		case '4' :
			return Keyboard::Num4;
		case '5' :
			return Keyboard::Num5;
		case '6' :
			return Keyboard::Num6;
		case '7' :
			return Keyboard::Num7;
		case '8' :
			return Keyboard::Num8;
		case '9' :
			return Keyboard::Num9;
	}
	
	return Keyboard::Unknown;
}

Device WindowWin32::ToDevice(const unsigned int& valueID) const {
	switch (valueID) {
		case 0x30 :
			return Device::AxisX;
		case 0x31 :
			return Device::AxisY;
		case 0x32 :
			return Device::AxisZ;
		case 0x33 :
			return Device::AxisRx;
		case 0x34 :
			return Device::AxisRy;
		case 0x35 :
			return Device::AxisRz;
		case 0x36 :
			return Device::Slider;
		case 0x37 :
			return Device::Dial;
		case 0x38 :
			return Device::Wheel;
		case 0x39 :
			return Device::HatSwitch;
	}
	
	return Device::Unknown;
}

bool WindowWin32::RegisterInputDevice(HANDLE deviceHandle) {
	UINT ridBufferSize; // size of the rawinput data buffer
	GetRawInputDeviceInfo(deviceHandle, RIDI_DEVICEINFO, NULL, &ridBufferSize); // get and store the required buffer size
	std::unique_ptr<BYTE[]> ridBuffer(new BYTE[ridBufferSize]); // create an appropiately sized array to hold the data
	if (GetRawInputDeviceInfo(deviceHandle, RIDI_DEVICEINFO, ridBuffer.get(), &ridBufferSize) != ridBufferSize) { // get the rawinput data and check for errors...
		// size mismatch
		return false;
	}
	
	PRID_DEVICE_INFO deviceInfo = (PRID_DEVICE_INFO)ridBuffer.get(); // cast the rawinput data to a pointer to a device info structure
	
	if (deviceInfo->dwType == RIM_TYPEHID) { // if the device connected wasn't a mouse or keyboard...
		WindowEvent e; // create a window event
		e.mType = WindowEvent::DeviceChangedType; // set the event type
		e.mDeviceChanged.mID = mInputDeviceIDCounter; // set the id to the device's id in the store
		e.mDeviceChanged.mStatus = true; // set the connection status to true (connected)
		
		UINT ppdBufferSize;
		GetRawInputDeviceInfo(deviceHandle, RIDI_PREPARSEDDATA, NULL, &ppdBufferSize);
		std::unique_ptr<BYTE[]> ppdBuffer(new BYTE[ppdBufferSize]);
		if (GetRawInputDeviceInfo(deviceHandle, RIDI_PREPARSEDDATA, ppdBuffer.get(), &ppdBufferSize) != ppdBufferSize) {
			// error
			return false;
		}
		
		PHIDP_PREPARSED_DATA preparsedData = (PHIDP_PREPARSED_DATA)ppdBuffer.get();
		HIDP_CAPS caps;
		
		PHIDP_BUTTON_CAPS buttonCaps;
		std::unique_ptr<BYTE[]> buttonCapsBuffer;
		
		PHIDP_VALUE_CAPS valueCaps;
		std::unique_ptr<BYTE[]> valueCapsBuffer;
		
		if (!GetDeviceCapabilities(preparsedData, caps, buttonCaps, buttonCapsBuffer, valueCaps, valueCapsBuffer)) {
			// error: unable to get device capabilities
			return false;
		}
		
		InputDevice inputDevice; // create the input device
		
		inputDevice.mButtonCount += (buttonCaps->Range.UsageMax - buttonCaps->Range.UsageMin) + 1;
		inputDevice.mButtonStates.insert(inputDevice.mButtonStates.end(), inputDevice.mButtonCount, false);
		
		inputDevice.mControlCount = caps.NumberInputValueCaps;
		for (unsigned int i = 0; i < caps.NumberInputValueCaps; ++i) {
			Device control = ToDevice(valueCaps[i].Range.UsageMin);
			if (control != Device::Unknown) {
				// cast the range to an unsigned value
				ULONG uLogicalMin = static_cast<ULONG>(std::max(0l, valueCaps[i].LogicalMin));
				ULONG uLogicalMax = static_cast<ULONG>(std::max(0l, valueCaps[i].LogicalMax));
				
				// cast the range to an int
				int intLowerRange = static_cast<int>(std::min(uLogicalMin, static_cast<ULONG>(std::numeric_limits<int>::max())));
				int intUpperRange = static_cast<int>(std::min(uLogicalMax, static_cast<ULONG>(std::numeric_limits<int>::max())));
				
				// fix the range if it is invalid
				if (intUpperRange <= intLowerRange) {
					intLowerRange = 0;
					intUpperRange = 65535;
				}
				
				// create a control capability structure
				InputManager::InputDeviceCaps::ControlCaps deviceControlCaps;
				deviceControlCaps.mDevice = control;
				deviceControlCaps.mLowerRange = intLowerRange;
				deviceControlCaps.mUpperRange = intUpperRange;
				deviceControlCaps.mCollectionID = valueCaps[i].LinkCollection;
				
				inputDevice.mControls.push_back(std::move(deviceControlCaps));
				inputDevice.mControlStates.emplace(control, 0);
			}
			else {
				--inputDevice.mControlCount; // decrement total control count to account for unknown control
			}
		}
		
		e.mDeviceChanged.mButtonCount = inputDevice.mButtonCount; // set the reported number of buttons present
		e.mDeviceChanged.mControlCount = inputDevice.mControlCount; // set the reported number of controls present
		for (unsigned int i = 0u; i < inputDevice.mControlCount && i < DEVICECOUNT; ++i) { // for all controls present...
			e.mDeviceChanged.mCaps.mControls.at(i) = inputDevice.mControls.at(i); // set the control capabilities
		}
		
		inputDevice.mID = mInputDeviceIDCounter++; // set the new device's id and then increment the id counter
		mInputDevices.emplace(deviceHandle, std::move(inputDevice)); // add the new device to the store
		mEventQueue.push_back(e); // add the window event to the event queue
	}
	
	return true;
}

bool WindowWin32::GetDeviceCapabilities(const PHIDP_PREPARSED_DATA& preparsedData, HIDP_CAPS& caps, PHIDP_BUTTON_CAPS& buttonCaps,
		std::unique_ptr<BYTE[]>& buttonCapsBuffer, PHIDP_VALUE_CAPS& valueCaps, std::unique_ptr<BYTE[]>& valueCapsBuffer) {
	
	if (HidP_GetCaps(preparsedData, &caps) != HIDP_STATUS_SUCCESS) { // attempt to get input device caps...
		return false; // unable to get input device caps
	}
	
	std::unique_ptr<BYTE[]> buttonCapsBufferTemp(new BYTE[sizeof(HIDP_BUTTON_CAPS) * caps.NumberInputButtonCaps]); // temporary buffer for button cap data
	buttonCapsBuffer = std::move(buttonCapsBufferTemp); // move temporary buffer into actual buffer
	buttonCaps = (PHIDP_BUTTON_CAPS)buttonCapsBuffer.get(); // convert buffer to pointer to button cap structure
	if (HidP_GetButtonCaps(HidP_Input, buttonCaps, &caps.NumberInputButtonCaps, preparsedData) != HIDP_STATUS_SUCCESS) { // attempt to get input device button caps...
		return false; // unable to get input device button caps
	}
	
	std::unique_ptr<BYTE[]> valueCapsBufferTemp(new BYTE[sizeof(HIDP_VALUE_CAPS) * caps.NumberInputValueCaps]);
	valueCapsBuffer = std::move(valueCapsBufferTemp);
	valueCaps = (PHIDP_VALUE_CAPS)valueCapsBuffer.get();
	if (HidP_GetValueCaps(HidP_Input, valueCaps, &caps.NumberInputValueCaps, preparsedData) != HIDP_STATUS_SUCCESS) {
		return false;
	}
	
	return true; // successfully retrieved input device capabilities
}

std::pair<glm::ivec2, glm::ivec2> WindowWin32::SetMouseCoords(const glm::ivec2& newCoords, const CoordinateSpace& coordinateSpace) {
	POINT localMouse;
	localMouse.x = newCoords.x;
	localMouse.y = newCoords.y;
	
	POINT globalMouse;
	globalMouse.x = newCoords.x;
	globalMouse.y = newCoords.y;
	
	if (coordinateSpace == CoordinateSpace::Local) {
		ClientToScreen(mWindowHandle, &globalMouse);
	}
	else {
		ScreenToClient(mWindowHandle, &localMouse);
	}
	
	SetCursorPos(globalMouse.x, globalMouse.y);
	
	if ((globalMouse.x != storedGlobalMouse.x || globalMouse.y != storedGlobalMouse.y) ||
			(localMouse.x != storedLocalMouse.x || localMouse.y != storedLocalMouse.y)) {
		
		storedGlobalMouse.x = globalMouse.x; storedGlobalMouse.y = globalMouse.y;
		storedLocalMouse.x = localMouse.x; storedLocalMouse.y = localMouse.y;
	}
	
	return std::make_pair(storedLocalMouse, storedGlobalMouse);
}
}
