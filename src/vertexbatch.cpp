/* **************************************************************** **
**	Uair Engine
**	Copyright (c) 2010 - 2017, Iain M. Crawford
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

#include "vertexbatch.hpp"

namespace uair {
/* std::list<RenderBatchData> VertexBatch::Upload() {
	// if array is less than vertex array then pad it with default values
	mColourData.insert(mColourData.end(), std::max(mVertexData.size() - mColourData.size(), 0), glm::vec4(1.0f));
	mTextureData.insert(mTextureData.end(), std::max(mVertexData.size() - mTextureData.size(), 0), glm::vec3(1.0f));
	mNormalData.insert(mNormalData.end(), std::max(mVertexData.size() - mNormalData.size(), 0), glm::vec3(0.0f, 0.0f, 1.0f));
	
	for (unsigned int i = 0u; i < mVertexData.size(); ++i) {
		VBOVertex vert; // create vertex suitable for rendering
		vert.mX = mVertexData.at(i).x; vert.mY = mVertexData.at(i).y; vert.mZ = mVertexData.at(i).z + mDepth;
		vert.mNX = mNormalData.at(i).x; vert.mNY = mNormalData.at(i).y; vert.mNZ = mNormalData.at(i).z;
		vert.mS = mTextureData.at(i).x; vert.mT = 1.0f - mTextureData.at(i).y; vert.mLayer = mTextureData.at(i).z;
		vert.mR = mColourData.at(i).x; vert.mG = mColourData.at(i).y; vert.mB = mColourData.at(i).z; vert.mA = mColourData.at(i).w;
		vert.mType = 0.0f; vert.mExtra[0] = texAvailable; vert.mExtra[1] = 0.0f;
	}
} */
}
