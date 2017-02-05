/* **************************************************************** **
**	Uair Engine
**	Copyright (c) 2010 - 2017, Iain M. Crawford
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

#ifndef UAIRINIT_HPP
#define UAIRINIT_HPP

#if defined(_WIN32) || defined(__WIN32__)
	#define UAIRWIN32
	
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	
	#ifndef NOMINMAX
		#define NOMINMAX
	#endif
	
	#define UNICODE
	
	#include <windows.h>
	#include <windowsx.h>
	
	extern "C" {
		#include <hidsdi.h>
	}
	
	#include "GL/glew.h"
	extern GLEWContext* glewGetContext();
	extern GLEWContext* CURRENTCONTEXT;
	
	#include "GL/wglew.h"
	extern WGLEWContext* wglewGetContext();
	extern WGLEWContext* CURRENTCONTEXTWIN32;
	
	#ifdef UAIRDYNAMIC // if we're compiling a dynamic library...
		#ifdef UAIREXPORTDLL
			#define EXPORTDLL __declspec(dllexport)
		#else
			#define EXPORTDLL __declspec(dllimport)
		#endif
	#else // otherwise we're compiling a static library...
		#define EXPORTDLL
	#endif
#elif defined(linux) || defined(__linux)
	#define UAIRLINUX
	
	#include "GL/glew.h"
	#include "GL/glxew.h"
	
	#define EXPORTDLL
#else
	#error Unsupported OS
#endif

#endif
