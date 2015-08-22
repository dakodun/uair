/* **************************************************************** **
**	Uair Engine
**	Copyright (c) 20XX Iain M. Crawford
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

#include "vbo.hpp"

#include <iostream>

#include "openglstates.hpp"
#include "renderbatch.hpp"
#include "fbo.hpp"

namespace uair {
VBO::~VBO() {
	if (mVertVBOID != 0) {
		if (OpenGLStates::mCurrentArrayBuffer == mVertVBOID) {
			OpenGLStates::BindArrayBuffer(0);
		}
		
		glDeleteBuffers(1, &mVertVBOID);
		mVertVBOID = 0;
	}
	
	if (mIndVBOID != 0) {
		if (OpenGLStates::mCurrentElementArrayBuffer == mIndVBOID) {
			OpenGLStates::BindElementArrayBuffer(0);
		}
		
		glDeleteBuffers(1, &mIndVBOID);
		mIndVBOID = 0;
	}
}

void VBO::AddData(const std::vector<VBOVertex>& vertData, const std::vector<VBOIndex>& indData) {
	std::vector<SegmentInfo> vecSegment; // container holding segment data
	vecSegment.emplace_back(0, GL_TRIANGLE_FAN, indData.size(), 0, 0, indData.size());
	
	std::map< unsigned int, std::vector<SegmentInfo> > segmentInfo;
	segmentInfo.insert(IndexedSegmentInfo(0, vecSegment));
	AddData(vertData, indData, segmentInfo);
}

void VBO::AddData(const std::vector<VBOVertex>& vertData, const std::vector<VBOIndex>& indData, const std::map< unsigned int, std::vector<SegmentInfo> >& segmentInfo) {
	EnsureInitialised();
	
	if (OpenGLStates::mCurrentArrayBuffer != mVertVBOID) {
		glBindBuffer(GL_ARRAY_BUFFER, mVertVBOID);
		OpenGLStates::mCurrentArrayBuffer = mVertVBOID;
	}
	
	std::size_t vertSize = std::max((mMinimumSize * 1024 * 1024), (vertData.size() * sizeof(uair::VBOVertex)));
	glBufferData(GL_ARRAY_BUFFER, vertSize, vertData.data(), mType);
	
	if (OpenGLStates::mCurrentElementArrayBuffer != mIndVBOID) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndVBOID);
		OpenGLStates::mCurrentElementArrayBuffer = mIndVBOID;
	}
	
	std::size_t indSize = std::max((mMinimumSize * 1024 * 1024), (indData.size() * sizeof(uair::VBOIndex)));
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indSize, indData.data(), mType);
	
	mSegmentInfo.clear();
	mSegmentInfo.insert(segmentInfo.begin(), segmentInfo.end());
}

void VBO::Draw(const unsigned int& pass) {
	Draw(0, pass);
}

void VBO::Draw(const FBO& fbo, const unsigned int& pass) {
	Draw(fbo.GetFBOID(), pass);
}

void VBO::EnsureInitialised() {
	if (mVertVBOID == 0) {
		glGenBuffers(1, &mVertVBOID);
	}
	
	if (mIndVBOID == 0) {
		glGenBuffers(1, &mIndVBOID);
	}
}

void VBO::Draw(const unsigned int& targetID, const unsigned int& pass) {
	std::vector<SegmentInfo>& segmentInfo = ((mSegmentInfo.insert(IndexedSegmentInfo(pass, {}))).first)->second; // either create a new segment info vector or get the existing one
	
	if (!segmentInfo.empty()) { // if we have something to render this pass...
		OpenGLStates::BindArrayBuffer(mVertVBOID);
		OpenGLStates::BindElementArrayBuffer(mIndVBOID);
		
		glUniformMatrix4fv(uair::OpenGLStates::mViewMatrixLocation, 1, GL_FALSE, &uair::OpenGLStates::mViewMatrix[0][0]);
		glUniformMatrix4fv(uair::OpenGLStates::mModelMatrixLocation, 1, GL_FALSE, &uair::OpenGLStates::mModelMatrix[0][0]);
		glUniformMatrix4fv(uair::OpenGLStates::mProjectionMatrixLocation, 1, GL_FALSE, &uair::OpenGLStates::mProjectionMatrix[0][0]);
		
		glEnableVertexAttribArray(uair::OpenGLStates::mVertexLocation);
		glEnableVertexAttribArray(uair::OpenGLStates::mNormalLocation);
		glEnableVertexAttribArray(uair::OpenGLStates::mColourLocation);
		glEnableVertexAttribArray(uair::OpenGLStates::mTexCoordLocation);
		glEnableVertexAttribArray(uair::OpenGLStates::mTypeLocation);
		glEnableVertexAttribArray(uair::OpenGLStates::mExtraLocation);
		
		glVertexAttribPointer(uair::OpenGLStates::mVertexLocation, 3, GL_FLOAT, GL_TRUE, sizeof(uair::VBOVertex), (void*)(offsetof(uair::VBOVertex, mX)));
		glVertexAttribPointer(uair::OpenGLStates::mNormalLocation, 3, GL_FLOAT, GL_TRUE, sizeof(uair::VBOVertex), (void*)(offsetof(uair::VBOVertex, mNX)));
		glVertexAttribPointer(uair::OpenGLStates::mColourLocation, 4, GL_FLOAT, GL_TRUE, sizeof(uair::VBOVertex), (void*)(offsetof(uair::VBOVertex, mR)));
		glVertexAttribPointer(uair::OpenGLStates::mTexCoordLocation, 3, GL_FLOAT, GL_TRUE, sizeof(uair::VBOVertex), (void*)(offsetof(uair::VBOVertex, mS)));
		glVertexAttribPointer(uair::OpenGLStates::mTypeLocation, 1, GL_FLOAT, GL_TRUE, sizeof(uair::VBOVertex), (void*)(offsetof(uair::VBOVertex, mType)));
		glVertexAttribPointer(uair::OpenGLStates::mExtraLocation, 2, GL_FLOAT, GL_TRUE, sizeof(uair::VBOVertex), (void*)(offsetof(uair::VBOVertex, mExtra)));
		
		if (OpenGLStates::mCurrentFBO != targetID) {
			glBindFramebuffer(GL_FRAMEBUFFER, targetID);
			OpenGLStates::mCurrentFBO = targetID;
		}
		
		for (unsigned int i = 0u; i < segmentInfo.size(); ++i) {
			OpenGLStates::BindTexture(segmentInfo[i].mTexID);
			glDrawRangeElements(segmentInfo[i].mRenderMode, segmentInfo[i].mMinIndex, segmentInfo[i].mMaxIndex, segmentInfo[i].mIndicesCount, GL_UNSIGNED_INT, (const GLuint*)(0) + segmentInfo[i].mIndicesOffset);
		}
		
		glDisableVertexAttribArray(uair::OpenGLStates::mExtraLocation);
		glDisableVertexAttribArray(uair::OpenGLStates::mTypeLocation);
		glDisableVertexAttribArray(uair::OpenGLStates::mTexCoordLocation);
		glDisableVertexAttribArray(uair::OpenGLStates::mColourLocation);
		glDisableVertexAttribArray(uair::OpenGLStates::mNormalLocation);
		glDisableVertexAttribArray(uair::OpenGLStates::mVertexLocation);
	}
}
}
