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

#include "renderbuffer.hpp"

#include <iostream>

#include "openglstates.hpp"

namespace uair {
RenderBuffer::RenderBuffer(RenderBuffer&& other) : RenderBuffer() {
	swap(*this, other);
}

RenderBuffer::~RenderBuffer() {
	Clear();
}

RenderBuffer& RenderBuffer::operator=(RenderBuffer other) {
	swap(*this, other);
	
	return *this;
}

void swap(RenderBuffer& first, RenderBuffer& second) {
	std::swap(first.mRenderBufferID, second.mRenderBufferID);
	
	std::swap(first.mWidth, second.mWidth);
	std::swap(first.mHeight, second.mHeight);
}

bool RenderBuffer::CreateRenderBuffer(const GLenum& internalformat, const unsigned int& width, const unsigned int& height) {
	EnsureInitialised(); // ensure this render buffer has been set up properly
	mWidth = width; // store the width
	mHeight = height; // store the height
	
	glBindRenderbuffer(GL_RENDERBUFFER, mRenderBufferID); // bind this render buffer as the current
	OpenGLStates::mCurrentRenderBuffer = mRenderBufferID; // set the current render buffer as this
	glRenderbufferStorage(GL_RENDERBUFFER, internalformat, mWidth, mHeight); // create this render buffer with the supplied data
	
	return true;
}

void RenderBuffer::EnsureInitialised() {
	if (mRenderBufferID == 0) { // if this render buffer doesn't have an id assigned...
		glGenRenderbuffers(1, &mRenderBufferID); // create this render buffer and store assigned id
	}
}

void RenderBuffer::Clear() {
	if (mRenderBufferID != 0) { // if this render buffer has an id assigned...
		if (OpenGLStates::mCurrentRenderBuffer == mRenderBufferID) { // if this render buffer is currently bound...
			glBindRenderbuffer(GL_RENDERBUFFER, 0); // unbind this render buffer
			OpenGLStates::mCurrentRenderBuffer = 0; // set the currently bound render buffer to none
		}
		
		glDeleteRenderbuffers(1, &mRenderBufferID); // delete this render buffer
	}
	
	mRenderBufferID = 0; // reset the assigned id
	mWidth = 0u; // reset the width
	mHeight = 0u; // reset the height
}

unsigned int RenderBuffer::GetRenderBufferID() const {
	return mRenderBufferID; // return the assigned id
}

unsigned int RenderBuffer::GetWidth() const {
	return mWidth; // return the width
}

unsigned int RenderBuffer::GetHeight() const {
	return mHeight; // return the height
}

unsigned int RenderBuffer::GetTypeID() {
	return static_cast<unsigned int>(Resources::RenderBuffer);
}
}
