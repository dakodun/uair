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

/** 
* \file		vbo.cpp
* \brief	
**/

#include "vbo.hpp"

#include <iostream>

#include "openglstates.hpp"
#include "renderbatch.hpp"

namespace uair {
VBO::~VBO() {
	if (mVertVBOID != 0) {
		if (OpenGLStates::mCurrentArrayBuffer == mVertVBOID) { // if our 
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			OpenGLStates::mCurrentArrayBuffer = 0;
		}
		
		glDeleteBuffers(1, &mVertVBOID); // destroy our 
		mVertVBOID = 0;
	}
	
	if (mIndVBOID != 0) {
		if (OpenGLStates::mCurrentElementArrayBuffer == mIndVBOID) { // if our 
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			OpenGLStates::mCurrentElementArrayBuffer = 0;
		}
		
		glDeleteBuffers(1, &mIndVBOID); // destroy our 
		mIndVBOID = 0;
	}
}

void VBO::AddData(const std::vector<VBOVertex> & vertData, const std::vector<VBOIndex> & indData) {
	EnsureInitialised();
	
	if (OpenGLStates::mCurrentArrayBuffer != mVertVBOID) {
		glBindBuffer(GL_ARRAY_BUFFER, mVertVBOID);
		OpenGLStates::mCurrentArrayBuffer = mVertVBOID;
	}
	
	std::size_t vertSize = std::max((mMinimumSize * 1024 * 1024), (vertData.size() * sizeof(uair::VBOVertex)));
	glBufferData(GL_ARRAY_BUFFER, vertSize, &vertData[0], mType);
	
	if (OpenGLStates::mCurrentElementArrayBuffer != mIndVBOID) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndVBOID);
		OpenGLStates::mCurrentElementArrayBuffer = mIndVBOID;
	}
	
	std::size_t indSize = std::max((mMinimumSize * 1024 * 1024), (indData.size() * sizeof(uair::VBOIndex)));
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indSize, &indData[0], mType);
	
	mIndicesSize = indData.size();
	
	mSegmentInfo.clear();
	mSegmentInfo.emplace_back(0, 0, mIndicesSize - 1);
}

void VBO::AddData(const std::vector<VBOVertex> & vertData, const std::vector<VBOIndex> & indData,
		const std::vector<SegmentInfo> & segmentInfo) {
	
	EnsureInitialised();
	
	if (OpenGLStates::mCurrentArrayBuffer != mVertVBOID) {
		glBindBuffer(GL_ARRAY_BUFFER, mVertVBOID);
		OpenGLStates::mCurrentArrayBuffer = mVertVBOID;
	}
	
	std::size_t vertSize = std::max((mMinimumSize * 1024 * 1024), (vertData.size() * sizeof(uair::VBOVertex)));
	glBufferData(GL_ARRAY_BUFFER, vertSize, &vertData[0], mType);
	
	if (OpenGLStates::mCurrentElementArrayBuffer != mIndVBOID) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndVBOID);
		OpenGLStates::mCurrentElementArrayBuffer = mIndVBOID;
	}
	
	std::size_t indSize = std::max((mMinimumSize * 1024 * 1024), (indData.size() * sizeof(uair::VBOIndex)));
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indSize, &indData[0], mType);
	
	mIndicesSize = indData.size();
	
	mSegmentInfo.clear();
	mSegmentInfo.insert(mSegmentInfo.end(), segmentInfo.begin(), segmentInfo.end());
}

void VBO::Draw() {
	if (OpenGLStates::mCurrentArrayBuffer != mVertVBOID) {
		glBindBuffer(GL_ARRAY_BUFFER, mVertVBOID);
	 	OpenGLStates::mCurrentArrayBuffer = mVertVBOID;
	}
	
	if (OpenGLStates::mCurrentElementArrayBuffer != mIndVBOID) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndVBOID);
	 	OpenGLStates::mCurrentElementArrayBuffer = mIndVBOID;
	}
	
	glUniformMatrix4fv(uair::OpenGLStates::mViewMatrixLocation, 1, GL_FALSE, &uair::OpenGLStates::mViewMatrix[0][0]);
	glUniformMatrix4fv(uair::OpenGLStates::mModelMatrixLocation, 1, GL_FALSE, &uair::OpenGLStates::mModelMatrix[0][0]);
	glUniformMatrix4fv(uair::OpenGLStates::mProjectionMatrixLocation, 1, GL_FALSE, &uair::OpenGLStates::mProjectionMatrix[0][0]);
	
	glEnableVertexAttribArray(uair::OpenGLStates::mVertexLocation);
	glEnableVertexAttribArray(uair::OpenGLStates::mNormalLocation);
	glEnableVertexAttribArray(uair::OpenGLStates::mColourLocation);
	glEnableVertexAttribArray(uair::OpenGLStates::mTexCoordLocation);
	glEnableVertexAttribArray(uair::OpenGLStates::mTexExistsLocation);
	
	glVertexAttribPointer(uair::OpenGLStates::mVertexLocation, 3, GL_FLOAT, GL_TRUE, sizeof(uair::VBOVertex), (void*)(offsetof(uair::VBOVertex, mX)));
	glVertexAttribPointer(uair::OpenGLStates::mNormalLocation, 3, GL_FLOAT, GL_TRUE, sizeof(uair::VBOVertex), (void*)(offsetof(uair::VBOVertex, mNX)));
	glVertexAttribPointer(uair::OpenGLStates::mColourLocation, 4, GL_FLOAT, GL_TRUE, sizeof(uair::VBOVertex), (void*)(offsetof(uair::VBOVertex, mR)));
	glVertexAttribPointer(uair::OpenGLStates::mTexCoordLocation, 3, GL_FLOAT, GL_TRUE, sizeof(uair::VBOVertex), (void*)(offsetof(uair::VBOVertex, mS)));
	glVertexAttribPointer(uair::OpenGLStates::mTexExistsLocation, 1, GL_FLOAT, GL_TRUE, sizeof(uair::VBOVertex), (void*)(offsetof(uair::VBOVertex, mTex)));
	
	for (unsigned int i = 0u; i < mSegmentInfo.size(); ++i) {
		if (OpenGLStates::mCurrentTexture == mSegmentInfo[i].mTexID) {
			glBindTexture(GL_TEXTURE_2D, mSegmentInfo[i].mTexID);
			OpenGLStates::mCurrentTexture = 0;
		}
		
		glDrawRangeElements(mRenderMode, mSegmentInfo[i].mStart, mSegmentInfo[i].mEnd, mIndicesSize, GL_UNSIGNED_INT, (void*)0);
	}
	
	glDisableVertexAttribArray(uair::OpenGLStates::mTexExistsLocation);
	glDisableVertexAttribArray(uair::OpenGLStates::mTexCoordLocation);
	glDisableVertexAttribArray(uair::OpenGLStates::mColourLocation);
	glDisableVertexAttribArray(uair::OpenGLStates::mNormalLocation);
	glDisableVertexAttribArray(uair::OpenGLStates::mVertexLocation);
}

void VBO::EnsureInitialised() {
	if (mVertVBOID == 0) {
		glGenBuffers(1, &mVertVBOID);
	}
	
	if (mIndVBOID == 0) {
		glGenBuffers(1, &mIndVBOID);
	}
}
}
