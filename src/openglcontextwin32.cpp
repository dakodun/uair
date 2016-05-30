/* **************************************************************** **
**	Uair Engine
**	Copyright (c) 2010 - 2016, Iain M. Crawford
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

#include "openglcontextwin32.hpp"

#include <iostream>
#include <vector>
#include <string>

#include "window.hpp"
#include "exception.hpp"

namespace uair {
OpenGLContextWin32::OpenGLContextWin32() {
	try {
		SetUpContext();
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		throw;
	}
}

OpenGLContextWin32::OpenGLContextWin32(Window& window) {
	try {
		SetUpContext(window);
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		throw;
	}
}

OpenGLContextWin32::OpenGLContextWin32(OpenGLContextWin32&& other) : OpenGLContextWin32() {
	swap(*this, other);
}

OpenGLContextWin32::~OpenGLContextWin32() {
	if (mRenderContext) {
		if (wglGetCurrentContext() == mRenderContext) {
			wglMakeCurrent(0, 0); // make sure it is not current
		}
		
		wglDeleteContext(mRenderContext);
		
		if (CURRENTCONTEXT == &mGlewContext) {
			CURRENTCONTEXT = nullptr;
			mGlewContext = GLEWContext();
		}
		
		if (CURRENTCONTEXTWIN32 == &mGlewContextWin32) {
			CURRENTCONTEXTWIN32 = nullptr;
			mGlewContextWin32 = WGLEWContext();
		}
	}
}

OpenGLContextWin32& OpenGLContextWin32::operator=(OpenGLContextWin32 other) {
	swap(*this, other);
	
	return *this;
}

void swap(OpenGLContextWin32& first, OpenGLContextWin32& second) {
	std::swap(first.mRenderContext, second.mRenderContext);
	std::swap(first.mGlewContext, second.mGlewContext);
	std::swap(first.mGlewContextWin32, second.mGlewContextWin32);
	// std::swap(first.mWindowPtr, second.mWindowPtr);
}

void OpenGLContextWin32::MakeCurrent(Window& window) {
	wglMakeCurrent(window.mDeviceContext, mRenderContext);
	CURRENTCONTEXT = &mGlewContext;
	CURRENTCONTEXTWIN32 = &mGlewContextWin32;
}

void OpenGLContextWin32::SetUpContext() {
	Window dummyWindow("dummyWindow", WindowDisplaySettings()); // create a temporary window
	SetUpContext(dummyWindow); // use temporary window to create context
}

void OpenGLContextWin32::SetUpContext(Window& window) {
	bool supported = false;
	HDC storedDeviceContext = wglGetCurrentDC(); // store the current dc
	HGLRC storedRenderContext = wglGetCurrentContext(); // store the current rc
	GLEWContext* storedGlewContext = CURRENTCONTEXT; // store the current glew context
	WGLEWContext* storedGlewContextWin32 = CURRENTCONTEXTWIN32; // store the current win32 glew context
	
	HGLRC dummyRenderContext = wglCreateContext(window.mDeviceContext); // create a temporary render context
	GLEWContext dummyGlewContext; // create a temporary glew context
	WGLEWContext dummyGlewContextWin32; // create a temporary wglew context
	
	if (!dummyRenderContext) { // if we failed to create a valid render context...
		throw std::runtime_error("Error creating dummy rendering context."); // throw exception
	}
	
	InitGlew(window.mDeviceContext, dummyRenderContext, dummyGlewContext, dummyGlewContextWin32); // initialise glew for the temporary context
	
	if (WGLEW_ARB_create_context) { // if we are able to create a 4.3 style rc...
		supported = true; // indicate 4.3 rc is available
	}
	else { // otherwise 4.3 style rc is not supported...
		supported = false; // indicate 4.3 rc is not available
	}
	
	if (supported == true) { // if we are able to create a 4.3 style rc...
		int attr[] = { // set attributes of rc for use with a 4.3 style rc
			WGL_CONTEXT_MAJOR_VERSION_ARB, 4, // opengl major version number
			WGL_CONTEXT_MINOR_VERSION_ARB, 3, // opengl minor version number
			WGL_CONTEXT_PROFILE_MASK_ARB, 2, // compatibility profile
			0
		};
		
		mRenderContext = wglCreateContextAttribsARB(window.mDeviceContext, 0, attr); // create 4.3 style rc
	}
	else { // otherwise 4.3 style rc is not supported...
		mRenderContext = wglCreateContext(window.mDeviceContext); // default to 2.1 style rc
	}
	
	if (!mRenderContext) { // if we failed to create a valid render context...
		wglMakeCurrent(storedDeviceContext, storedRenderContext); // restore the previous dc/rc combo
		CURRENTCONTEXT = storedGlewContext; // restore the previous glew context
		CURRENTCONTEXTWIN32 = storedGlewContextWin32; // restore the previous wglew context
		throw std::runtime_error("Error creating rendering context."); // throw exception
	}
	
	InitGlew(window.mDeviceContext, mRenderContext, mGlewContext, mGlewContextWin32); // initialise glew for the actual context
	
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CW);
		glCullFace(GL_BACK);
		
		glActiveTexture(GL_TEXTURE0);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	
	wglMakeCurrent(storedDeviceContext, storedRenderContext); // restore the previous dc/rc combo
	CURRENTCONTEXT = storedGlewContext; // restore the previous glew context
	CURRENTCONTEXTWIN32 = storedGlewContextWin32; // restore the previous wglew context
	
	wglDeleteContext(dummyRenderContext); // remove the temporary context
}

void OpenGLContextWin32::InitGlew(HDC& deviceContext, HGLRC& renderContext, GLEWContext& glewContext, WGLEWContext& glewContextWin32) {
	wglMakeCurrent(deviceContext, renderContext); // set the passed dc and rc as current
	CURRENTCONTEXT = &glewContext; // set the passed glew context as current
	CURRENTCONTEXTWIN32 = &glewContextWin32; // set the passed win32 glew context as current
	
	GLenum err = glewContextInit(&glewContext); // initialise the current glew context
	if (err != GLEW_OK) { // if initialisation wasn't successful...
		std::string glewError = reinterpret_cast<const char*>(glewGetErrorString(err)); // get the error string
		throw std::runtime_error(glewError); // throw an exception
	}
	
	err = wglewContextInit(&glewContextWin32); // initialise the current win32 glew context
	if (err != GLEW_OK) { // if initialisation wasn't successful...
		std::string glewError = reinterpret_cast<const char*>(glewGetErrorString(err)); // get the error string
		throw std::runtime_error(glewError); // throw an exception
	}
}

/* void OpenGLContextWin32::Create(HDC& deviceContext, const WindowDisplaySettings& winDS) {
	HWND dummyWindowHandle;
	{ // create a dummy window
		std::string dummyTitle = "Dummy";
		DWORD windowsStyle = WS_DISABLED;
		DWORD windowsStyleEx = 0;
		
		RECT winRect;
		winRect.left = static_cast<long>(0); winRect.right = static_cast<long>(1);
		winRect.top = static_cast<long>(0); winRect.bottom = static_cast<long>(1);
		
		DWORD dwStringSize = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, dummyTitle.c_str(), -1, 0, 0);
		std::vector<wchar_t> wsBuffer(dwStringSize);
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, dummyTitle.c_str(), static_cast<int>(dummyTitle.size()), &wsBuffer[0], dwStringSize);
		
		WNDCLASSEXW winClassExW;
		winClassExW.cbSize		  = sizeof(WNDCLASSEXW);
		winClassExW.style         = CS_OWNDC | CS_DBLCLKS;
		winClassExW.lpfnWndProc   = DefWindowProcW;
		winClassExW.cbClsExtra    = 0;
		winClassExW.cbWndExtra    = 0;
		winClassExW.hInstance     = GetModuleHandle(0);
		winClassExW.hIcon         = 0;
		winClassExW.hCursor       = ::LoadCursor(0, IDC_ARROW);
		winClassExW.hbrBackground = 0;
		winClassExW.lpszMenuName  = 0;
		winClassExW.lpszClassName = L"UairDummyWindow";
		winClassExW.hIconSm		  = 0;
		
		RegisterClassExW(&winClassExW);
		dummyWindowHandle = CreateWindowExW(windowsStyleEx, 
			L"UairDummyWindow", &wsBuffer[0], windowsStyle,
			0, 0, winRect.right - winRect.left, winRect.bottom - winRect.top,
			0, 0, GetModuleHandle(0), this);
		
		if (!dummyWindowHandle) {
			UnregisterClassW(L"UairDummyWindow", GetModuleHandle(0));
			
			throw UairException("Unable to create dummy window for proper context creation.");
		}
	}
	
	HDC dummyDeviceContext = GetDC(dummyWindowHandle);
	{
		// set the PFD for the DC
		PIXELFORMATDESCRIPTOR pfd = {
			sizeof(PIXELFORMATDESCRIPTOR), 								// The size of the PFD data structure
			1,															// The version number of the pfd
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, // Bit flags specifying properties of the PFD
			PFD_TYPE_RGBA,												// The type of pixel data
			static_cast<unsigned char>(winDS.mColourDepth),				// The colour-depth of the frame buffer
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
		int dummyIndPixelFormat = ChoosePixelFormat(dummyDeviceContext, &pfd);
		if (dummyIndPixelFormat == 0) {
			throw UairException("No appropiate Pixel Format was found."); // an error eccoured
		}
		
		// set the pixel format of the dummy device context to the pixel format found previously (dummyIndPixelFormat)
		if (SetPixelFormat(dummyDeviceContext, dummyIndPixelFormat, &pfd) == false) {
			throw UairException("Unable to set Pixel Format."); // an error eccoured
		}
	}
	
	// create a dummy RC and set it as the current RC
	HGLRC dummyRenderContext = wglCreateContext(dummyDeviceContext);
	wglMakeCurrent(dummyDeviceContext, dummyRenderContext);
	
	if (!dummyRenderContext) { // 
		throw UairException("Error creating dummy rendering context."); // throw exception if there was a problem
	}
	
	bool supported = false;
	
	{
		// initialise GLEW
		GLenum err = glewInit();
		if (err != GLEW_OK) { // if initialisation wasn't successful
			std::string glewError = reinterpret_cast<const char*>(glewGetErrorString(err));
			throw UairException(glewError);
		}
		
		if (WGLEW_ARB_create_context) { // if we are able to create a 4.3 style RC
			supported = true;
		}
		else { // otherwise 4.3 style RC is not supported
			supported = false;
		}
	}
	
	{ // create our actual render context using our dummy context
		if (deviceContext) {
			// set the PFD for the DC
			PIXELFORMATDESCRIPTOR pfd = {
				sizeof(PIXELFORMATDESCRIPTOR), 								// The size of the PFD data structure
				1,															// The version number of the pfd
				PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, // Bit flags specifying properties of the PFD
				PFD_TYPE_RGBA,												// The type of pixel data
				static_cast<unsigned char>(winDS.mColourDepth),				// The colour-depth of the frame buffer
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
			int indPixelFormat = ChoosePixelFormat(deviceContext, &pfd);
			if (indPixelFormat == 0) {
				throw UairException("No appropiate Pixel Format was found."); // an error has occured
			}
			
			// set the pixel format of the device context to the pixel format found previously (indPixelFormat)
			if (SetPixelFormat(deviceContext, indPixelFormat, &pfd) == false) {
				throw UairException("Unable to set Pixel Format."); // an error has occured
			}
			
			if (supported == true) { // if we are able to create a 4.3 style RC
				int attr[] = { // set attributes of RC for use with a 4.3 style RC
					WGL_CONTEXT_MAJOR_VERSION_ARB, 4, // OpenGL major version number
					WGL_CONTEXT_MINOR_VERSION_ARB, 3, // OpenGL minor version number
					WGL_CONTEXT_PROFILE_MASK_ARB, 2, // Compatibility profile
					0
				};
				
				mRenderContext = wglCreateContextAttribsARB(deviceContext, 0, attr); // create 4.3 style RC
			}
			else { // otherwise 4.3 style RC is not supported
				mRenderContext = wglCreateContext(deviceContext); // default to 2.1 style RC
			}
			
			wglMakeCurrent(0, 0); // set current RC to 0
			
			if (!mRenderContext) { // if we failed to create a valid render context
				throw UairException("Error creating rendering context."); // throw exception if there was a problem
			}
			
			wglMakeCurrent(deviceContext, mRenderContext); // set our render context as current
			
			// initialise GLEW
			GLenum err = glewInit();
			if (err != GLEW_OK) { // if initialisation wasn't successful
				std::string glewError = reinterpret_cast<const char*>(glewGetErrorString(err));
				throw UairException(glewError);
			}
		}
		else {
			throw UairException("Unable to link contexts.");
		}
	}
	
	if (dummyWindowHandle) { // if we have a valid dummy window handle
		if (dummyDeviceContext) {  // if we have a valid dummy device context
			ReleaseDC(dummyWindowHandle, dummyDeviceContext); // release our dummy device context
		}
		
		DestroyWindow(dummyWindowHandle); // destroy our dummy window
		UnregisterClassW(L"UairDummyWindow", GetModuleHandle(0)); // unregister dummy window class
	}
	
	if (dummyRenderContext) { // if we have a valid dummy window handle
		wglDeleteContext(dummyRenderContext); // delete our dummy RC
	}
}

void OpenGLContextWin32::SetUp(const glm::uvec2& winSize) {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);
	
	glActiveTexture(GL_TEXTURE0);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glClearDepth(1.0f);
	
	GLint width = static_cast<GLint>(winSize.x);
	GLint height = static_cast<GLint>(winSize.y);
	if (height == 0) {
		height = 1;
	}
	
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
} */
}
