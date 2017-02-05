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

#ifndef UAIRFBO_HPP
#define UAIRFBO_HPP

#include <vector>

#include "init.hpp"
#include "texture.hpp"
#include "renderbuffer.hpp"

namespace uair {
class EXPORTDLL FBO {
	public :
		FBO() = default;
		FBO(const FBO& other) = delete;
		FBO(FBO&& other);
		~FBO();
		
		FBO& operator=(FBO other);
		
		friend void swap(FBO& first, FBO& second);
		
		// attach a texture via a pointer to the frame buffer
		bool AttachTexture(ResourcePtr<Texture> texture, const GLenum& attachmentPoint, const GLint& textureLayer, const GLint& mipmapLevel = 0);
		bool AttachTexture(Texture* texture, const GLenum& attachmentPoint, const GLint& textureLayer, const GLint& mipmapLevel = 0);
		
		// attach a render buffer via a pointer to the frame buffer
		bool AttachRenderBuffer(ResourcePtr<RenderBuffer> renderBuffer, const GLenum& attachmentPoint);
		bool AttachRenderBuffer(RenderBuffer* renderBuffer, const GLenum& attachmentPoint);
		
		// map the fragment shader output to specified buffers
		void MapBuffers(const std::vector<GLuint>& attachmentPoints);
		
		// ensure the fbo is properly initialised before use
		void EnsureInitialised();
		
		// clear the resources used by the fbo
		void Clear();
		
		// return the id assigned to the fbo
		unsigned int GetFBOID() const; 
	private :
		GLuint mFBOID = 0; // the id assigned to the fbo
};
}

#endif
