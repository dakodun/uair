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

#include "vbo.hpp"

#include <iostream>

#include "openglstates.hpp"
#include "renderbatch.hpp"
#include "fbo.hpp"

namespace uair {
ResourcePtr<Shader> VBO::mDefaultShader;

VBO::VBO(VBO&& other) : VBO() {
	swap(*this, other);
}

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
	
	for (auto iter = mVAOStore.begin(); iter != mVAOStore.end(); ++iter) { // for all VAOs...
		if (OpenGLStates::mCurrentVertexArray == iter->second) {
			OpenGLStates::BindVertexArray(0);
		}
		
		glDeleteVertexArrays(1, &(iter->second));
	}
	
	mVAOStore.clear(); // clear the VAO ID map
}

VBO& VBO::operator=(VBO other) {
	swap(*this, other);
	
	return *this;
}

void swap(VBO& first, VBO& second) {
	std::swap(first.mType, second.mType);
	std::swap(first.mMinimumSize, second.mMinimumSize);
	
	std::swap(first.mVertVBOID, second.mVertVBOID);
	std::swap(first.mIndVBOID, second.mIndVBOID);
	
	std::swap(first.mSegmentInfo, second.mSegmentInfo);
	std::swap(first.mVAOStore, second.mVAOStore);
}

void VBO::AddData(const std::vector<VBOVertex>& vertData, const std::vector<VBOIndex>& indData) {
	unsigned int min = std::numeric_limits<unsigned int>::max();
	unsigned int max = 0u;
	
	for (auto iter = indData.begin(); iter != indData.end(); ++iter) {
		min = std::min(min, *iter);
		max = std::max(max, *iter);
	}
	
	Segment segmentInfo(0u, 0u, mDefaultShader, 0u, GL_TRIANGLES, indData.size(), 0u, min, max);
	AddData(vertData, indData, {segmentInfo});
}

void VBO::AddData(const std::vector<VBOVertex>& vertData, const std::vector<VBOIndex>& indData, const std::vector<Segment>& segments) {
	Initialise(); // ensure buffers have been generated
	
	OpenGLStates::BindVertexArray(0); // ensure no VAO is bound before we modify buffers
	OpenGLStates::BindArrayBuffer(mVertVBOID); // bind the vertex buffer
	// calculate the size that the vertex buffer should be and ensure it is greater than the minimum
	std::size_t vertSize = std::max((mMinimumSize * 1048576), (vertData.size() * sizeof(VBOVertex)));
	glBufferData(GL_ARRAY_BUFFER, vertSize, vertData.data(), mType); // initialise the buffer's store
	
	OpenGLStates::BindElementArrayBuffer(mIndVBOID);
	std::size_t indSize = std::max((mMinimumSize * 1048576), (indData.size() * sizeof(VBOIndex)));
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indSize, indData.data(), mType);
	
	mSegmentInfo.clear(); // clear any existing segment info from previous data adds
	
	for (auto iter = segments.begin(); iter != segments.end(); ++iter) { // for all segments in the store...
		if (iter->mShader) {
			GLuint shaderID = iter->mShader.GetConst()->GetProgramID();
			
			GLuint vaoID = 0u;
			auto result = mVAOStore.find(shaderID); // attempt to find matching vao
			if (result != mVAOStore.end()) { // if a matching vao was found...
				vaoID = result->second;
			}
			else {
				glGenVertexArrays(1, &vaoID); // create a new VAO and return its ID
				OpenGLStates::BindVertexArray(vaoID); // bind the newly created vertex array object
				
				OpenGLStates::BindElementArrayBuffer(mIndVBOID);
				iter->mShader->VAOCallback(); // perform vao initialisation according to the shader's needs
				
				OpenGLStates::BindVertexArray(0); // unbind the vertex array object
				
				mVAOStore.emplace(shaderID, vaoID); // add new shaderid/vaoid pair
			}
			
			// create or return the segment info array for current pass and add the current segment info to it
			std::pair< unsigned int, std::vector<Segment> > newPair; newPair.first = iter->mPass;
			std::vector<Segment>& segmentInfo = (mSegmentInfo.insert(newPair).first)->second;
			segmentInfo.push_back(*iter);
			segmentInfo.back().mVAOID = vaoID; // update the VAO ID of the segment
		}
	}
}

void VBO::Draw(const unsigned int& pass) {
	Draw(0, pass);
}

void VBO::Draw(const FBO& fbo, const unsigned int& pass) {
	Draw(fbo.GetFBOID(), pass);
}

void VBO::Initialise() {
	if (mVertVBOID == 0) {
		glGenBuffers(1, &mVertVBOID);
	}
	
	if (mIndVBOID == 0) {
		glGenBuffers(1, &mIndVBOID);
	}
}

void VBO::Draw(const unsigned int& targetID, const unsigned int& pass) {
	auto segmentInfo = mSegmentInfo.find(pass);
	if (segmentInfo != mSegmentInfo.end()) {
		OpenGLStates::BindArrayBuffer(mVertVBOID);
		OpenGLStates::BindFBO(targetID);
		
		GLuint previousShader = 0u;
		
		for (auto iter = segmentInfo->second.begin(); iter != segmentInfo->second.end(); ++iter) {
			// if the shader is valid and differs from the currently bound shader...
			if (iter->mShader && iter->mShader->GetProgramID() != previousShader) {
				OpenGLStates::UseProgram(iter->mShader->GetProgramID()); // set the new current shader
				iter->mShader->RenderCallback(); // call any neccesary setup before rendering (uniforms, textures, etc)
				previousShader = iter->mShader->GetProgramID(); // update the previous shader id to current
				
				OpenGLStates::BindVertexArray(iter->mVAOID); // bind the vertex array (no effect if already bound)
			}
			
			OpenGLStates::BindTexture(iter->mTextureID); // bind the required texture (no effect if already bound)
			
			glDrawRangeElements(iter->mRenderMode, iter->mMinIndex, iter->mMaxIndex,
					iter->mIndicesCount, GL_UNSIGNED_INT, (const GLuint*)(0) + iter->mIndicesOffset);
		}
		
		OpenGLStates::BindVertexArray(0); // unbind current VAO to prevent accidental overwriting
	}
}
}
