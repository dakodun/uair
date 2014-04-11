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

/** 
* \file		windowwin32.hpp
* \brief	Defines the WindowWin32 class which may be (depending on OS) inherited by the Window class (via WindowOS typedef).
**/

#ifndef UAIRWINDOWWIN32_HPP
#define UAIRWINDOWWIN32_HPP

// Missing winuser defines
#define MAPVK_VK_TO_VSC 0
#define VK_OEM_PLUS 0xBB // '+' any country
#define VK_OEM_COMMA 0xBC // ',' any country
#define VK_OEM_MINUS 0xBD // '-' any country
#define VK_OEM_PERIOD 0xBE // '.' any country

#include <string>
#include <set>
#include <deque>

#include <glm/glm.hpp>

#include "init.hpp"
#include "windowdisplaysettings.hpp"
#include "windowevent.hpp"
#include "windowstyles.hpp"

namespace uair {
class WindowWin32 {
	public :
		WindowWin32();
		WindowWin32(const std::string & windowTitle, const WindowDisplaySettings & settings,
				const unsigned long & windowStyle = WindowStyles::Visible | WindowStyles::Titlebar | WindowStyles::Close);
		virtual ~WindowWin32();
		
		WindowWin32(const WindowWin32& copyFrom) = delete;
		WindowWin32& operator=(const WindowWin32& other) = delete;
		
		void Process();
		bool Display() const;
		
		bool IsOpen() const;
		
		WindowDisplaySettings GetCurrentDS() const;
		std::set<WindowDisplaySettings> GetSupportedDS() const;
		
		unsigned int GetWidth() const;
		unsigned int GetHeight() const;
		
		unsigned int GetActualWidth() const;
		unsigned int GetActualHeight() const;
	private :
		friend class Game;
		
		void SetUpWindow();
		
		void SetUpOpenGL();
		void CreateContext();
		
		void HandleEvents(const HWND & hWnd, const UINT & message, const WPARAM & wParam, const LPARAM & lParam);
		
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		
		Keyboard ToKeyboard(const WPARAM & code, const LPARAM & flags) const;
	private :
		HWND mWindowHandle = 0;
		HDC mDeviceContext = 0;
		HGLRC mRenderContext = 0;
		
		std::deque<WindowEvent> mEventQueue;
		
		bool mOpen = false;
		std::string mWinTitle;
		WindowDisplaySettings mWinDS;
		glm::uvec2 mWinSize;
		glm::uvec2 mActualSize;
		glm::ivec2 mWinPos;
		unsigned long mWinStyle;
		
		bool mHasFocus;
		unsigned int mCaptureCount;
		
		glm::ivec2 storedGlobalMouse;
		glm::ivec2 storedLocalMouse;
};
}

#endif
