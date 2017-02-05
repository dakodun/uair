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

#include "openglstates.hpp"

namespace uair {
bool OpenGLStates::BindArrayBuffer(const GLuint& id) {
	if (OpenGLStates::mCurrentArrayBuffer != id) {
		glBindBuffer(GL_ARRAY_BUFFER, id);
		OpenGLStates::mCurrentArrayBuffer = id;
		
		return true;
	}
	
	return false;
}

bool OpenGLStates::BindElementArrayBuffer(const GLuint& id) {
	if (OpenGLStates::mCurrentElementArrayBuffer != id) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
		OpenGLStates::mCurrentElementArrayBuffer = id;
		
		return true;
	}
	
	return false;
}

bool OpenGLStates::BindVertexArray(const GLuint& id) {
	if (OpenGLStates::mCurrentVertexArray != id) {
		glBindVertexArray(id);
		OpenGLStates::mCurrentVertexArray = id;
		
		GLint elementBinding;
		glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &elementBinding);
		OpenGLStates::mCurrentElementArrayBuffer = elementBinding;
		
		return true;
	}
	
	return false;
}

bool OpenGLStates::UseProgram(const GLuint& id) {
	if (OpenGLStates::mCurrentProgram != id) {
		glUseProgram(id);
		OpenGLStates::mCurrentProgram = id;
		
		return true;
	}
	
	return false;
}

bool OpenGLStates::BindTexture(const GLuint& id) {
	if (OpenGLStates::mCurrentTexture != id) {
		glBindTexture(GL_TEXTURE_2D_ARRAY, id);
		OpenGLStates::mCurrentTexture = id;
		
		return true;
	}
	
	return false;
}

bool OpenGLStates::BindFBO(const GLuint& id) {
	if (OpenGLStates::mCurrentFBO != id) {
		glBindFramebuffer(GL_FRAMEBUFFER, id);
		OpenGLStates::mCurrentFBO = id;
		
		return true;
	}
	
	return false;
}

bool OpenGLStates::BindRenderBuffer(const GLuint& id) {
	if (OpenGLStates::mCurrentRenderBuffer != id) {
		glBindRenderbuffer(GL_RENDERBUFFER, id);
		OpenGLStates::mCurrentRenderBuffer = id;
		
		return true;
	}
	
	return false;
}

bool OpenGLStates::BindPBOPack(const GLuint& id) {
	if (OpenGLStates::mCurrentPBOPack != id) {
		glBindBuffer(GL_PIXEL_PACK_BUFFER, id);
		OpenGLStates::mCurrentPBOPack = id;
		
		return true;
	}
	
	return false;
}

bool OpenGLStates::BindPBOUnpack(const GLuint& id) {
	if (OpenGLStates::mCurrentPBOUnpack != id) {
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, id);
		OpenGLStates::mCurrentPBOUnpack = id;
		
		return true;
	}
	
	return false;
}

glm::mat4 OpenGLStates::mProjectionMatrix = glm::mat4(1.0f);
glm::mat4 OpenGLStates::mViewMatrix = glm::mat4(1.0f);
glm::mat4 OpenGLStates::mModelMatrix = glm::mat4(1.0f);

GLuint OpenGLStates::mCurrentArrayBuffer = 0;
GLuint OpenGLStates::mCurrentElementArrayBuffer = 0;
GLuint OpenGLStates::mCurrentVertexArray = 0;
GLuint OpenGLStates::mCurrentProgram = 0;
GLuint OpenGLStates::mCurrentTexture = 0;
GLuint OpenGLStates::mCurrentFBO = 0;
GLuint OpenGLStates::mCurrentRenderBuffer = 0;
GLuint OpenGLStates::mCurrentPBOPack = 0;
GLuint OpenGLStates::mCurrentPBOUnpack = 0;
}
