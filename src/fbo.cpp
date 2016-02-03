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

#include "fbo.hpp"

#include <iostream>

#include "openglstates.hpp"
#include "util.hpp"

namespace uair {
FBO::FBO(FBO&& other) : FBO() {
	swap(*this, other);
}

FBO::~FBO() {
	Clear();
}

FBO& FBO::operator=(FBO other) {
	swap(*this, other);
	
	return *this;
}

void swap(FBO& first, FBO& second) {
	std::swap(first.mFBOID, second.mFBOID);
}

bool FBO::AttachTexture(ResourcePtr<Texture> texture, const GLenum& attachmentPoint, const GLint& textureLayer, const GLint& mipmapLevel) {
	return AttachTexture(texture.GetResource(), attachmentPoint, textureLayer, mipmapLevel); // call member function with raw pointer
}

bool FBO::AttachTexture(Texture* texture, const GLenum& attachmentPoint, const GLint& textureLayer, const GLint& mipmapLevel) {
	if (!texture) { // if pointer isn't valid...
		util::LogMessage(1, "invalid texture pointer"); // log an error message
		return false; // return failure
	}
	
	EnsureInitialised(); // ensure this fbo has been set up properly
	OpenGLStates::BindFBO(mFBOID); // bind the fbo as current
	
	//glFramebufferTexture3D(GL_FRAMEBUFFER, attachmentPoint, GL_TEXTURE_2D_ARRAY, texture->GetTextureID(), mipmapLevel, textureLayer); // attach the texture to the specified point
	glFramebufferTextureLayer(GL_FRAMEBUFFER, attachmentPoint, texture->GetTextureID(), mipmapLevel, textureLayer); // attach the texture to the specified point
	
	GLenum err = glCheckFramebufferStatus(GL_FRAMEBUFFER); // check status of the fbo
	if (err != GL_FRAMEBUFFER_COMPLETE) { // if an error occured...
		util::LogMessage(1, util::ToString(err)); // log the error message
		return false; // return failure
	}
	
	return true; // return success
}

bool FBO::AttachRenderBuffer(ResourcePtr<RenderBuffer> renderBuffer, const GLenum& attachmentPoint) {
	return AttachRenderBuffer(renderBuffer.GetResource(), attachmentPoint); // call member function with raw pointer
}

bool FBO::AttachRenderBuffer(RenderBuffer* renderBuffer, const GLenum& attachmentPoint) {
	if (!renderBuffer) { // if pointer isn't valid...
		util::LogMessage(1, "invalid render buffer pointer"); // log an error message
		return false; // return failure
	}
	
	EnsureInitialised(); // ensure this fbo has been set up properly
	OpenGLStates::BindFBO(mFBOID); // bind the fbo as current
	
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachmentPoint, GL_RENDERBUFFER, renderBuffer->GetRenderBufferID()); // attach the render buffer to the specified point
	
	GLenum err = glCheckFramebufferStatus(GL_FRAMEBUFFER); // check status of the fbo
	if (err != GL_FRAMEBUFFER_COMPLETE) { // if an error occured...
		util::LogMessage(1, util::ToString(err)); // log the error message
		return false; // return failure
	}
	
	return true; // return success
}

void FBO::MapBuffers(const std::vector<GLuint>& attachmentPoints) {
	glDrawBuffers(attachmentPoints.size(), attachmentPoints.data()); // assign the buffers for fragment shader output
}

void FBO::EnsureInitialised() {
	if (mFBOID == 0) { // if no id has been assigned...
		glGenFramebuffers(1, &mFBOID); // create this fbo and store assigned id
	}
}

void FBO::Clear() {
	if (mFBOID != 0) { // if there is an id assigned...
		if (OpenGLStates::mCurrentFBO == mFBOID) { // if this fbo is the currently bound fbo...
			glBindFramebuffer(GL_FRAMEBUFFER, 0); // unbind this fbo
			OpenGLStates::mCurrentFBO = 0; // set the currently bound fbo to 'none'
		}
		
		glDeleteFramebuffers(1, &mFBOID); // delete this fbo
		mFBOID = 0; // reset the assigned id
	}
}

unsigned int FBO::GetFBOID() const {
	return mFBOID; // return the assigned id
}
}
