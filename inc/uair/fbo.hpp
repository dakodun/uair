/* **************************************************************** **
**	Uair Engine
**	Copyright (c) 2014 Iain M. Crawford
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

#ifndef UAIRFBO_HPP
#define UAIRFBO_HPP

#include <vector>

#include "init.hpp"
#include "texture.hpp"
#include "renderbuffer.hpp"

namespace uair {
class FBO {
	public :
		FBO() = default;
		FBO(const FBO& other) = delete;
		FBO(FBO&& other);
		~FBO();
		
		FBO& operator=(FBO other);
		
		friend void swap(FBO& first, FBO& second);
		
		void AddTexture(const std::vector<GLenum>& attachments, const unsigned int& width, const unsigned int& height); // add a texture to the fbo and attach to specified points (a layer each)
		void AddRenderBuffer(const GLenum& attachment, const GLenum& internalFormat, const unsigned int& width, const unsigned int& height); // add a render buffer to the fbo and attach to specified point
		
		void EnsureInitialised(); // ensure the fbo is properly initialised before use
		void Clear(); // clear the resources used by the fbo
		
		unsigned int GetFBOID() const; // return the id assigned to the fbo
	private :
		GLuint mFBOID = 0; // the id assigned to the fbo
		
		std::vector<Texture> mTextures; // the textures attached to the fbo
		std::vector<RenderBuffer> mRenderBuffers; // the render buffers attach to the fbo
};
}

#endif
