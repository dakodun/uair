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
#include "util.hpp"

namespace uair {
Shape::Shape(const std::vector<Contour>& contours, const CoordinateSpace& coordinateSpace) {
	AddContours(contours, coordinateSpace);
}

Shape::Shape(const ClipperLib::Paths& clipperPaths) {
	FromClipperPaths(clipperPaths);
}

void Shape::AddContour(const Contour& contour, const CoordinateSpace& coordinateSpace) {
	Polygon::AddContour(contour, coordinateSpace);
	
	for (auto iter = mIndices.begin(); iter != mIndices.end(); ++iter) { // for all index groups...
		iter->clear(); // clear the now invalidated indices
	}
	
	mVertices.clear();
	
	for (auto frame = mFrames.begin(); frame != mFrames.end(); ++frame) {
		frame->mTexCoords.clear();
		frame->mTexCoordsExtra.clear();
		
		for (auto contour = mContours.begin(); contour != mContours.end(); ++contour) {
			CalculateTexCoords(contour->GetPoints(), {frame->mMinST, frame->mMaxST}, frame->mTexCoords);
		}
	}
}

void Shape::AddContours(const std::vector<Contour>& contours, const CoordinateSpace& coordinateSpace) {
	Polygon::AddContours(contours, coordinateSpace);
	
	for (auto iter = mIndices.begin(); iter != mIndices.end(); ++iter) { // for all index groups...
		iter->clear(); // clear the now invalidated indices
	}
	
	mVertices.clear();
	
	for (auto frame = mFrames.begin(); frame != mFrames.end(); ++frame) {
		frame->mTexCoords.clear();
		frame->mTexCoordsExtra.clear();
		
		for (auto contour = mContours.begin(); contour != mContours.end(); ++contour) {
			CalculateTexCoords(contour->GetPoints(), {frame->mMinST, frame->mMaxST}, frame->mTexCoords);
		}
	}
}

void Shape::Offset(const float& distance, const ClipperLib::JoinType& miterType, const double& miterLimit) {
	std::vector<Contour> outContours;
	for (auto contour = mContours.begin(); contour != mContours.end(); ++contour) {
		std::vector<Contour> offsetContours = contour->GetOffset(distance, miterType, miterLimit);
		outContours.insert(outContours.end(), offsetContours.begin(), offsetContours.end());
	}
	
	mContours.clear();
	mBounds.clear();
	AddContours(outContours);
}

void Shape::FromClipperPaths(const ClipperLib::Paths& clipperPaths) {
	mContours.clear();
	mBounds.clear();
	
	std::vector<Contour> contours;
	for (auto path = clipperPaths.begin(); path != clipperPaths.end(); ++path) {
		contours.emplace_back(*path);
	}
	
	AddContours(contours);
}

Shape Shape::GetTransformed() const {
	Shape newShape; // the transformed shape
	glm::mat3 trans = mTransformation; // the complete transofrmation matrix (initially the custom transform matrix)
	std::vector<Contour> newContours; // the transformed contours array
	
	{ // apply the individual transforms to the tranformation matrix
		trans *= util::GetTranslationMatrix(mPosition - mOrigin);
		
		trans *= util::GetTranslationMatrix(mOrigin);
		trans *= util::GetRotationMatrix(mRotation);
		trans *= util::GetSkewingMatrix(mSkew);
		trans *= util::GetScalingMatrix(mScale);
		trans *= util::GetTranslationMatrix(-mOrigin);
	}
	
	{ // copy the mask and the animation data and properties (unchanged)
		newShape.mLocalMask = mLocalMask;
		newShape.mGlobalMask = mLocalMask;
		
		newShape.mWindingRule = mWindingRule;
		
		newShape.mFrames = mFrames;
		newShape.mCurrentFrame = mCurrentFrame;
		newShape.mIsAnimated = mIsAnimated;
		newShape.mAnimationDirection = mAnimationDirection;
		newShape.mAnimationLimit = mAnimationLimit;
		newShape.mAnimationTimer = mAnimationTimer;
		newShape.mAnimationLoopCount = mAnimationLoopCount;
		newShape.mAnimationStartFrame = mAnimationStartFrame;
		newShape.mAnimationEndFrame = mAnimationEndFrame;
		
		newShape.mName = mName;
		
		newShape.mDepth = mDepth;
		newShape.mColour = mColour;
		newShape.mAlpha = mAlpha;
	}
	
	for (auto contour = mContours.begin(); contour != mContours.end(); ++contour) { // for all contours in the shape...
		Contour newContour; // the transformed contour
		for (auto point = contour->mPoints.begin(); point != contour->mPoints.end(); ++point) { // for all points in the contour...
			glm::vec3 pos = trans * glm::vec3(*point, 1.0f); // get the transformed point (as a 3d vector)
			
			newContour.AddPoint(glm::vec2(pos.x, pos.y)); // add the point to the transformed contour (only x and y are needed)
		}
		
		newContours.push_back(std::move(newContour)); // add the transformed contours to the transformed contours array
	}
	
	newShape.Polygon::AddContours(newContours); // add the transformed contours to the new shape (call the base method directly to avoid messing with any texturing)
	return newShape; // return the new shape
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

void Shape::SetWindingRule(const WindingRule& windingRule) {
	if (mWindingRule != windingRule) { // if winding rule is not already set (prevents unneccesary retriangulation)...
		mWindingRule = windingRule; // set the winding rule
		
		std::vector<VBOIndex> indices; // create empty index array
		std::vector<glm::vec2> vertices; // create empty vertex array
		
		// replace current arrays with the empty arrays
		std::swap(indices, mIndices[4]);
		std::swap(vertices, mVertices);
	}
}

void Shape::AddFrame(ResourcePtr<Texture> texture, const unsigned int& layer, const std::vector<glm::vec2>& textureRect) {
	Texture* tex = texture.GetResource();
	
	if (tex) {
		if (layer < tex->GetDepth()) {
			const TextureData& texData = tex->GetData(layer);
			
			// set default values for the texture ST coordinate bounds
			glm::vec2 min = glm::vec2(0.0f, 0.0f); // top-left of texture
			glm::vec2 max = glm::vec2(texData.mSMax, texData.mTMax); // bottom-right of texture
			
			if (textureRect.size() == 2) { // if a custom texture rect was supplied...
				// calculate the min and max (top-left and bottom-right) ST coordinates of the required portion of the texture
				min = glm::vec2(textureRect.at(0).x / texData.mWidth, textureRect.at(0).y / texData.mHeight);
				max = glm::vec2(textureRect.at(1).x / texData.mWidth, textureRect.at(1).y / texData.mHeight);
			}
			
			AnimationFrame frame; // create a new frame
			frame.mTexture = texture; // set the texture pointer of the frame
			frame.mMinST = min; frame.mMaxST = max; // set the ST coordinate limits
			
			for (auto contour = mContours.begin(); contour != mContours.end(); ++contour) { // for all contours in the shape...
				CalculateTexCoords(contour->GetPoints(), {frame.mMinST, frame.mMaxST}, frame.mTexCoords); // calculate the texture coordinates for the contour for the current frame
			}
			
			CalculateTexCoords(mVertices, {frame.mMinST, frame.mMaxST}, frame.mTexCoordsExtra);// calculate the texture coordinates for the extra triangulation vertices for the current frame
			
			frame.mLayer = layer;
			mFrames.push_back(std::move(frame));
		}
	}
}

void Shape::AddFrame(Texture* texture, const unsigned int& layer, const std::vector<glm::vec2>& textureRect) {
	AddFrame(ResourcePtr<Texture>(texture), layer, textureRect);
}

void Shape::AddFrames(ResourcePtr<Texture> texture, const unsigned int& layer, const unsigned int& numFrames, const unsigned int& numPerRow,
		const unsigned int& numPerCol, const glm::ivec2& offset) {
	
	Texture* tex = texture.GetResource();
	
	if (tex) {
		if (layer < tex->GetDepth()) {
			const TextureData& texData = tex->GetData(layer);
			
			float frameWidth = static_cast<float>(texData.mWidth) - (offset.x * numPerRow);
			float frameHeight = static_cast<float>(texData.mHeight) - (offset.y * numPerCol);
			
			glm::vec2 increment = glm::vec2((frameWidth / texData.mWidth) / numPerRow, (frameHeight / texData.mHeight) / numPerCol);
			glm::vec2 offsetIncrement = glm::vec2(offset.x / texData.mWidth, offset.y / texData.mHeight);
			
			for (unsigned int i = 0; i < numFrames; ++i) {
				unsigned int t = i / numPerRow;
				unsigned int s = i % numPerRow;
				
				// calculate the min and max (top-left and bottom-right) ST coordinates of the required portion of the texture
				glm::vec2 min = glm::vec2((s * increment.x) + (s * offsetIncrement.x), (t * increment.y) + (t * offsetIncrement.y)); // top-left of texture
				glm::vec2 max = glm::vec2(((s + 1) * increment.x) + (s * offsetIncrement.x), ((t + 1) * increment.y) + (t * offsetIncrement.y)); // bottom-right of texture
				
				AnimationFrame frame; // create a new frame
				frame.mTexture = texture; // set the texture pointer of the frame
				frame.mMinST = min; frame.mMaxST = max; // set the ST coordinate limits
				
				for (auto contour = mContours.begin(); contour != mContours.end(); ++contour) { // for all contours in the shape...
					CalculateTexCoords(contour->GetPoints(), {frame.mMinST, frame.mMaxST}, frame.mTexCoords); // calculate the texture coordinates for the contour for the current frame
				}
				
				CalculateTexCoords(mVertices, {frame.mMinST, frame.mMaxST}, frame.mTexCoordsExtra);// calculate the texture coordinates for the extra triangulation vertices for the current frame
				
				frame.mLayer = layer;
				mFrames.push_back(std::move(frame));
			}
		}
	}
}

void Shape::AddFrames(Texture* texture, const unsigned int& layer, const unsigned int& numFrames, const unsigned int& numPerRow,
		const unsigned int& numPerCol, const glm::ivec2& offset) {
	
	AddFrames(ResourcePtr<Texture>(texture), layer, numFrames, numPerRow, numPerCol, offset);
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
	trans *= util::GetTranslationMatrix(mPosition - mOrigin);
	
	trans *= util::GetTranslationMatrix(mOrigin);
	trans *= util::GetRotationMatrix(mRotation);
	trans *= util::GetSkewingMatrix(mSkew);
	trans *= util::GetScalingMatrix(mScale);
	trans *= util::GetTranslationMatrix(-mOrigin);
	
	std::vector<unsigned int> vertCounts;
	std::vector<glm::vec2> vertices;
	for (auto iter = mContours.begin(); iter != mContours.end(); ++iter) {
		const std::vector<glm::vec2>& points = iter->GetPoints();
		vertCounts.push_back(points.size());
		vertices.insert(vertices.end(), points.begin(), points.end());
	}
	
	std::vector<glm::vec2> texCoords;
	if (mCurrentFrame < mFrames.size()) {
		texCoords.insert(texCoords.end(), mFrames.at(mCurrentFrame).mTexCoords.begin(), mFrames.at(mCurrentFrame).mTexCoords.end());
	}
	
	CreateVBOVertices(vertices, trans, rbd, texCoords);
	
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
				
				if (mIndices.at(indexType).empty() && mVertices.empty()) { // if indices and vertices haven't been calculated yet...
					uair::Triangulate triangulate;
					
					for (auto iter = mContours.begin(); iter != mContours.end(); ++iter) {
						triangulate.AddContour(std::move(iter->GetPoints()));
					}
					
					Triangulate::Result result = triangulate.Process(mWindingRule);
					mIndices.at(indexType) = result.second;
					for (auto vertex = result.first.begin(); vertex != result.first.end(); ++vertex) {
						mVertices.push_back(vertex->mPoint);
						
						for (auto frame = mFrames.begin(); frame != mFrames.end(); ++frame) {
							glm::vec2 texCoord;
							for (unsigned int i = 0u; i < vertex->mNeighbourIndices.size(); ++i) {
								texCoord += frame->mTexCoords.at(vertex->mNeighbourIndices.at(i)) * vertex->mNeighbourWeights.at(i);
							}
							
							frame->mTexCoordsExtra.push_back(std::move(texCoord));
						}
					}
				}
				
				std::vector<glm::vec2> texCoords;
				if (mCurrentFrame < mFrames.size()) {
					texCoords.insert(texCoords.end(), mFrames.at(mCurrentFrame).mTexCoordsExtra.begin(), mFrames.at(mCurrentFrame).mTexCoordsExtra.end());
				}
				
				CreateVBOVertices(mVertices, trans, rbd, texCoords);
				
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
	
	return rbd; // return batch data
}

void Shape::CalculateTexCoords(const std::vector<glm::vec2>& points, const std::vector<glm::vec2>& texCoordsMax, std::vector<glm::vec2>& texCoordsLocation) {
	if (mBounds.size() > 1) { // if valid bounds have been set up for the shape...
		for (auto point = points.begin(); point != points.end(); ++point) { // for all points in the shape...
			// get the ratio of the current point in regards to the bounds
			glm::vec2 ratio((point->x - mBounds.at(0).x) / (mBounds.at(1).x - mBounds.at(0).x),
					(point->y - mBounds.at(0).y) / (mBounds.at(1).y - mBounds.at(0).y));
			
			// calculate the texture coordinates in regards to the ratio of the current point and the texture rectangle of the current frame's texture
			texCoordsLocation.emplace_back(((1 - ratio.x) * texCoordsMax.at(0).x) + (ratio.x * texCoordsMax.at(1).x),
					((1 - ratio.y) * texCoordsMax.at(0).y) + (ratio.y * texCoordsMax.at(1).y));
		}
	}
}

void Shape::CreateVBOVertices(const std::vector<glm::vec2>& points, const glm::mat3& transform, RenderBatchData& batchData, std::vector<glm::vec2> texCoords) {
	float texAvailable = 0.0f;
	float texLayer = 0.0f;
	
	if (mCurrentFrame < mFrames.size()) {
		texAvailable = 1.0f;
		texLayer = mFrames.at(mCurrentFrame).mLayer;
	}
	
	int difference = points.size() - texCoords.size();
	if (difference > 0) {
		texCoords.insert(texCoords.end(), difference, glm::vec2(0.0f, 0.0f));
	}
	
	for (unsigned int i = 0; i < points.size(); ++i) {
		glm::vec3 pos = transform * glm::vec3(points.at(i), 1.0f);
		
		VBOVertex vert;
		vert.mX = pos.x; vert.mY = pos.y; vert.mZ = mDepth + 1000.5f;
		vert.mNX = 0.0f; vert.mNY = 0.0f; vert.mNZ = 1.0f;
		vert.mS = texCoords.at(i).x; vert.mT = texCoords.at(i).y; vert.mLayer = texLayer;
		vert.mR = mColour.x; vert.mG = mColour.y; vert.mB = mColour.z; vert.mA = mAlpha;
		vert.mTex = texAvailable;
		
		batchData.mVertData.push_back(vert);
	}
}
}
