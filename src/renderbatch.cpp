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

#include "renderbatch.hpp"

#include <algorithm>
#include <iostream>

#include "renderable.hpp"

namespace uair {
void RenderBatch::Add(Renderable& renderable, const unsigned int& pass) {
	std::vector<RenderBatchData>& vecRender = ((mRenderData.insert(IndexedRenderBatchData(pass, {}))).first)->second; // create a new vector for the specified pass, or return the existing one
	vecRender.push_back(renderable.Upload()); // call the renderables upload function and add the result to the batch data container
}

void RenderBatch::Upload() {
	std::map< unsigned int, std::vector<SegmentInfo> > segmentInfo; // container holding segment data
	std::vector<uair::VBOVertex> verts; // final container for all vertex data
	std::vector<uair::VBOIndex> inds; // final container for all index data
	
	GLuint offset = 0;
	
	std::size_t vertCount = 0; // the total current vertex count for the vbo
	
	for (auto iter = mRenderData.begin(); iter != mRenderData.end(); ++iter) {
		unsigned int pass = iter->first; // get the render pass from the iterator
		std::vector<RenderBatchData>& vecRender = iter->second; // get the vector from the iterator
		std::vector<SegmentInfo>& vecSegment = ((segmentInfo.insert(IndexedSegmentInfo(pass, {}))).first)->second;
		
		if (vecRender.size() > 0) {
			// a lambda that is used to sort data by texture id (to ensure fewer binds)
			auto sortDataLambda = [](const RenderBatchData & first, const RenderBatchData & second)->bool {
				return (first.mTexID < second.mTexID);
			};
			
			std::sort(vecRender.begin(), vecRender.end(), sortDataLambda); // sort the data by texture id
			
			GLuint count = 0;
			GLuint min = UINT_MAX;
			GLuint max = 0;
			
			for (unsigned int i = 0u; i < vecRender.size(); ++i) { // for all batches of data...
				for (unsigned int j = 0u; j < vecRender.at(i).mIndData.size(); ++j) { // for all indices in the batch...
					vecRender.at(i).mIndData[j] += vertCount; // add the total vertex count to each of the indices
					
					min = std::min(min, vecRender.at(i).mIndData[j]); // store the smallest index
					max = std::max(max, vecRender.at(i).mIndData[j]); // store the largest index
				}
				
				vertCount += vecRender.at(i).mVertData.size(); // add the current batches vertex count to the total
			}
			
			GLuint currentTex = vecRender.at(0).mTexID; // the current texture id we are checking
			GLenum currentRenderMode = vecRender.at(0).mRenderMode; // the current render mode we are checking
			for (unsigned int i = 0u; i < vecRender.size(); ++i) { // for all batches of data...
				if (vecRender.at(i).mTexID != currentTex || vecRender.at(i).mRenderMode != currentRenderMode) { // if the texture ids or render modes don't match...
					vecSegment.emplace_back(currentTex, currentRenderMode, count, offset, min, max); // add the current segment info to the container
					
					currentTex = vecRender.at(i).mTexID; // update the current texture id
					currentRenderMode = vecRender.at(i).mRenderMode; // update the current render mode
					offset += count;
					count = 0;
				}
				
				count += vecRender.at(i).mIndData.size(); // add the current index count to the end limit
			}
			
			vecSegment.emplace_back(currentTex, currentRenderMode, count, offset, min, max); // add the final segment info
			offset += count;
			
			for (unsigned int i = 0u; i < vecRender.size(); ++i) { // for all batches of data
				// add the data to the final containers
				verts.insert(verts.end(), vecRender.at(i).mVertData.begin(), vecRender.at(i).mVertData.end());
				inds.insert(inds.end(), vecRender.at(i).mIndData.begin(), vecRender.at(i).mIndData.end());
			}
			
			vecRender.clear(); // clear any stored batch data
		}
	}
	
	mVBO.AddData(verts, inds, segmentInfo); // add the vertices, indices and segment data to the vbo
	mRenderData.clear(); // clear (now empty) batch data containers
}

void RenderBatch::Draw(const unsigned int& pass) {
	mVBO.Draw(pass); // draw the vbo
}

void RenderBatch::Draw(const FBO& fbo, const unsigned int& pass) {
	mVBO.Draw(fbo, pass); // draw the vbo
}
}
