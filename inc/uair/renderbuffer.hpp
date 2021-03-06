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

#ifndef UAIRRENDERBUFFER_HPP
#define UAIRRENDERBUFFER_HPP

#include "init.hpp"
#include "resource.hpp"

namespace uair {
class EXPORTDLL RenderBuffer : public Resource {
	public :
		RenderBuffer() = default;
		RenderBuffer(const RenderBuffer& other) = delete;
		RenderBuffer(RenderBuffer&& other);
		~RenderBuffer();
		
		RenderBuffer& operator=(RenderBuffer other);
		
		friend void swap(RenderBuffer& first, RenderBuffer& second);
		
		bool CreateRenderBuffer(const GLenum& internalformat, const unsigned int& width, const unsigned int& height);
		
		void EnsureInitialised();
		void Clear();
		
		unsigned int GetRenderBufferID() const;
		unsigned int GetWidth() const;
		unsigned int GetHeight() const;
		
		static unsigned int GetTypeID();
	private :
		GLuint mRenderBufferID = 0;
		
		unsigned int mWidth;
		unsigned int mHeight;
};
}

#endif
