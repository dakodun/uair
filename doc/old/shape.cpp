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

#include "shape.hpp"

#include <iostream>
#include <algorithm>

#include "game.hpp"
#include "renderbatch.hpp"
#include "triangulate.hpp"
#include "util.hpp"

namespace uair {
void Shape::AddPoint(const glm::vec2& point, const unsigned int& coordinateSpace) {
	Polygon::AddPoint(point, coordinateSpace);
	
	for (auto iter = mIndices.begin(); iter != mIndices.end(); ++iter) { // for all index groups...
		iter->clear(); // clear the now invalidated indices
	}
}

void Shape::AddPoints(const std::vector<glm::vec2>& points, const unsigned int& coordinateSpace) {
	Polygon::AddPoints(points, coordinateSpace);
	
	for (auto iter = mIndices.begin(); iter != mIndices.end(); ++iter) { // for all index groups...
		iter->clear(); // clear the now invalidated indices
	}
}

void Shape::Clear() {
	Polygon::Clear();
	
	for (auto iter = mIndices.begin(); iter != mIndices.end(); ++iter) { // for all index groups...
		iter->clear();
	}
}

std::string Shape::GetTag() const {
	return "Shape";
}

void Shape::Process() {
	if (mIsAnimated) {
		int currentFrame = mCurrentFrame;
		while (mAnimationTimer >= mAnimationLimit) {
			mAnimationTimer -= mAnimationLimit;
			currentFrame += mAnimationDirection;
			
			if (currentFrame < 0 || currentFrame >= static_cast<int>(mFrames.size()) || currentFrame == static_cast<int>(mAnimationEndFrame) + mAnimationDirection) {
				currentFrame = mAnimationStartFrame;
			}
		}
		
		mCurrentFrame = currentFrame;
		mAnimationTimer += GAME.mFrameLowerLimit;
	}
}

void Shape::AddFrame(ResourcePtr<Texture> texture, const std::vector<glm::vec2>& textureCoords, const unsigned int& layer) {
	Texture * tex = texture.GetResource();
	
	if (tex) {
		if (layer < tex->GetDepth()) {
			TextureData texData = tex->GetData(layer);
			
			AnimationFrame frame;
			frame.mTexture = texture;
			frame.mTexCoords.insert(frame.mTexCoords.end(), textureCoords.begin(), textureCoords.end());
			frame.mLayer = layer;
			mFrames.push_back(std::move(frame));
		}
	}
}

void Shape::AddFrame(Texture* texture, const std::vector<glm::vec2>& textureCoords, const unsigned int& layer) {
	AddFrame(ResourcePtr<Texture>(texture), textureCoords, layer);
}

void Shape::SetAnimation(const float& speed, const unsigned int& start, const unsigned int& end, const int& loops) {
	size_t frameCount = mFrames.size();
	
	if (frameCount > 0) {
		if (std::abs(speed) <= util::EPSILON) {
			mIsAnimated = false;
			mAnimationLimit = 0.0f;
		}
		else {
			mIsAnimated = true;
			mAnimationLimit = (1 / std::abs(speed));
		}
		
		mAnimationDirection = util::SignOf(speed);
		
		mAnimationStartFrame = start;
		if (mAnimationStartFrame < 0) {
			mAnimationStartFrame = 0;
		}
		else if (mAnimationStartFrame >= frameCount) {
			mAnimationStartFrame = frameCount - 1;
		}
		
		mAnimationEndFrame = end;
		if (mAnimationEndFrame < 0) {
			mAnimationEndFrame = 0;
		}
		else if (mAnimationEndFrame >= mFrames.size()) {
			mAnimationEndFrame = frameCount - 1;
		}
		
		mAnimationLoopCount = loops;
		
		mAnimationTimer = 0.0f;
		mCurrentFrame = mAnimationStartFrame;
	}
}

RenderBatchData Shape::Upload() {
	RenderBatchData rbd;
	
	glm::mat3 trans = mTransformation;
	
	{
		trans *= util::GetTranslationMatrix(mPosition - mOrigin);
		
		trans *= util::GetTranslationMatrix(mOrigin);
		trans *= util::GetRotationMatrix(mRotation);
		trans *= util::GetSkewingMatrix(mSkew);
		trans *= util::GetScalingMatrix(mScale);
		trans *= util::GetTranslationMatrix(-mOrigin);
	}
	
	std::vector<unsigned int> vertCounts;
	vertCounts.push_back(CreateVBOVertices(mPoints, trans, rbd));
	for (auto iter = mInnerBoundaries.begin(); iter != mInnerBoundaries.end(); ++iter) {
		vertCounts.push_back(CreateVBOVertices(*iter, trans, rbd));
	}
	
	size_t indexType = 0u;
	if (mIndices.size() >= 5) {
		switch (mRenderMode) { // depending on render mode...
			case 0 : { // GL_POINTS
				indexType = 0u;
				
				if (mIndices.at(indexType).empty()) { // if indices haven't been calculated yet...
					unsigned int count = 0u;
					for (auto iter = vertCounts.begin(); iter != vertCounts.end(); ++iter) {
						for (unsigned int i = 0u; i < *iter; ++i) {
							mIndices.at(indexType).emplace_back(i + count);
						}
						
						count += *iter;
					}
				}
				
				break;
			}
			case 1 : { // GL_LINES
				indexType = 1u;
				
				if (mIndices.at(indexType).empty()) { // if indices haven't been calculated yet...
					unsigned int count = 0u;
					for (auto iter = vertCounts.begin(); iter != vertCounts.end(); ++iter) {
						for (unsigned int i = 0u; i < *iter - 1; i += 2) {
							mIndices.at(indexType).emplace_back(i + count);
							mIndices.at(indexType).emplace_back((i + 1) + count);
						}
						
						count += *iter;
					}
				}
				
				break;
			}
			case 2 : { // GL_LINE_LOOP
				indexType = 2u;
				
				std::cout << "HI" << std::endl;
				if (mIndices.at(indexType).empty()) { // if indices haven't been calculated yet...
					mRenderMode = GL_LINES;
					
					unsigned int count = 0u;
					for (auto iter = vertCounts.begin(); iter != vertCounts.end(); ++iter) {
						for (unsigned int i = 0u; i < *iter - 1; ++i) {
							mIndices.at(indexType).emplace_back(i + count);
							mIndices.at(indexType).emplace_back((i + 1) + count);
						}
						
						mIndices.at(indexType).emplace_back((*iter - 1) + count);
						mIndices.at(indexType).emplace_back(count);
						count += *iter;
					}
				}
				std::cout << "BYE" << std::endl;
				
				break;
			}
			case 3 : { // GL_LINE_STRIP
				indexType = 3u;
				
				if (mIndices.at(indexType).empty()) { // if indices haven't been calculated yet...
					mRenderMode = GL_LINES;
					
					unsigned int count = 0u;
					for (auto iter = vertCounts.begin(); iter != vertCounts.end(); ++iter) {
						for (unsigned int i = 0u; i < *iter - 1; ++i) {
							mIndices.at(indexType).emplace_back(i + count);
							mIndices.at(indexType).emplace_back((i + 1) + count);
						}
						
						count += *iter;
					}
				}
				
				break;
			}
			case 4 : // GL_TRIANGLES
			case 5 : // GL_TRIANGLE_STRIP
			case 6 : { // GL_TRIANGLE_FAN
				indexType = 4u;
				
				if (mIndices.at(indexType).empty()) { // if indices haven't been calculated yet...
					uair::Triangulate t;
					mIndices.at(indexType) = t.Process(mPoints, mInnerBoundaries);
				}
				
				{
					bool reverseWinding = false;
					if (util::CompareFloats(mScale.x, util::LessThan, 0.0)) {
						reverseWinding = !reverseWinding;
					}
					
					if (util::CompareFloats(mScale.y, util::LessThan, 0.0)) {
						reverseWinding = !reverseWinding;
					}
					
					if (reverseWinding) {
						std::reverse(mIndices.at(indexType).begin(), mIndices.at(indexType).end());
					}
				}
				
				break;
			}
			default :
				break;
		}
	}
	
	rbd.mIndData.insert(rbd.mIndData.end(), mIndices.at(indexType).begin(), mIndices.at(indexType).end()); // copy indices into render batch data
	
	rbd.mTexID = 0; // set texture id
	rbd.mRenderMode = mRenderMode; // set render mode
	rbd.mTag = GetTag(); // set renderables tag
	
	if (mCurrentFrame < mFrames.size()) { // if shape is textured...
		rbd.mTexID = mFrames.at(mCurrentFrame).mTexture.GetResource()->GetTextureID(); // set texture id
	}
	
	std::cout << "DONE" << std::endl;
	return rbd; // return batch data
}

unsigned int Shape::CreateVBOVertices(const std::vector<glm::vec2>& points, const glm::mat3& transform, RenderBatchData& batchData) {
	float texAvailable = 0.0f;
	std::vector<glm::vec2> texCoords;
	float texLayer = 0.0f;
	
	if (mCurrentFrame < mFrames.size()) {
		texAvailable = 1.0f;
		
		texCoords.insert(texCoords.end(), mFrames.at(mCurrentFrame).mTexCoords.begin(), mFrames.at(mCurrentFrame).mTexCoords.end());
		texLayer = mFrames.at(mCurrentFrame).mLayer;
	}
	
	size_t difference = points.size() - texCoords.size();
	if (difference > 0) {
		texCoords.insert(texCoords.end(), difference, glm::vec2(0.0f, 0.0f));
	}
	
	unsigned int count = 0u;
	for (unsigned int i = 0; i < points.size(); ++i) {
		glm::vec3 pos = transform * glm::vec3(points.at(i), 1.0f);
		
		VBOVertex vert;
		vert.mX = pos.x; vert.mY = pos.y; vert.mZ = mDepth + 1000.5f;
		vert.mNX = 0.0f; vert.mNY = 0.0f; vert.mNZ = 1.0f;
		vert.mS = texCoords.at(i).x; vert.mT = texCoords.at(i).y; vert.mLayer = texLayer;
		vert.mR = mColour.x; vert.mG = mColour.y; vert.mB = mColour.z; vert.mA = mAlpha;
		vert.mTex = texAvailable;
		
		batchData.mVertData.push_back(vert);
		++count;
	}
	
	return count;
}
}
