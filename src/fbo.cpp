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
	
	std::swap(first.mTextures, second.mTextures);
	std::swap(first.mRenderBuffers, second.mRenderBuffers);
}

void FBO::AddTexture(const std::vector<GLenum>& attachments, const unsigned int& width, const unsigned int& height) {
	glGetError();
	EnsureInitialised(); // ensure this fbo has been set up properly
	
	if (OpenGLStates::mCurrentFBO != mFBOID) {
		glBindFramebuffer(GL_FRAMEBUFFER, mFBOID);
		OpenGLStates::mCurrentFBO = mFBOID;
	}
	
	mTextures.emplace_back(); // add a new texture to the store
	
	for (unsigned int i = 0; i < attachments.size(); ++i) { // for all requested attachment points...
		mTextures.back().AddFromMemory({}, width, height); // add a new empty layer to the texture
	}
	
	mTextures.back().CreateTexture(); // set up the new texture
	
	for (unsigned int i = 0; i < attachments.size(); ++i) { // for all requested attachment points...
		glFramebufferTexture3D(GL_FRAMEBUFFER, attachments.at(i), GL_TEXTURE_2D_ARRAY, mTextures.back().GetTextureID(), 0, i); // attach the layer of the new texture to the specified point
	}
	
	GLenum err = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (err != GL_FRAMEBUFFER_COMPLETE) {
		std::clog << err << '\n';
	}
}

void FBO::AddRenderBuffer(const GLenum& attachment, const GLenum& internalFormat, const unsigned int& width, const unsigned int& height) {
	EnsureInitialised(); // ensure this fbo has been set up properly
	if (OpenGLStates::mCurrentFBO != mFBOID) {
		glBindFramebuffer(GL_FRAMEBUFFER, mFBOID);
		OpenGLStates::mCurrentFBO = mFBOID;
	}
	
	mRenderBuffers.emplace_back(); // add a new render buffer to the store
	mRenderBuffers.back().CreateRenderBuffer(internalFormat, width, height); // set up the new render buffer using supplied data
	
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, mRenderBuffers.back().GetRenderBufferID()); // attach the new render buffer to the specified point
	
	GLenum err = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (err != GL_FRAMEBUFFER_COMPLETE) {
		std::clog << err << '\n';
	}
}

std::vector< ResourcePtr<Texture> > FBO::GetTextures() {
	std::vector< ResourcePtr<Texture> > textures;
	
	for (auto texture = mTextures.begin(); texture != mTextures.end(); ++texture) {
		textures.emplace_back(&(*texture));
	}
	
	return textures;
}

std::vector< ResourcePtr<RenderBuffer> > FBO::GetRenderBuffers() {
	std::vector< ResourcePtr<RenderBuffer> > renderBuffers;
	
	for (auto renderBuffer = mRenderBuffers.begin(); renderBuffer!= mRenderBuffers.end(); ++renderBuffer) {
		renderBuffers.emplace_back(&(*renderBuffer));
	}
	
	return renderBuffers;
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
		
		mTextures.clear(); // remove attached textures
		mRenderBuffers.clear(); // remove attached render buffers
		
		glDeleteFramebuffers(1, &mFBOID); // delete this fbo
		mFBOID = 0; // reset the assigned id
	}
}

unsigned int FBO::GetFBOID() const {
	return mFBOID; // return the assigned id
}
}
