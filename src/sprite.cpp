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

#include "sprite.hpp"

#include <iostream>

#include "game.hpp"
#include "renderbatch.hpp"
#include "util.hpp"

namespace uair {
std::string Sprite::GetTag() const {
	return "Sprite"; // indicate that this renderable is a sprite
}

void Sprite::Process() {
	if (mIsAnimated == true) {
		int currentFrame = mCurrentFrame;
		while (mAnimationTimer >= mAnimationLimit) {
			mAnimationTimer -= mAnimationLimit;
			currentFrame += mAnimationDirection;
			
			if (currentFrame < 0 || currentFrame >= static_cast<int>(GetFrameCount()) ||
					currentFrame == static_cast<int>(mAnimationEndFrame) + mAnimationDirection) {
				
				currentFrame = mAnimationStartFrame;
			}
		}
		
		mCurrentFrame = currentFrame;
		mAnimationTimer += GAME.mFrameLowerLimit;
	}
}

void Sprite::AddFrame(ResourcePtr<Texture> texture, unsigned int layer) {
	Texture * t = texture.GetResource();
	
	if (t) {
		if (layer < t->GetDepth()) {
			TextureData texData = t->GetData(layer);
			
			if (mFrames.empty() == true) {
				mWidth = texData.mWidth;
				mHeight = texData.mHeight;
				
				// update the local bounding box
				mLocalBoundingBox.Clear();
				mLocalBoundingBox.SetPosition(glm::vec2(0, 0));
				mLocalBoundingBox.AddPoint(glm::vec2(mWidth,       0));
				mLocalBoundingBox.AddPoint(glm::vec2(mWidth, mHeight));
				mLocalBoundingBox.AddPoint(glm::vec2(     0, mHeight));
				
				UpdateGlobalBoundingBox(); // update the global bounding box
			}
			
			SpriteFrame sf;
			sf.mTexture = texture;
			
			sf.mTexCoords.emplace_back(         0.0f,          0.0f);
			sf.mTexCoords.emplace_back(texData.mSMax,          0.0f);
			sf.mTexCoords.emplace_back(texData.mSMax, texData.mTMax);
			sf.mTexCoords.emplace_back(         0.0f, texData.mTMax);
			
			sf.mLayer = layer;
			
			mFrames.push_back(std::move(sf));
		}
	}
}

void Sprite::AddFrame(Texture* texture, unsigned int layer) {
	AddFrame(ResourcePtr<Texture>(texture), layer);
}

void Sprite::AddFrameSheet(ResourcePtr<Texture> texture, const unsigned int & numFrames, const unsigned int & framesPerRow,
		unsigned int layer) {
	
	Texture * t = texture.GetResource();
	
	if (t) {
		if (layer < t->GetDepth()) {
			TextureData texData = t->GetData(layer);
			
			unsigned int columns = framesPerRow;
			if (columns < 1) {
				columns = 1;
			}
			
			unsigned int rows = std::ceil(static_cast<float>(numFrames) / columns);
			if (rows < 1) {
				rows = 1;
			}
			
			unsigned int frameWidth = texData.mWidth / columns;
			unsigned int frameHeight = texData.mHeight / rows;
			
			if (mFrames.empty() == true) {
				mWidth = frameWidth;
				mHeight = frameHeight;
				
				// update the local bounding box
				mLocalBoundingBox.Clear();
				mLocalBoundingBox.SetPosition(glm::vec2(0, 0));
				mLocalBoundingBox.AddPoint(glm::vec2(mWidth,       0));
				mLocalBoundingBox.AddPoint(glm::vec2(mWidth, mHeight));
				mLocalBoundingBox.AddPoint(glm::vec2(     0, mHeight));
				
				UpdateGlobalBoundingBox(); // update the global bounding box
			}
			
			glm::vec2 texCoordInc(texData.mSMax / columns, texData.mSMax / rows);
			for (unsigned int i = 0; i < numFrames; ++i) {
				SpriteFrame sf;
				sf.mTexture = texture;
				
				glm::vec2 texCoords(texCoordInc.x * (i % framesPerRow),
						texCoordInc.y * std::floor(i / framesPerRow));
				
				sf.mTexCoords.emplace_back(                texCoords.x,                 texCoords.y);
				sf.mTexCoords.emplace_back(texCoords.x + texCoordInc.x,                 texCoords.y);
				sf.mTexCoords.emplace_back(texCoords.x + texCoordInc.x, texCoords.y + texCoordInc.y);
				sf.mTexCoords.emplace_back(                texCoords.x, texCoords.y + texCoordInc.y);
				
				sf.mLayer = layer;
				
				mFrames.push_back(std::move(sf));
			}
		}
	}
}

void Sprite::AddFrameSheet(Texture * texture, const unsigned int & numFrames, const unsigned int & framesPerRow,
		unsigned int layer) {
	
	AddFrameSheet(ResourcePtr<Texture>(texture), numFrames, framesPerRow, layer);
}

void Sprite::SetAnimation(const float & speed, const unsigned int & start, const unsigned int & end, const int & loops) {
	if (GetFrameCount() > 0) {
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
		else if (mAnimationStartFrame >= GetFrameCount()) {
			mAnimationStartFrame = GetFrameCount() - 1;
		}
		
		mAnimationEndFrame = end;
		if (mAnimationEndFrame < 0) {
			mAnimationEndFrame = 0;
		}
		else if (mAnimationEndFrame >= GetFrameCount()) {
			mAnimationEndFrame = GetFrameCount() - 1;
		}
		
		mAnimationLoopCount = loops;
		
		mAnimationTimer = 0.0f;
		mCurrentFrame = mAnimationStartFrame;
	}
}

unsigned int Sprite::GetCurrentFrame() const {
	return mCurrentFrame;
}

void Sprite::SetCurrentFrame(const unsigned int & currentFrame) {
	mCurrentFrame = currentFrame;
	
	// add bound checks
}

size_t Sprite::GetFrameCount() const {
	return mFrames.size();
}

Polygon Sprite::GetClip() const {
	return mClip;
}

void Sprite::SetClip(const Polygon& polyClip) {
	// create the source polygon that represents our sprite
	uair::Polygon polySource;
	polySource.SetPosition(glm::vec2(  0.0f,    0.0f));
	   polySource.AddPoint(glm::vec2(mWidth,    0.0f));
	   polySource.AddPoint(glm::vec2(mWidth, mHeight));
	   polySource.AddPoint(glm::vec2(  0.0f, mHeight));
	
	// get the absolute points of the clip polygon and add the position
	std::vector<glm::vec2> ptsClip = polyClip.GetAbsolutePoints();
	ptsClip.insert(ptsClip.begin(), polyClip.GetPosition());
	
	// get the absolute points of the source polygon and add the position
	std::vector<glm::vec2> ptsSource = polySource.GetAbsolutePoints();
	ptsSource.insert(ptsSource.begin(), polySource.GetPosition());
	
	// lambda function that finds the point at which 2 lines intersect
	auto LineLineIntersection = [](const glm::vec2& pointA1, const glm::vec2& pointA2,
			const glm::vec2& pointB1, const glm::vec2& pointB2)->glm::vec2 {
		
		// Ax + By = C
		// A = y2 - y1
		// B = x1 - x2
		// C = A * x1 + B * y1
		
		using namespace uair::util;
		
		// find the components of first line
		float A1 = pointA2.y - pointA1.y;
		float B1 = pointA1.x - pointA2.x;
		float C1 = (A1 * pointA1.x) + (B1 * pointA1.y);
		
		// find the components of second line
		float A2 = pointB2.y - pointB1.y;
		float B2 = pointB1.x - pointB2.x;
		float C2 = (A2 * pointB1.x) + (B2 * pointB1.y);
		
		float determinant = (A1 * B2) - (A2 * B1); // find the determinant (essentially cross product in 2d)
		if (determinant > -EPSILON && determinant < EPSILON) { // if it is 0
			throw std::runtime_error("parallel"); // points are parallel
		}
		else {
			// find and return the point of intersection
			double x = ((B2 * C1) - (B1 * C2)) / determinant;
			double y = ((A1 * C2) - (A2 * C1)) / determinant;
			return glm::vec2(x, y);
		}
	};
	
	
	if (ptsClip.empty() == false) {
		ptsClip.push_back(ptsClip.front()); // copy the first point to the end
	}
	
	int ptsClipSize = static_cast<int>(ptsClip.size()) - 1;
	for (int i = 0; i < ptsClipSize; ++i) { // for all points in the clip polygon
		glm::vec2 clipPtA = ptsClip.at(i); // get the current point
		glm::vec2 clipPtB = ptsClip.at(i + 1); // get the next point
		
		std::vector<glm::vec2> ptsResult; // the resultant points after this clip pass
		if (ptsSource.empty() == false) {
			ptsSource.push_back(ptsSource.front()); // copy the first point to the end
		}
		
		int ptsSourceSize = static_cast<int>(ptsSource.size()) - 1;
		for (int j = 0; j < ptsSourceSize; ++j) { // for all points in the source polygon
			glm::vec2 sourcePtA = ptsSource.at(j); // get the current point
			glm::vec2 sourcePtB = ptsSource.at(j + 1); // get the next point
			
			// assume both points are outside the clip region initially
			bool sourcePtAInside = false;
			bool sourcePtBInside = false;
			
			{
				using namespace uair::util;
				
				glm::vec2 clipLine = clipPtB - clipPtA; // create the clip line from the clip points
				glm::vec2 clipLinePerp(-clipLine.y, clipLine.x); // get the line perpindicular to the clip line (perp clip line)
				
				glm::vec2 sourceLineA = sourcePtA - clipPtA; // get the line from source point A to the first clip point (first line)
				glm::vec2 sourceLineB = sourcePtB - clipPtA; // get the line from source point B to the first clip point (second line)
				
				// if the dot product of the first line and the perp clip line is greater than 0
				if (glm::dot(sourceLineA, clipLinePerp) > EPSILON) {
					sourcePtAInside = true; // indicate first source point is inside the clip region
				}
				
				// if the dot product of the second line and the perp clip line is greater than 0
				if (glm::dot(sourceLineB, clipLinePerp) > EPSILON) {
					sourcePtBInside = true; // indicate second source point is inside the clip region
				}
			}
			
			if (sourcePtAInside == true) { // if the first source point is inside the clip region
				if (sourcePtBInside == true) { // if the second source point is inside the clip region
					ptsResult.push_back(sourcePtB); // add the second point
				}
				else {
					try {
						glm::vec2 intersect = LineLineIntersection(clipPtA, clipPtB, sourcePtA, sourcePtB); // find the intersect of both lines
						ptsResult.push_back(std::move(intersect)); // add the intersect
					} catch (std::exception& e) {
						std::cout << e.what() << std::endl;
					}
				}
			}
			else if (sourcePtBInside == true) {  // otherwise if ONLY the second source point is inside the clip region
				try {
					glm::vec2 intersect = LineLineIntersection(clipPtA, clipPtB, sourcePtA, sourcePtB); // find the intersect of both lines
					ptsResult.push_back(std::move(intersect)); // add the intersect
				} catch (std::exception& e) {
					std::cout << e.what() << std::endl;
				}
				
				ptsResult.push_back(sourcePtB); // add the second point
			}
		}
		
		ptsSource = std::move(ptsResult); // copy the result of this pass to the source polygon
	}
	
	if (ptsSource.empty() == false) { // if the newly clipped source polygon isn't empty
		mClip.Clear(); // clear the current clip polygon
		mClip.SetPosition(ptsSource.at(0)); // set the position of the clip polygon
		ptsSource.erase(ptsSource.begin()); // remove the first point (the position) from the clipped source polygon
		
		if (ptsSource.empty() == false) { // if the clipped source polygon still isn't empty
			mClip.AddAbsolutePoints(ptsSource); // add the remaining points as absolute values to the clip polygon
		}
		
		mIsClipped = true; // indicate this sprite is clipped (for rendering)
		
		std::vector<glm::vec2> bounds = mClip.GetBounds();
		mClippedWidth = bounds.at(1).x - bounds.at(0).x;
		mClippedHeight = bounds.at(1).y - bounds.at(0).y;
		
		// update the local bounding box
		mLocalBoundingBox.Clear();
		mLocalBoundingBox.SetPosition(glm::vec2(0, 0));
		mLocalBoundingBox.AddPoint(glm::vec2(mClippedWidth,              0));
		mLocalBoundingBox.AddPoint(glm::vec2(mClippedWidth, mClippedHeight));
		mLocalBoundingBox.AddPoint(glm::vec2(            0, mClippedHeight));
		
		UpdateGlobalBoundingBox(); // update the global bounding box
	}
	else {
		mIsClipped = false; // indicate this sprite is NOT clipped
		
		// update the local bounding box
		mLocalBoundingBox.Clear();
		mLocalBoundingBox.SetPosition(glm::vec2(0, 0));
		mLocalBoundingBox.AddPoint(glm::vec2(mWidth,       0));
		mLocalBoundingBox.AddPoint(glm::vec2(mWidth, mHeight));
		mLocalBoundingBox.AddPoint(glm::vec2(     0, mHeight));
		
		UpdateGlobalBoundingBox(); // update the global bounding box
	}
}

void Sprite::UpdateGlobalBoundingBox() {
	Polygon poly; // temporary polygon representing the shape (without transforms)
	unsigned int width = mWidth;
	unsigned int height = mHeight;
	
	if (mIsClipped == true) {
		width = mClippedWidth;
		height = mClippedHeight;
	}
	
	poly.AddPoint(glm::vec2(width,   0.0f));
	poly.AddPoint(glm::vec2(width, height));
	poly.AddPoint(glm::vec2( 0.0f, height));
	
	// get the transformation matrix that represents the shape
	glm::mat3 trans = mTransformation;
	
	trans *= util::GetTranslationMatrix(mPosition - mOrigin);
	
	trans *= util::GetTranslationMatrix(mOrigin);
	trans *= util::GetRotationMatrix(mRotation);
	trans *= util::GetSkewingMatrix(mSkew);
	trans *= util::GetScalingMatrix(mScale);
	trans *= util::GetTranslationMatrix(-mOrigin);
	
	poly.Transform(trans); // transform the temp polygon by the shape's transformation
	std::vector<glm::vec2> bounds = poly.GetBounds(); // get the bounds of the temp polygon
	glm::vec2 pos = poly.GetPosition(); // store it's position
	
	// clear the global bounding box and set it to the bounds of the temp polygon
	mGlobalBoundingBox.Clear();
	mGlobalBoundingBox.SetPosition(pos + bounds.at(0));
	mGlobalBoundingBox.AddPoint(glm::vec2(bounds.at(1).x - bounds.at(0).x,                               0));
	mGlobalBoundingBox.AddPoint(glm::vec2(bounds.at(1).x - bounds.at(0).x, bounds.at(1).y - bounds.at(0).y));
	mGlobalBoundingBox.AddPoint(glm::vec2(                              0, bounds.at(1).y - bounds.at(0).y));
}

void Sprite::UpdateGlobalMask() {
	mGlobalMask = mLocalMask; // set the global mask to the local mask
	
	// get the transformation matrix that represents the shape
	glm::mat3 trans = mTransformation;
	
	trans *= util::GetTranslationMatrix(mPosition - mOrigin);
	
	trans *= util::GetTranslationMatrix(mOrigin);
	trans *= util::GetRotationMatrix(mRotation);
	trans *= util::GetSkewingMatrix(mSkew);
	trans *= util::GetScalingMatrix(mScale);
	trans *= util::GetTranslationMatrix(-mOrigin);
	
	mGlobalMask.Transform(trans); // transform the global mask by the shape's transformation
}

void Sprite::CreateLocalMask() {
	mLocalMask.Clear(); // clear the current local mask
	
	unsigned int width = mWidth;
	unsigned int height = mHeight;
	
	if (mIsClipped == true) {
		width = mClippedWidth;
		height = mClippedHeight;
	}
	
	mLocalMask.AddPoint(glm::vec2(width,   0.0f));
	mLocalMask.AddPoint(glm::vec2(width, height));
	mLocalMask.AddPoint(glm::vec2( 0.0f, height));
}

RenderBatchData Sprite::Upload() {
	RenderBatchData rbd; // our batch data representing the shape
	
	// get the transformation matrix that represents the shape
	glm::mat3 trans = mTransformation;
	
	trans *= util::GetTranslationMatrix(mPosition - mOrigin);
	
	trans *= util::GetTranslationMatrix(mOrigin);
	trans *= util::GetRotationMatrix(mRotation);
	trans *= util::GetSkewingMatrix(mSkew);
	trans *= util::GetScalingMatrix(mScale);
	trans *= util::GetTranslationMatrix(-mOrigin);
	
	// get all points representing the shape including the origin (0, 0)
	std::vector<glm::vec2> points = {glm::vec2(0.0f, 0.0f), glm::vec2(mWidth, 0.0f),
			glm::vec2(mWidth, mHeight), glm::vec2(0.0f, mHeight)};
	
	GLuint texID = 0;
	float layer = 0.0f;
	std::vector<glm::vec2> texCoords = {glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f),
			glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f)};
	
	if (mCurrentFrame < mFrames.size()) {
		auto frame = mFrames.begin() + mCurrentFrame;
		texID = frame->mTexture.GetResource()->GetTextureID();
		layer = frame->mLayer;
		texCoords = frame->mTexCoords;
	}
	
	if (mIsClipped == true) { // if the sprite has been clipped
		points = mClip.GetAbsolutePoints(); // get the absolute points of the clip polygon
		points.insert(points.begin(), mClip.GetPosition()); // add the position of the clip polygon
		
		glm::vec2 topLeftTex(texCoords.at(0)); // get the top-left texture coords of the sprite rectangle
		glm::vec2 bottomRightTex(texCoords.at(2)); // get the bottom-right texture coords of the sprite rectangle
		texCoords.clear(); // clear the texture coords
		
		for (auto iter = points.begin(); iter != points.end(); ++iter) { // for all points in the clip polygon
			glm::vec2 ratio(iter->x / mWidth, iter->y / mHeight); // get the ratio of the current point to the sprite rectangle
			
			// calculate the x and y texture coordinates of the current point
			float texX = topLeftTex.x + ((bottomRightTex.x - topLeftTex.x) * ratio.x);
			float texY = topLeftTex.y + ((bottomRightTex.y - topLeftTex.y) * ratio.y);
			
			texCoords.emplace_back(std::move(texX), std::move(texY)); // add the texture coordinates
		}
	}
	
	for (unsigned int i = 0u; i < points.size(); ++i) { // for all points
		// convert the point into a vbo vertex
		glm::vec3 pos = trans * glm::vec3(points.at(i), 1.0f);
		
		VBOVertex vert;
		vert.mX = pos.x; vert.mY = pos.y; vert.mZ = mDepth;
		vert.mNX = 0.0f; vert.mNY = 0.0f; vert.mNZ = 1.0f;
		vert.mS = texCoords.at(i).x; vert.mT = texCoords.at(i).y; vert.mLayer = layer;
		vert.mR = mColour.x; vert.mG = mColour.y; vert.mB = mColour.z; vert.mA = mAlpha;
		vert.mTex = 1.0f;
		
		rbd.mVertData.push_back(vert); // add the vbo vertex to the batch data
	}
	
	for (unsigned int i = 0u; i < rbd.mVertData.size() - 2; ++i) { // for all vbo vertices in the batch data
		// add the appropiate vbo indices
		rbd.mIndData.emplace_back(i);
		rbd.mIndData.emplace_back(i + 1);
		rbd.mIndData.emplace_back(rbd.mVertData.size() - 1);
	}
	
	rbd.mTexID = texID; // set the texture id
	rbd.mTag = GetTag(); // set the renderables tag
	
	return rbd; // return the batch data
}
}
