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

#ifndef UAIROPENGLCONTEXT_HPP
#define UAIROPENGLCONTEXT_HPP

#include <memory>
#include "init.hpp"

#if defined(UAIRWIN32)
	#include "openglcontextwin32.hpp"
	typedef uair::OpenGLContextWin32 OpenGLContextOS;
#elif defined(UAIRLINUX)
	#include "openglcontextlinux.hpp"
	typedef uair::OpenGLContextLinux OpenGLContextOS;
#endif

namespace uair {
class EXPORTDLL OpenGLContext : public OpenGLContextOS {
	friend class WindowOS;
	
	public :
		OpenGLContext();
		OpenGLContext(Window& window);
		~OpenGLContext();
};

typedef std::shared_ptr<OpenGLContext> OpenGLContextPtr;
}

#endif
