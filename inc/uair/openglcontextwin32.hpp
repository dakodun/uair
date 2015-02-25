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

#ifndef UAIROPENGLCONTEXTWIN32_HPP
#define UAIROPENGLCONTEXTWIN32_HPP

namespace uair {
class OpenGLContextWin32 {
	friend class Game;
	
	public :
		OpenGLContextWin32() = default;
		OpenGLContextWin32(const OpenGLContextWin32& other) = delete;
		OpenGLContextWin32(OpenGLContextWin32&& other);
		~OpenGLContextWin32();
		
		OpenGLContextWin32& operator=(OpenGLContextWin32 other);
		
		friend void swap(OpenGLContextWin32& first, OpenGLContextWin32& second);
		
		void CreateContext();
		void SetUpOpenGL();
	private :
		
};
}

#endif
