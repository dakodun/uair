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

#include "renderbatch.hpp"

#include <algorithm>
#include <iostream>

#include "renderable.hpp"

namespace uair {
void RenderBatch::Add(Renderable& renderable, const unsigned int& pass) {	
	// call the renderables upload function and store the returned render data
	std::list<RenderBatchData> rbd = renderable.Upload();
	
	for (auto iter = rbd.begin(); iter != rbd.end(); ++iter) { // for all render data in the returned list...
		if (!iter->mShader) { // if the render data doesn't have a shader associated with it...
			iter->mShader = VBO::mDefaultShader; // assign the default shader
		}
		
		iter->mPass = pass; // set the pass that the data is to be rendered on
		mRenderData.push_back(*iter); // add the data to the array
	}
}

void RenderBatch::Upload() {
	if (!mRenderData.empty()) { // if upload data exists...
		std::vector<VBOVertex> vertices; // final container for all vertex data
		std::vector<VBOIndex> indices; // final container for all index data
		std::vector<Segment> segments; // container holding segment data
		
		std::size_t vertexCount = 0u; // the total current vertex count for the vbo
		GLuint offset = 0u;
		unsigned int min = std::numeric_limits<unsigned int>::max();
		unsigned int max = 0u;
		
		std::sort(mRenderData.begin(), mRenderData.end(), RenderDataSort); // sort the data first by pass, then by shader id and finally by texture id
		
		unsigned int currentPass = mRenderData.at(0).mPass;
		ResourcePtr<Shader> currentShader = mRenderData.at(0).mShader;
		GLuint currentTextureID = mRenderData.at(0).mTextureID;
		GLuint currentRenderMode = mRenderData.at(0).mRenderMode;
		unsigned int currentMin = std::numeric_limits<unsigned int>::max();
		unsigned int currentMax = 0u;
		std::size_t currentCount = 0u;
		
		for (unsigned int i = 0u; i < mRenderData.size(); ++i) { // for all render data...
			if (!mRenderData.at(i).mShader) { // if render data has an invalid shader...
				continue; // skip data
			}
			
			for (unsigned int j = 0u; j < mRenderData.at(i).mIndexData.size(); ++j) { // for all indices in the current render data...
				mRenderData.at(i).mIndexData.at(j) += vertexCount; // add the total vertex count to each of the indices
				
				min = std::min(min, mRenderData.at(i).mIndexData.at(j)); // store the smallest index
				max = std::max(max, mRenderData.at(i).mIndexData.at(j)); // store the largest index
			}
			
			vertexCount += mRenderData.at(i).mVertexData.size(); // add the current batch's vertex count to the total
			
			if (mRenderData.at(i).mPass != currentPass || mRenderData.at(i).mShader->GetProgramID() != currentShader->GetProgramID() ||
					mRenderData.at(i).mTextureID != currentTextureID || mRenderData.at(i).mRenderMode != currentRenderMode) {
				
				currentMin = std::min(currentMin, currentMax); // ensure the minimum is smaller than the maximum
				segments.push_back({currentPass, 0u, currentShader, currentTextureID, currentRenderMode,
						static_cast<GLuint>(currentCount), offset, currentMin, currentMax});
				offset += currentCount;
				
				// reset current statuses to match current render data...
				currentPass = mRenderData.at(i).mPass;
				currentShader = mRenderData.at(i).mShader;
				currentTextureID = mRenderData.at(i).mTextureID;
				currentRenderMode = mRenderData.at(i).mRenderMode;
				currentMin = std::numeric_limits<unsigned int>::max();
				currentMax = 0u;
				currentCount = 0u;
				// ...end
			}
			
			currentCount += mRenderData.at(i).mIndexData.size(); // 
			
			// update the current minimum and maximum index for the current batch...
			currentMin = std::min(currentMin, min);
			currentMax = std::max(currentMax, max);
			min = std::numeric_limits<unsigned int>::max();
			max = 0;
			// ...end
			
			// add the current render data to the vertex and index arrays...
			vertices.insert(vertices.end(), mRenderData.at(i).mVertexData.begin(), mRenderData.at(i).mVertexData.end());
			indices.insert(indices.end(), mRenderData.at(i).mIndexData.begin(), mRenderData.at(i).mIndexData.end());
			// ...end
		}
		
		currentMin = std::min(currentMin, currentMax); // ensure the minimum is smaller than the maximum
		segments.push_back({currentPass, 0u, currentShader, currentTextureID, currentRenderMode,
				static_cast<GLuint>(currentCount), offset, currentMin, currentMax});
		
		mVBO.AddData(vertices, indices, segments); // add the vertices, indices and segment data to the vbo
		mRenderData.clear(); // clear batch data containers
	}
}

void RenderBatch::Draw(const unsigned int& pass) {
	mVBO.Draw(pass); // draw the vbo
}

void RenderBatch::Draw(const FBO& fbo, const unsigned int& pass) {
	mVBO.Draw(fbo, pass); // draw the vbo
}

bool RenderBatch::RenderDataSort(const RenderBatchData& first, const RenderBatchData& second) {
	if (!first.mShader || !second.mShader) { // if either render data has an invalid shader...
		return true; // enforce no change in order (invalid entry will be skipped anyway)
	}
	
	if (first.mPass < second.mPass) { // first check if data can be sorted by pass...
		return true;
	}
	else if (first.mPass == second.mPass) {
		// ...then check if data can be sorted by shader id...
		if (first.mShader.GetConst()->GetProgramID() < second.mShader.GetConst()->GetProgramID()) {
			return true;
		}
		else if (first.mShader.GetConst()->GetProgramID() == second.mShader.GetConst()->GetProgramID()) {
			if (first.mTextureID < second.mTextureID) { // ...then check if data can be sorted by texture id...
				return true;
			}
			else if (first.mTextureID == second.mTextureID) {
				if (first.mRenderMode < second.mRenderMode) { // ...finally check if data can be sorted by rendermode
					return true;
				}
			}
		}
	}
	
	return false;
}
}
