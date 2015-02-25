/* **************************************************************** **
**	Uair Engine
**	Copyright (c) 2013 Iain M. Crawford
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

#include <vector>
#include <string>

#include "init.hpp"
#include "exception.hpp"

namespace uair {
OpenGLContextWin32::OpenGLContextWin32(OpenGLContextWin32&& other) : OpenGLContextWin32() {
	swap(*this, other);
}

OpenGLContextWin32::~OpenGLContextWin32() {
	
}

OpenGLContextWin32& OpenGLContextWin32::operator=(OpenGLContextWin32 other) {
	swap(*this, other);
	
	return *this;
}

void swap(OpenGLContextWin32& first, OpenGLContextWin32& second) {
	
}

void OpenGLContextWin32::CreateContext() {
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
			32,															// The colour-depth of the frame buffer
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
	
	// mWinDS         FROM WINDOW
	// mDeviceContext FROM WINDOW
	// mRenderContext FROM THIS
	// 
	/* { // create our actual render context using our dummy context
		if (mDeviceContext) {
			// set the PFD for the DC
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
			
			if (supported == true) { // if we are able to create a 4.3 style RC
				int attr[] = { // set attributes of RC for use with a 4.3 style RC
					WGL_CONTEXT_MAJOR_VERSION_ARB, 4, // OpenGL major version number
					WGL_CONTEXT_MINOR_VERSION_ARB, 3, // OpenGL minor version number
					WGL_CONTEXT_PROFILE_MASK_ARB, 2, // Compatibility profile
					0
				};
				
				mRenderContext = wglCreateContextAttribsARB(mDeviceContext, 0, attr); // create 4.3 style RC
			}
			else { // otherwise 4.3 style RC is not supported
				mRenderContext = wglCreateContext(mDeviceContext); // default to 2.1 style RC
			}
			
			wglMakeCurrent(0, 0); // set current RC to 0
			
			if (!mRenderContext) { // if we failed to create a valid render context
				throw UairException("Error creating rendering context."); // throw exception if there was a problem
			}
			
			wglMakeCurrent(mDeviceContext, mRenderContext); // set our render context as current
			
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
	} */
	
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

void OpenGLContextWin32::SetUpOpenGL() {
	try {
		CreateContext();
	} catch (UairException& e) {
		throw;
	}
	
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
	}
	
	// mWindowHandle
	// mWinPos
	// mWinSize
	
	/* RECT windowSize;
    GetClientRect(mWindowHandle, &windowSize);
    mWinSize.x = windowSize.right - windowSize.left;
    mWinSize.y = windowSize.bottom - windowSize.top;
	
	POINT clientPos;
	clientPos.x = windowSize.left;
	clientPos.y = windowSize.top;
	ClientToScreen(mWindowHandle, &clientPos);
	mWinPos = glm::ivec2(clientPos.x, clientPos.y);
	
	GLint width = static_cast<GLint>(mWinSize.x);
	GLint height = static_cast<GLint>(mWinSize.y);
	if (height == 0) {
		height = 1;
	}
	
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); */
}
}
