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
* \file		windowstyles.hpp
* \brief	Enumerators relating to various window styles (WindowStyle).
**/

#ifndef UAIRWINDOWSTYLES_HPP
#define UAIRWINDOWSTYLES_HPP

namespace uair {
namespace WindowStyles {
	enum {
		Visible = 1, // WS_VISIBLE
		FullScreen = 2, // WS_POPUP
		Titlebar = 4, // WS_CAPTION
		Close = 8, // WS_SYSMENU
		Minimise = 16, // WS_MINIMIZEBOX
		Maximise = 32, // WS_MAXIMIZEBOX
		Resize = 64 // WS_SIZEBOX
	};
}

namespace WindowStylesEx {
	enum {
		
	};
}
}

#endif
