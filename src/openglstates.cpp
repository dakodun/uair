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
* \file		openglstates.cpp
* \brief	
**/

#include "openglstates.hpp"

namespace uair {
GLint OpenGLStates::mVertexLocation = -1;
GLint OpenGLStates::mNormalLocation = -1;
GLint OpenGLStates::mColourLocation = -1;
GLint OpenGLStates::mTexCoordLocation = -1;
GLint OpenGLStates::mTexLocation = -1;
GLint OpenGLStates::mTexExistsLocation = -1;

GLint OpenGLStates::mProjectionMatrixLocation = -1;
GLint OpenGLStates::mViewMatrixLocation = -1;
GLint OpenGLStates::mModelMatrixLocation = -1;

glm::mat4 OpenGLStates::mProjectionMatrix = glm::mat4(1.0f);
glm::mat4 OpenGLStates::mViewMatrix = glm::mat4(1.0f);
glm::mat4 OpenGLStates::mModelMatrix = glm::mat4(1.0f);

GLuint OpenGLStates::mCurrentArrayBuffer = 0;
GLuint OpenGLStates::mCurrentElementArrayBuffer = 0;
GLuint OpenGLStates::mCurrentProgram = 0;
GLuint OpenGLStates::mCurrentTexture = 0;
}
