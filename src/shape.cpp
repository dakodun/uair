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

#include "shape.hpp"

#include <iostream>
#include <algorithm>
#include <limits>

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
	using std::swap;
	
	Polygon::AddContour(contour, coordinateSpace); // add contour to shape using parent function
	
	// reset invalid indices, extra triangulation vertices, colours and tex coords
	std::vector< std::vector<VBOIndex> > indices = {{}, {}, {}, {}, {}};
	std::vector<Triangulate::Vertex> vertices;
	std::vector<glm::vec4> colourArrayExtra;
	
	swap(mIndices, indices);
	swap(mVertices, vertices);
	swap(mColourArrayExtra, colourArrayExtra);
	
	for (auto frame = mFrames.begin(); frame < mFrames.end(); ++frame) {
		std::vector<glm::vec2> texCoordsExtra;
		swap(frame->mTexCoordsExtra, texCoordsExtra);
	}
}

void Shape::AddContours(const std::vector<Contour>& contours, const CoordinateSpace& coordinateSpace) {
	using std::swap;
	
	Polygon::AddContours(contours, coordinateSpace); // add contours to shape using parent function
	
	// reset invalid indices, extra triangulation vertices, colours and tex coords
	std::vector< std::vector<VBOIndex> > indices = {{}, {}, {}, {}, {}};
	std::vector<Triangulate::Vertex> vertices;
	std::vector<glm::vec4> colourArrayExtra;
	
	swap(mIndices, indices);
	swap(mVertices, vertices);
	swap(mColourArrayExtra, colourArrayExtra);
	
	for (auto frame = mFrames.begin(); frame < mFrames.end(); ++frame) {
		std::vector<glm::vec2> texCoordsExtra;
		swap(frame->mTexCoordsExtra, texCoordsExtra);
	}
}

void Shape::Offset(float distance, const ClipperLib::JoinType& miterType, const double& miterLimit) {
	using std::swap;
	
	ClipperLib::ClipperOffset clipperOffset;
	ClipperLib::Paths outPaths;
	clipperOffset.MiterLimit = miterLimit;
	
	for (auto contour = mContours.begin(); contour != mContours.end(); ++contour) {
		clipperOffset.AddPath(static_cast<ClipperLib::Path>(*contour), miterType, ClipperLib::etClosedPolygon);
	}
	
    clipperOffset.Execute(outPaths, distance);
	
	std::vector<Contour> outContours;
	for (auto path = outPaths.begin(); path != outPaths.end(); ++path) {
		outContours.emplace_back(*path);
	}
	
	{ // clear invalid contours and bounds
		std::vector<Contour> contours;
		std::vector<glm::vec2> bounds;
		
		swap(mContours, contours);
		swap(mBounds, bounds);
	}
	
	AddContours(outContours); // add new offset contours to shape
}

void Shape::PositionContoursAtOrigin() {
	using std::swap;
	
	if (!mBounds.empty()) {
		glm::vec2 lowest = mBounds.at(0);
		
		if (!util::CompareFloats(lowest.x, util::Equals, 0.0f) || !util::CompareFloats(lowest.y, util::Equals, 0.0f)) { // if x or y value of lowest bounds is not at origin...
			std::vector<Contour> newContours;
			
			for (auto contour = mContours.begin(); contour != mContours.end(); ++contour) {
				std::vector<glm::vec2> newPoints = contour->GetPoints();
				for (auto point = newPoints.begin(); point != newPoints.end(); ++point) {
					*point -= lowest;
				}
				
				newContours.emplace_back(newPoints);
			}
			
			{ // clear invalid contours and bounds
				std::vector<Contour> contours;
				std::vector<glm::vec2> bounds;
				
				swap(mContours, contours);
				swap(mBounds, bounds);
			}
			
			AddContours(newContours); // add new offset contours to shape
		}
	}
}

void Shape::FromClipperPaths(const ClipperLib::Paths& clipperPaths) {
	using std::swap;
	
	{ // clear invalid contours and bounds
		std::vector<Contour> contours;
		std::vector<glm::vec2> bounds;
		
		swap(mContours, contours);
		swap(mBounds, bounds);
	}
	
	std::vector<Contour> contours;
	for (auto path = clipperPaths.begin(); path != clipperPaths.end(); ++path) { // for all clipper paths...
		contours.emplace_back(*path); // convert to contours and store
	}
	
	AddContours(contours); // add new contours to shape
}

Shape Shape::GetTransformed() const {
	Shape transformedShape = *this;
	std::vector<Contour> transformedContours;
	
	{ // create arrays of transformed vertices and vertex counts
		glm::mat3 transMat = mTransformation; // get the transform matrix for shape
		transMat *= util::GetTranslationMatrix(mPosition - mOrigin); // translate by position offset (take into account origin offset)
		
		transMat *= util::GetTranslationMatrix(mOrigin); // translate to origin...
		transMat *= util::GetRotationMatrix(mRotation); // ...rotate...
		transMat *= util::GetSkewingMatrix(mSkew); // ...skew...
		transMat *= util::GetScalingMatrix(mScale); // ...scale...
		transMat *= util::GetTranslationMatrix(-mOrigin); // ...and then translate back from origin
		
		for (auto contour = mContours.begin(); contour != mContours.end(); ++contour) { // for all contours...
			std::vector<glm::vec2> vertices;
			const std::vector<glm::vec2>& points = contour->GetPoints(); // get const reference to vertices in countour
			
			for (auto vertex = points.begin(); vertex != points.end(); ++vertex) { // for all vertices in contour...
				glm::vec3 pos = transMat * glm::vec3(*vertex, 1.0f); // get position of transformed vertex
				vertices.emplace_back(pos.x, pos.y); // add transformed vertex
			}
			
			transformedContours.emplace_back(vertices);
		}
	}
	
	transformedShape.mPosition = glm::vec2();
	transformedShape.mOrigin = glm::vec2();
	transformedShape.mTransformation = glm::mat3();
	transformedShape.mScale = glm::vec2(1.0f, 1.0f);
	transformedShape.mRotation = 0;
	transformedShape.mSkew = glm::vec2();
	
	transformedShape.mContours.clear();
	transformedShape.AddContours(transformedContours);
	
	return transformedShape;
}

std::string Shape::GetTag() const {
	return "Shape";
}

bool Shape::Process(float deltaTime) {
	if (mIsAnimated && !mFrames.empty()) { // if shape is animated and has animation frames...
		mAnimationTimer += deltaTime; // increment animation timer
		
		int currentFrame = mCurrentFrame; // get current animation frame
		float limit = mFrames.at(currentFrame).mAnimationSpeed;
		
		while (mAnimationTimer >= limit) { // while the animation timer eclipses the limit (speed)...
			mAnimationTimer -= limit; // remove the limit (speed) from the timer
			
			if (currentFrame == static_cast<int>(mAnimationEndFrame)) {
				if (mAnimationLoopCount != 0) { // if animation hasn't finished looping...
					if (mAnimationLoopCount > 0) { // if animation isn't looping indefinitely...
						--mAnimationLoopCount; // decrease loop counter by one
					}
					
					// reset to initial frame (we subtract one to account for the change that occurs later)
					currentFrame = mAnimationStartFrame - mAnimationDirection;
				}
				else { // otherwise if animation is finished...
					mIsAnimated = false; // set shape as static
					break;
				}
			}
			
			currentFrame = (currentFrame + mAnimationDirection) % mFrames.size(); // move the current frame one in the animation direction
			if (currentFrame < 0) {
				currentFrame = mFrames.size() - 1;
			}
			
			limit = mFrames.at(currentFrame).mAnimationSpeed;
		}
		
		if (mCurrentFrame != static_cast<unsigned int>(currentFrame)) {
			mCurrentFrame = currentFrame; // set current animation frame
			return true;
		}
	}
	
	return false;
}

void Shape::SetWindingRule(const WindingRule& windingRule) {
	using std::swap;
	
	if (mWindingRule != windingRule) { // if winding rule is not already set (prevents unneccesary retriangulation)...
		mWindingRule = windingRule; // set the winding rule
		
		// reset invalid indices ('filled' rendering style only), extra triangulation vertices, colours and tex coords
		std::vector<VBOIndex> indices;
		std::vector<Triangulate::Vertex> vertices;
		std::vector<glm::vec4> colourArrayExtra;
		
		swap(mIndices[4], indices);
		swap(mVertices, vertices);
		swap(mColourArrayExtra, colourArrayExtra);
		
		for (auto frame = mFrames.begin(); frame < mFrames.end(); ++frame) {
			std::vector<glm::vec2> texCoordsExtra;
			swap(frame->mTexCoordsExtra, texCoordsExtra);
		}
	}
}

void Shape::AddColour(const std::vector<glm::vec4>& colour) {
	mColourArray.insert(mColourArray.end(), colour.begin(), colour.end()); // add per-vertex colouring to store
	CalculateExtraColour(); // recalculate colouring for any extra triangulation vertices
}

void Shape::AddFrame(ResourcePtr<Texture> texture, const unsigned int& layer) {
	Texture* tex = texture.Get();
	
	if (tex) {
		if (layer < tex->GetDepth()) {
			const Texture::LayerData& texData = tex->GetData(layer);
			
			AnimationFrame frame; // create a new frame
			frame.mTexture = texture; // set the texture pointer of the frame
			frame.mMinST = glm::vec2(0.0f, 0.0f); frame.mMaxST = glm::vec2(texData.mSMax, texData.mTMax); // set the ST coordinate limits
			
			for (auto contour = mContours.begin(); contour != mContours.end(); ++contour) { // for all contours in the shape...
				CalculateTexCoords(contour->GetPoints(), {frame.mMinST, frame.mMaxST}, frame.mTexCoords); // calculate the texture coordinates for the contour for the current frame
			}
			
			CalculateExtraTexCoords(frame); // recalculate texture coords for any extra triangulation vertices
			frame.mLayer = layer;
			mFrames.push_back(std::move(frame));
		}
	}
}

void Shape::AddFrameCoords(ResourcePtr<Texture> texture, const unsigned int& layer, std::vector<glm::vec2> textureCoords) {
	Texture* tex = texture.Get();
	
	if (tex) {
		if (layer < tex->GetDepth()) {
			const Texture::LayerData& texData = tex->GetData(layer);
			
			AnimationFrame frame; // create a new frame
			frame.mTexture = texture; // set the texture pointer of the frame
			
			// calculate the scale between the non-padded and padded texture (if any)
			glm::vec2 scaleFactor = glm::vec2(texData.mWidth / static_cast<float>(tex->GetWidth()),
					texData.mHeight / static_cast<float>(tex->GetHeight()));
			
			if (!textureCoords.empty()) {
				// set the min and max to extreme values initially
				frame.mMinST = glm::vec2(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
				frame.mMaxST = glm::vec2(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());
				
				for (auto coord = textureCoords.begin(); coord != textureCoords.end(); ++coord) { // for all coords in the array...
					coord->x *= scaleFactor.x; coord->y *= scaleFactor.y; // scale the coord by the scale factor
					
					// update the min and max coord values
					frame.mMinST.x = std::min(frame.mMinST.x, coord->x); frame.mMinST.y = std::min(frame.mMinST.y, coord->y);
					frame.mMaxST.x = std::max(frame.mMaxST.x, coord->x); frame.mMaxST.y = std::max(frame.mMaxST.y, coord->y);
				}
			}
			
			frame.mTexCoords = textureCoords;
			
			CalculateExtraTexCoords(frame); // recalculate texture coords for any extra triangulation vertices
			frame.mLayer = layer;
			mFrames.push_back(std::move(frame));
		}
	}
}

void Shape::AddFrameRect(ResourcePtr<Texture> texture, const unsigned int& layer, const std::vector<glm::vec2>& textureRect) {
	Texture* tex = texture.Get();
	
	if (tex) {
		if (layer < tex->GetDepth()) {
			const Texture::LayerData& texData = tex->GetData(layer);
			
			// set default values for the texture ST coordinate bounds
			glm::vec2 min = glm::vec2(0.0f, 0.0f); // top-left of texture
			glm::vec2 max = glm::vec2(texData.mSMax, texData.mTMax); // bottom-right of texture
			
			if (textureRect.size() == 2) { // if a custom texture rect was supplied...
				// calculate the min and max (top-left and bottom-right) ST coordinates of the required portion of the texture
				min = glm::vec2(std::max(std::max(0.0f, textureRect.at(0).x / tex->GetWidth()), min.x),
						std::max(std::max(0.0f, textureRect.at(0).y / tex->GetHeight()), min.y));
				max = glm::vec2(std::min(std::min(texData.mSMax, textureRect.at(1).x / tex->GetWidth()), max.x),
						std::min(std::min(texData.mTMax, textureRect.at(1).y / tex->GetHeight()), max.y));
			}
			
			AnimationFrame frame; // create a new frame
			frame.mTexture = texture; // set the texture pointer of the frame
			frame.mMinST = min; frame.mMaxST = max; // set the ST coordinate limits
			
			for (auto contour = mContours.begin(); contour != mContours.end(); ++contour) { // for all contours in the shape...
				CalculateTexCoords(contour->GetPoints(), {frame.mMinST, frame.mMaxST}, frame.mTexCoords); // calculate the texture coordinates for the contour for the current frame
			}
			
			CalculateExtraTexCoords(frame); // recalculate texture coords for any extra triangulation vertices
			frame.mLayer = layer;
			mFrames.push_back(std::move(frame));
		}
	}
}

void Shape::AddFrameStrip(ResourcePtr<Texture> texture, const unsigned int& layer, const unsigned int& numFrames, const unsigned int& numPerRow,
		const unsigned int& numPerCol, const unsigned int& start, const glm::ivec2& spacing) {
	
	Texture* tex = texture.Get();
	
	if (tex) {
		if (layer < tex->GetDepth()) {
			const Texture::LayerData& texData = tex->GetData(layer); // get the data for the layer
			
			// calculate the dimensions of a single frame
			float frameWidth = texData.mWidth / static_cast<float>(numPerRow);
			float frameHeight = texData.mHeight / static_cast<float>(numPerCol);
			
			// calculate the dimensions of a single frame in texture coordinates, including the spacing between frames
			glm::vec2 increment = glm::vec2((frameWidth - spacing.x) / tex->GetWidth(), (frameHeight - spacing.y) / tex->GetHeight());
			glm::vec2 spacingIncrement = glm::vec2(static_cast<float>(spacing.x) / tex->GetWidth(), static_cast<float>(spacing.y) / tex->GetHeight());
			
			for (unsigned int i = start; i < numFrames + start; ++i) {
				unsigned int t = i / numPerRow; // the current row in the sheet
				unsigned int s = i % numPerRow; // the current column in the sheet
				
				// calculate the min and max (top-left and bottom-right) ST coordinates of the required portion of the texture
				glm::vec2 min = glm::vec2((s * increment.x) + (s * spacingIncrement.x), (t * increment.y) + (t * spacingIncrement.y)); // top-left of texture
				glm::vec2 max = glm::vec2(((s + 1) * increment.x) + (s * spacingIncrement.x), ((t + 1) * increment.y) + (t * spacingIncrement.y)); // bottom-right of texture
				
				AnimationFrame frame; // create a new frame
				frame.mTexture = texture; // set the texture pointer of the frame
				frame.mMinST = min; frame.mMaxST = max; // set the ST coordinate limits
				
				for (auto contour = mContours.begin(); contour != mContours.end(); ++contour) { // for all contours in the shape...
					CalculateTexCoords(contour->GetPoints(), {frame.mMinST, frame.mMaxST}, frame.mTexCoords); // calculate the texture coordinates for the contour for the current frame
				}
				
				CalculateExtraTexCoords(frame); // recalculate texture coords for any extra triangulation vertices
				frame.mLayer = layer;
				mFrames.push_back(std::move(frame));
			}
		}
	}
}

void Shape::AddFrame(Texture* texture, const unsigned int& layer) {
	AddFrame(ResourcePtr<Texture>(texture), layer); // add naked pointer to resource pointer and call parent function
}

void Shape::AddFrameCoords(Texture* texture, const unsigned int& layer, std::vector<glm::vec2> textureCoords) {
	AddFrameCoords(ResourcePtr<Texture>(texture), layer, textureCoords); // add naked pointer to resource pointer and call parent function
}

void Shape::AddFrameRect(Texture* texture, const unsigned int& layer, const std::vector<glm::vec2>& textureRect) {
	AddFrameRect(ResourcePtr<Texture>(texture), layer, textureRect); // add naked pointer to resource pointer and call parent function
}

void Shape::AddFrameStrip(Texture* texture, const unsigned int& layer, const unsigned int& numFrames, const unsigned int& numPerRow,
		const unsigned int& numPerCol, const unsigned int& start, const glm::ivec2& spacing) {
	
	AddFrameStrip(ResourcePtr<Texture>(texture), layer, numFrames, numPerRow, numPerCol, start, spacing); // add naked pointer to resource pointer and call parent function
}

AnimationFrame Shape::GetFrame() const {
	if (mCurrentFrame < mFrames.size()) { // if the current frame is valid (false if shape isn't animated)...
		return mFrames.at(mCurrentFrame); // return a copy of the current frame object
	}
	
	throw std::runtime_error("shape isn't animated (no frame to return)");
}

AnimationFrame Shape::GetFrame(const unsigned int& frame) const {
	if (frame < mFrames.size()) { // if requested frame is valid...
		return mFrames.at(frame); // return a copy of the requested frame object
	}
	
	throw std::runtime_error("requested frame is invalid");
}

void Shape::RemoveFrame() {
	// reset the animation to default and remove any existing frames
	SetAnimation();
	mFrames.clear();
}

void Shape::RemoveFrame(const unsigned int& frame) {
	// [TODO] dummy function
	//   remove selected frame if valid
	//   adjust animation if necessary
}

void Shape::SetAnimation() {
	mIsAnimated = false; // set shape as static
	mAnimationDirection = 0;
	
	mAnimationStartFrame = 0u;
	mAnimationEndFrame = 0u;
	
	mAnimationLoopCount = 0;
	mAnimationLoopMax = 0;
	
	mAnimationTimer = 0.0f;
	
	for (auto frameIter = mFrames.begin(); frameIter != mFrames.end(); ++frameIter) {
		frameIter->mAnimationSpeed = 0.0167f;
	}
}

void Shape::SetAnimation(std::vector<float> speeds, const unsigned int& start, const unsigned int& end,
		const int& direction, const int& loops) {
	
	if (!mFrames.empty()) {
		mAnimationStartFrame = std::min(static_cast<unsigned int>(mFrames.size() - 1u), start); // set the start frame
		mAnimationEndFrame = std::min(static_cast<unsigned int>(mFrames.size() - 1u), end); // set the end frame
	}
	else {
		mAnimationStartFrame = 0u;
		mAnimationEndFrame = 0u;
	}
	
	if (mAnimationStartFrame != mAnimationEndFrame) {
		mIsAnimated = true; // set shape as animated
		mAnimationDirection = util::SignOf(direction); // set direction of animation
		
		//
			unsigned int frameCount = 0u;
			if (mAnimationStartFrame < mAnimationEndFrame) {
				frameCount = (mAnimationEndFrame + 1u) - mAnimationStartFrame;
			}
			else {
				frameCount = (mFrames.size() - mAnimationStartFrame) + (mAnimationEndFrame + 1u);
			}
			
			float speedPad = 0.0167f;
			if (!speeds.empty()) {
				speedPad = speeds.back();
			}
			
			speeds.resize(frameCount, speedPad);
			
			for (unsigned int i = mAnimationStartFrame, j = 0u; ; i += mAnimationDirection, ++j) {
				i = i % mFrames.size();
				
				mFrames.at(i).mAnimationSpeed = speeds.at(j);
				
				if (i == mAnimationEndFrame) {
					break;
				}
				else if (i == 0u && mAnimationDirection < 0) {
					i = mFrames.size();
				}
			}
		//
		
		mAnimationLoopMax = loops; // set the number of loops
		mAnimationLoopCount = mAnimationLoopMax;
		
		mAnimationTimer = 0.0f; // reset the current animation timer
		mCurrentFrame = mAnimationStartFrame; // set the current frame to the initial frame
	}
	else {
		mIsAnimated = false;
	}
}

void Shape::ResetAnimation() {
	mAnimationLoopCount = mAnimationLoopMax;
	
	mAnimationTimer = 0.0f; // reset the current animation timer
	mCurrentFrame = mAnimationStartFrame; // set the current frame to the initial frame
}

bool Shape::IsAnimated() const {
	return mIsAnimated;
}

void Shape::SetCurrentFrame(const unsigned int& frame) {
	size_t frameCount = mFrames.size(); // get the frame count
	
	if (frameCount > 0) { // if shape has any animation frames...
		mAnimationTimer = 0.0f; // reset the current animation timer
		mCurrentFrame = std::min(frameCount - 1, static_cast<size_t>(frame)); // set the end frame
	}
}

unsigned int Shape::GetCurrentFrame() const {
	return mCurrentFrame;
}

unsigned int Shape::GetFrameCount() {
	return mFrames.size();
}

void Shape::SetWrapMode(const Texture::Coordinate& coord, const WrapMode& mode) {
	unsigned int index = static_cast<unsigned int>(coord);
	mWrapModes.at(index) = mode;
	
	mWrapModeBitmask = 0u | static_cast<unsigned int>(mWrapModes.at(0u)) |
			(static_cast<unsigned int>(mWrapModes.at(1u)) * 2u);
}

Shape::WrapMode Shape::GetWrapMode(const Texture::Coordinate& coord) const {
	unsigned int index = static_cast<unsigned int>(coord);
	return mWrapModes.at(index);
}

std::list<RenderBatchData> Shape::Upload() {
	RenderBatchData rbd; // rendering data struct
	std::vector<unsigned int> vertCounts; // count of vertices in each contour
	std::vector<glm::vec2> vertices; // all transformed vertices in shape (from each contour)
	
	{ // create arrays of transformed vertices and vertex counts
		for (auto contour = mContours.begin(); contour != mContours.end(); ++contour) { // for all contours...
			const std::vector<glm::vec2>& points = contour->GetPoints(); // get const reference to vertices in countour
			vertices.insert(vertices.end(), points.begin(), points.end());
			vertCounts.push_back(points.size()); // store count of vertices
		}
	}
	
	size_t indexType = 0u; // type of indices for current render mode
	GLenum renderMode = mRenderMode; // make a copy of the current render mode
	if (mIndices.size() >= 5) { // if index array is valid...
		switch (mRenderMode) { // depending on render mode...
			case 0 : { // GL_POINTS
				indexType = 0u; // set index type to match render mode
				
				if (mIndices.at(indexType).empty()) { // if indices haven't been calculated yet...
					unsigned int count = 0u; // current index count
					for (auto iter = vertCounts.begin(); iter != vertCounts.end(); ++iter) { // for all vertex counts...
						for (unsigned int i = 0u; i < *iter; ++i) { // for 0 to vertex count...
							mIndices.at(indexType).emplace_back(i + count); // add index to array
						}
						
						count += *iter; // increment index count
					}
				}
				
				break;
			}
			case 1 : { // GL_LINES
				indexType = 1u; // set index type to match render mode
				
				if (mIndices.at(indexType).empty()) { // if indices haven't been calculated yet...
					unsigned int count = 0u; // current index count
					for (auto iter = vertCounts.begin(); iter != vertCounts.end(); ++iter) { // for all vertex counts...
						for (unsigned int i = 0u; i < *iter - 1; i += 2) { // for 0 to vertex count (2 at a time to create seperated lines)...
							mIndices.at(indexType).emplace_back(i + count); // add current index to array
							mIndices.at(indexType).emplace_back((i + 1) + count); // add next index to array
						}
						
						count += *iter; // increment index count
					}
				}
				
				break;
			}
			case 2 : { // GL_LINE_LOOP
				indexType = 2u; // set index type to match render mode
				renderMode = GL_LINES; // set render mode to lines (we'll loop manually)
				
				if (mIndices.at(indexType).empty()) { // if indices haven't been calculated yet...
					unsigned int count = 0u; // current index count
					for (auto iter = vertCounts.begin(); iter != vertCounts.end(); ++iter) { // for all vertex counts...
						for (unsigned int i = 0u; i < *iter - 1; ++i) { // for 0 to vertex count...
							mIndices.at(indexType).emplace_back(i + count); // add current index to array
							mIndices.at(indexType).emplace_back((i + 1) + count); // add next index to array
						}
						
						mIndices.at(indexType).emplace_back((*iter - 1) + count); // add second last index
						mIndices.at(indexType).emplace_back(count); // add last index to close the loop
						count += *iter; // increment index count
					}
				}
				
				break;
			}
			case 3 : { // GL_LINE_STRIP
				indexType = 3u; // set index type to match render mode
				renderMode = GL_LINES; // set render mode to lines (we'll strip manually)
				
				if (mIndices.at(indexType).empty()) { // if indices haven't been calculated yet...
					unsigned int count = 0u; // current index count
					for (auto iter = vertCounts.begin(); iter != vertCounts.end(); ++iter) { // for all vertex counts...
						for (unsigned int i = 0u; i < *iter - 1; ++i) { // for 0 to vertex count...
							mIndices.at(indexType).emplace_back(i + count); // add current index to array
							mIndices.at(indexType).emplace_back((i + 1) + count); // add next index to array
						}
						
						count += *iter; // increment index count
					}
				}
				
				break;
			}
			case 4 : // GL_TRIANGLES
			case 5 : // GL_TRIANGLE_STRIP
			case 6 : { // GL_TRIANGLE_FAN
				indexType = 4u; // set index type to match render mode
				renderMode = GL_TRIANGLES; // set render mode to triangles
				
				if (mIndices.at(indexType).empty() && mVertices.empty()) { // if indices and vertices haven't been calculated yet...
					Triangulate triangulate; // get a triangulator instance
					
					size_t offset = 0u;
					for (auto count = vertCounts.begin(); count != vertCounts.end(); ++count) {
						std::vector<glm::vec2> contour(vertices.begin() + offset, vertices.begin() + offset + *count);
						offset += *count;
						
						triangulate.AddContour(contour);
					}
					
					Triangulate::Result result = triangulate.Process(mWindingRule); // triangulate using current winding rule and store the results
					mVertices = result.first; // store any new vertices
					mIndices.at(indexType) = result.second; // set the indices for regular and new vertices
				}
				
				CalculateExtraColour(); // calculate per-vertex colouring for new vertices
				
				for (auto frame = mFrames.begin(); frame != mFrames.end(); ++frame) { // for all animation frames...
					CalculateExtraTexCoords(*frame); // calculate texture coords for new vertices
				}
				
				break;
			}
			default :
				break;
		}
	}
	
	{
		std::vector<glm::vec2> verticesTransformed;
		std::vector<glm::vec2> texCoords;
		std::vector<glm::vec4> colours;
		
		{
			glm::mat3 transMat = mTransformation; // get the transform matrix for shape
			transMat *= util::GetTranslationMatrix(mPosition - mOrigin); // translate by position offset (take into account origin offset)
			
			transMat *= util::GetTranslationMatrix(mOrigin); // translate to origin...
			transMat *= util::GetRotationMatrix(mRotation); // ...rotate...
			transMat *= util::GetSkewingMatrix(mSkew); // ...skew...
			transMat *= util::GetScalingMatrix(mScale); // ...scale...
			transMat *= util::GetTranslationMatrix(-mOrigin); // ...and then translate back from origin
			
			for (auto vertex = vertices.begin(); vertex != vertices.end(); ++vertex) {
				glm::vec3 pos = transMat * glm::vec3(*vertex, 1.0f); // get position of transformed vertex
				verticesTransformed.emplace_back(pos.x, pos.y); // add transformed vertex
			}
			
			for (auto vertex = mVertices.begin(); vertex != mVertices.end(); ++vertex) {
				glm::vec3 pos = transMat * glm::vec3(vertex->mPoint, 1.0f); // get position of transformed vertex
				verticesTransformed.emplace_back(pos.x, pos.y); // add transformed vertex
			}
			
			if (mCurrentFrame < mFrames.size()) {
				texCoords.insert(texCoords.end(), mFrames.at(mCurrentFrame).mTexCoords.begin(), mFrames.at(mCurrentFrame).mTexCoords.end());
				texCoords.insert(texCoords.end(), mFrames.at(mCurrentFrame).mTexCoordsExtra.begin(), mFrames.at(mCurrentFrame).mTexCoordsExtra.end());
			}
			
			colours.insert(colours.end(), mColourArray.begin(), mColourArray.end());
			colours.insert(colours.end(), mColourArrayExtra.begin(), mColourArrayExtra.end());
		}
		
		CreateVBOVertices(rbd, verticesTransformed, texCoords, colours);
	}
	
	rbd.mIndexData.insert(rbd.mIndexData.end(), mIndices.at(indexType).begin(), mIndices.at(indexType).end()); // copy indices into render batch data
	if (util::CompareFloats(mScale.x, util::LessThan, 0.0f) ^ util::CompareFloats(mScale.y, util::LessThan, 0.0f)) { // if ONE of the x or y scales is negative...
		std::reverse(rbd.mIndexData.begin(), rbd.mIndexData.end());
	}
	
	rbd.mTextureID = 0; // set texture id
	rbd.mRenderMode = renderMode; // set render mode
	rbd.mTag = GetTag(); // set renderables tag
	
	rbd.mShader = GetShader();
	
	if (mCurrentFrame < mFrames.size()) { // if shape is textured...
		if (mFrames.at(mCurrentFrame).mTexture) {
			rbd.mTextureID = mFrames.at(mCurrentFrame).mTexture->GetTextureID(); // set texture id
		}
	}
	
	return {rbd}; // return batch data
}

void Shape::CalculateTexCoords(const std::vector<glm::vec2>& points, const std::vector<glm::vec2>& texRect, std::vector<glm::vec2>& texCoordsLocation) {
	if (mBounds.size() > 1) { // if valid bounds have been set up for the shape...
		for (auto point = points.begin(); point != points.end(); ++point) { // for all points in the shape...
			// get the ratio of the current point in regards to the bounds
			glm::vec2 ratio((point->x - mBounds.at(0).x) / (mBounds.at(1).x - mBounds.at(0).x),
					(point->y - mBounds.at(0).y) / (mBounds.at(1).y - mBounds.at(0).y));
			
			// calculate the texture coordinates in regards to the ratio of the current point and the texture rectangle of the current frame's texture
			texCoordsLocation.emplace_back(((1 - ratio.x) * texRect.at(0).x) + (ratio.x * texRect.at(1).x),
					((1 - ratio.y) * texRect.at(0).y) + (ratio.y * texRect.at(1).y));
		}
	}
}

void Shape::CreateVBOVertices(RenderBatchData& batchData, const std::vector<glm::vec2>& vertices, std::vector<glm::vec2> texCoords,
		std::vector<glm::vec4> colours) {
	
	GLubyte texAvailable = 0u; // is shape textured
	GLushort texLayer = 0u; // layer of current frame texture
	glm::vec2 minST(0.0f, 0.0f); // the top left texture coordinate for the current frame
	glm::vec2 maxST(1.0f, 1.0f); // the bottom right texture coordinate for the current frame
	glm::vec2 textureSize(0.0f, 0.0f); // the size of the texture for the current frame
	
	if (mCurrentFrame < mFrames.size()) { // if shape is textured...
		AnimationFrame& frame = mFrames.at(mCurrentFrame);
		
		texAvailable = 1u; // shape is textured
		texLayer = std::min(frame.mLayer, 65535u); // store layer of current frame texture
		minST.x = std::min(frame.mMinST.x, 1.0f); minST.y = std::min(frame.mMinST.y, 1.0f);
		maxST.x = std::min(frame.mMaxST.x, 1.0f); maxST.y = std::min(frame.mMaxST.y, 1.0f);
		
		if (frame.mTexture) { // if the current frame has a valid texture...
			// calculate the size of the texture for the uccrent frame by
			// multiplying the total size of the texture by the texture coords
			// (max - min) implies the rect representing the frame
			textureSize.x = frame.mTexture->GetWidth() * (maxST.x - minST.x);
			textureSize.y = frame.mTexture->GetHeight() * (maxST.y - minST.y);
		}
	}
	
	int difference = vertices.size() - texCoords.size(); // difference in count of texture coords
	if (difference > 0) { // if a difference exists...
		texCoords.insert(texCoords.end(), difference, glm::vec2(0.0f, 0.0f)); // add default texture coords to array
	}
	
	difference = vertices.size() - colours.size(); // difference in count of colours
	if (difference > 0) { // if a difference exists...
		colours.insert(colours.end(), difference, glm::vec4(mColour, mAlpha)); // add default colours to array
	}
	
	for (unsigned int i = 0; i < vertices.size(); ++i) { // for all vertices...
		VBOVertex vert; // create vertex suitable for rendering
		vert.mX = vertices.at(i).x; vert.mY = vertices.at(i).y; vert.mZ = mDepth;
		
		vert.mNormal = 0u;
		vert.mNormal = vert.mNormal | (0u << 30); // padding
		vert.mNormal = vert.mNormal | (1023u << 20); // z
		vert.mNormal = vert.mNormal | (0u << 10); // y
		vert.mNormal = vert.mNormal | (0u << 0); // x
		
		vert.mS = texCoords.at(i).x * 65535u; vert.mT = (1.0f - texCoords.at(i).y) * 65535u;
		
		vert.mLWTT = 0u;
		vert.mLWTT = vert.mLWTT | (texAvailable << 30); // "is textured" flag
		vert.mLWTT = vert.mLWTT | (0u << 20); // render type (shape)
		vert.mLWTT = vert.mLWTT | (mWrapModeBitmask << 10); // wrap mode bit mask
		vert.mLWTT = vert.mLWTT | (texLayer << 0); // layer (z coordinate of texture coordinates)
		
		vert.mMinS = minST.x * 65535u; vert.mMinT = minST.y * 65535u;
		vert.mMaxS = maxST.x * 65535u; vert.mMaxT = maxST.y * 65535u;
		
		vert.mR = colours.at(i).x * 255u; vert.mG = colours.at(i).y * 255u;
		vert.mB = colours.at(i).z * 255u; vert.mA = colours.at(i).w * 255u;
		
		glm::vec2 scale(0.0f, 0.0f);
		if (textureSize.x != 0 && textureSize.y != 0 &&
				maxST.x - minST.x != 0 && maxST.y - minST.y != 0) { // if texture and coordinates are valid...
			
			// calculate the size of the shape (local bounding box)
			glm::vec2 shapeSize;
			std::vector<glm::vec2> bbox = GetLocalBoundingBox();
			if (bbox.size() == 4) {
				shapeSize = bbox.at(2) - bbox.at(0);
			}
			
			// rescale the maximum t texture coordinate
			glm::vec2 newMax(maxST.x * (shapeSize.x / textureSize.x),
					maxST.y * (shapeSize.y / textureSize.y));
			
			// calculate the scale factor to move texture coordinates into the new range
			scale = glm::vec2((newMax.x - minST.x) / (maxST.x - minST.x),
					(newMax.y - minST.y) / (maxST.y - minST.y));
		}
		
		vert.mScaleS = scale.x; vert.mScaleT = scale.y;
		
		batchData.mVertexData.push_back(vert); // add rendering vertex to rendering data
	}
}

void Shape::CalculateExtraColour() {
	mColourArrayExtra.clear(); // clear any existing colours
	
	for (auto vertex = mVertices.begin(); vertex != mVertices.end(); ++vertex) { // for all extra vertices...
		glm::vec4 colour; // new colour
		
		for (unsigned int i = 0u; i < vertex->mNeighbourIndices.size(); ++i) { // for all neighbouring vertices...
			unsigned int index = vertex->mNeighbourIndices.at(i); // get neighbouring index
			
			if (index >= mColourArray.size()) { // if the neighbouring index is NOT a regular vertex... 
				index -= mColourArray.size(); // adjust the neighbouring index
				
				if (index < mColourArrayExtra.size()) { // if the neighbouring index is valid...
					colour += mColourArrayExtra.at(index) * vertex->mNeighbourWeights.at(i); // add weighted colour (and alpha)
				}
				else { // otherwise neighbouring index is not valid...
					colour += glm::vec4(mColour, mAlpha) * vertex->mNeighbourWeights.at(i); // add standard colour (and alpha)
				}
			}
			else { // otherwise neighbouring index is regular...
				colour += mColourArray.at(index) * vertex->mNeighbourWeights.at(i); // add weighted colour (and alpha)
			}
		}
		
		mColourArrayExtra.push_back(std::move(colour)); // add new colour to array
	}
}

void Shape::CalculateExtraTexCoords(AnimationFrame& frame) {
	frame.mTexCoordsExtra.clear(); // clear any existing extra texture coords
	
	for (auto vertex = mVertices.begin(); vertex != mVertices.end(); ++vertex) { // for all extra vertices...
		glm::vec2 texCoord; // new texture coord
		
		for (unsigned int i = 0u; i < vertex->mNeighbourIndices.size(); ++i) { // for all neighbouring vertices...
			unsigned int index = vertex->mNeighbourIndices.at(i); // get the neighbouring index
			
			if (index >= frame.mTexCoords.size()) { // if the neighbouring index is NOT a regular vertex...
				index -= frame.mTexCoords.size(); // adjust the neighbouring index
				
				if (index < frame.mTexCoordsExtra.size()) { // if the neighbouring index is valid...
					texCoord += frame.mTexCoordsExtra.at(index) * vertex->mNeighbourWeights.at(i); // add the weighted texture coordinate
				} // otherwise assumed default
			}
			else { // otherwise neighbouring index is regular...
				texCoord += frame.mTexCoords.at(index) * vertex->mNeighbourWeights.at(i); // add the weighted texture coordinate
			}
		}
		
		frame.mTexCoordsExtra.push_back(std::move(texCoord)); // add new texture coord to array
	}
}
}
