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
void RenderBatch::Add(const Renderable & renderable) {
	// call the renderables upload function and add it the result to the batch data container
	mRenderData.push_back(renderable.Upload());
}

void RenderBatch::Upload() {
	if (mRenderData.size() > 0) {
		std::vector<SegmentInfo> segmentInfo; // container holding segment data
		
		// a lambda that is used to sort data by texture id (to ensure fewer binds)
		auto sortDataLambda = [](const RenderBatchData & first, const RenderBatchData & second)->bool {
			return (first.mTexID < second.mTexID);
		};
		
		std::sort(mRenderData.begin(), mRenderData.end(), sortDataLambda); // sort the data by texture id
		
		GLuint currentTex = mRenderData[0].mTexID; // the current texture id we are checking
		GLuint start = 0; GLuint end = mRenderData[0].mIndData.size() - 1; // the current start and end limits (inclusive)
		for (unsigned int i = 1u; i < mRenderData.size(); ++i) { // for all batches of data
			if (mRenderData[i].mTexID != currentTex) { // if the texture ids don't match
				segmentInfo.emplace_back(currentTex, start, end); // add the current segment info to the container
				
				currentTex = mRenderData[i].mTexID; // update the current texture id
				start = end + 1; end = start + mRenderData[i].mIndData.size() - 1; // update the current limits
				
				continue; // go to the next loop iteration
			}
			
			end += mRenderData[i].mIndData.size(); // add the current index count to the end limit
		}
		
		segmentInfo.emplace_back(currentTex, start, end); // add the final segment info
		
		std::size_t vertCount = mRenderData[0].mVertData.size(); // get the number of vertices in the first batch of data
		for (unsigned int i = 1u; i < mRenderData.size(); ++i) { // for all other batches of data
			for (unsigned int j = 0u; j < mRenderData[i].mIndData.size(); ++j) { // for all indices in the batch
				mRenderData[i].mIndData[j] += vertCount; // add the total vertex count to each of the indices
			}
			
			vertCount += mRenderData[i].mVertData.size(); // add the current batches vertex count to the total
		}
		
		std::vector<uair::VBOVertex> verts; // final container for all vertex data
		std::vector<uair::VBOIndex> inds; // final container for all index data
		for (unsigned int i = 0u; i < mRenderData.size(); ++i) { // for all batches of data
			// add the data do the final containers
			verts.insert(verts.end(), mRenderData[i].mVertData.begin(), mRenderData[i].mVertData.end());
			inds.insert(inds.end(), mRenderData[i].mIndData.begin(), mRenderData[i].mIndData.end());
		}
		
		mVBO.AddData(verts, inds, segmentInfo); // add the vertices, indices and segment data to the vbo
		
		mRenderData.clear(); // clear any stored batch data
	}
}

void RenderBatch::Draw() {
	mVBO.Draw(); // draw the vbo
}
}
