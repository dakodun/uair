/* **************************************************************** **
**	Uair Engine
**	Copyright (c) 2010 - 2016, Iain M. Crawford
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

#ifndef UAIROPENGLSTATES_HPP
#define UAIROPENGLSTATES_HPP

#include <glm/gtc/matrix_transform.hpp>

#include "init.hpp"

namespace uair {
class OpenGLStates {
	public :
		// bind and store currently bound target
		static bool BindArrayBuffer(const GLuint& id);
		static bool BindElementArrayBuffer(const GLuint& id);
		static bool BindVertexArray(const GLuint& id);
		static bool UseProgram(const GLuint& id);
		static bool BindTexture(const GLuint& id);
		static bool BindFBO(const GLuint& id);
		static bool BindRenderBuffer(const GLuint& id);
		static bool BindPBOPack(const GLuint& id);
		static bool BindPBOUnpack(const GLuint& id);
		
		// matrices for rendering
		static glm::mat4 mProjectionMatrix;
		static glm::mat4 mViewMatrix;
		static glm::mat4 mModelMatrix;
		
		// currently bound target
		static GLuint mCurrentArrayBuffer;
		static GLuint mCurrentElementArrayBuffer;
		static GLuint mCurrentVertexArray;
		static GLuint mCurrentProgram;
		static GLuint mCurrentTexture;
		static GLuint mCurrentFBO;
		static GLuint mCurrentRenderBuffer;
		static GLuint mCurrentPBOPack;
		static GLuint mCurrentPBOUnpack;
};
}

#endif
