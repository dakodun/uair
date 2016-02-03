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

#include "pbo.hpp"

#include <iostream>
#include <algorithm>

#include "openglstates.hpp"

namespace uair {
PBO::PBO(PBO&& other) : PBO() {
	swap(*this, other);
}

PBO::~PBO() {
	Clear();
}

PBO& PBO::operator=(PBO other) {
	swap(*this, other);
	
	return *this;
}

void swap(PBO& first, PBO& second) {
	std::swap(first.mPBOID, second.mPBOID);
}

void PBO::BufferData(const GLenum& target, const GLsizeiptr& size, const GLenum& usage) {
	EnsureInitialised(); // ensure this pbo has been set up properly
	
	if (target == GL_PIXEL_PACK_BUFFER) { // if we're using the pack buffer...
		OpenGLStates::BindPBOPack(mPBOID); // bind the pbo to the pack buffer
	}
	else {
		OpenGLStates::BindPBOUnpack(mPBOID); // otherwise bind the pbo to the unpack buffer
	}
	
	glBufferData(target, size, 0, usage); // assign memory to the pbo
}

void PBO::MapBuffer(const GLuint& attachmentPoint) {
	glReadBuffer(attachmentPoint); // assign the buffer to read from
}

void PBO::EnsureInitialised() {
	if (mPBOID == 0) { // if no id has been assigned...
		glGenBuffers(1, &mPBOID); // create this pbo and store assigned id
	}
}

void PBO::Clear() {
	if (mPBOID != 0) { // if there is an id assigned...
		if (OpenGLStates::mCurrentPBOPack == mPBOID) { // if this pbo is the currently bound pack pbo...
			OpenGLStates::BindPBOPack(0);
		}
		
		if (OpenGLStates::mCurrentPBOUnpack == mPBOID) { // if this pbo is the currently bound unpack pbo...
			OpenGLStates::BindPBOUnpack(0);
		}
		
		glDeleteBuffers(1, &mPBOID); // delete this pbo
		mPBOID = 0; // reset the assigned id
	}
}

unsigned int PBO::GetPBOID() const {
	return mPBOID; // return the assigned id
}
}
