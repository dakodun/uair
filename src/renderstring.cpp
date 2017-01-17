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

#include "renderstring.hpp"

#include <iostream>
#include <algorithm>

#include "game.hpp"
#include "util.hpp"
#include "shape.hpp"

namespace uair {
RenderString::RenderString(ResourcePtr<Font> font, const unsigned int& size, const std::u16string& text) :
		mFont(font), mSize(size), mAppendString(text) {
	
	if (mFont) { // if we have a valid font...
		mRBDData.mTextureID = mFont->GetTextureID(); // update the texture id of the batch data
		
		float fontSize = static_cast<float>(mFont->GetFontSize()); // get the size of the font
		mTextScale = glm::vec2(mSize / fontSize, mSize / fontSize); // calculate the scale factor for the render string
	}
	
	UpdateRenderString(); // ensure all characters are ready to be rendered
	
	mRBDData.mTag = GetTag(); // update the tag of the batch data
}

RenderString::RenderString(Font* font, const unsigned int& size, const std::u16string& text) : mSize(size),
		mAppendString(text) {
	
	mFont.Set(font); // set the resource pointer
	
	if (mFont) { // if we have a valid font...
		mRBDData.mTextureID = mFont->GetTextureID(); // update the texture id of the batch data
		
		float fontSize = static_cast<float>(mFont->GetFontSize()); // get the size of the font
		mTextScale = glm::vec2(mSize / fontSize, mSize / fontSize); // calculate the scale factor for the render string
	}
	
	UpdateRenderString(); // ensure all characters are ready to be rendered
	
	mRBDData.mTag = GetTag(); // update the tag of the batch data
}

std::string RenderString::GetTag() const {
	return "RenderString";
}

void RenderString::SetFont(ResourcePtr<Font> font) {
	mFont = font; // update the renderstring's font
	
	if (mFont) { // if we have a valid font...
		mRBDData.mTextureID = mFont->GetTextureID(); // update the texture id of the batch data
		
		float fontSize = static_cast<float>(mFont->GetFontSize()); // get the size of the font
		mTextScale = glm::vec2(mSize / fontSize, mSize / fontSize); // calculate the scale factor for the render string
	}
	
	// set the entire string as the string to be appended (to ensure all render characters are recreated)
	mAppendString = mString + mAppendString;
	mString = u"";
	
	mRenderChars.clear(); // remove any existing render characters
	
	// clear the bounding box and bounds
	mTopLeft = glm::vec2(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	mBottomRight = glm::vec2(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());
	mLocalBoundingBoxFull.clear();
	mLocalBoundingBox.clear();
	
	UpdateRenderString(); // ensure all characters are ready to be rendered
	
	// remove all existing render data
	mCharsRendered = 0u;
	mRBDData.mVertexData.clear();
	mRBDData.mIndexData.clear();
}

void RenderString::SetFont(Font* font) {
	mFont.Set(font); // update the renderstring's font
	
	if (mFont) { // if we have a valid font...
		mRBDData.mTextureID = mFont->GetTextureID(); // update the texture id of the batch data
		
		float fontSize = static_cast<float>(mFont->GetFontSize()); // get the size of the font
		mTextScale = glm::vec2(mSize / fontSize, mSize / fontSize); // calculate the scale factor for the render string
	}
	
	// set the entire string as the string to be appended (to ensure all render characters are recreated)
	mAppendString = mString + mAppendString;
	mString = u"";
	
	mRenderChars.clear(); // remove any existing render characters
	
	// clear the bounding box and bounds
	mTopLeft = glm::vec2(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	mBottomRight = glm::vec2(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());
	mLocalBoundingBoxFull.clear();
	mLocalBoundingBox.clear();
	
	UpdateRenderString(); // ensure all characters are ready to be rendered
	
	// remove all existing render data
	mCharsRendered = 0u;
	mRBDData.mVertexData.clear();
	mRBDData.mIndexData.clear();
}

void RenderString::SetText(const std::u16string& newText) {
	// set the new text to be appended and remove the existing string
	mAppendString = newText;
	mString = u"";
	
	mRenderChars.clear(); // remove any existing render characters
	
	// clear the bounding box and bounds
	mTopLeft = glm::vec2(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	mBottomRight = glm::vec2(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());
	mLocalBoundingBoxFull.clear();
	mLocalBoundingBox.clear();
	
	UpdateRenderString(); // ensure all characters are ready to be rendered
	
	// remove all existing render data
	mCharsRendered = 0u;
	mRBDData.mVertexData.clear();
	mRBDData.mIndexData.clear();
}

void RenderString::AddText(const std::u16string& newText) {
	mAppendString += newText; // add the new string to the end of the text to be appended
	
	UpdateRenderString(); // ensure all characters are ready to be rendered
}

void RenderString::AddText(const char16_t& newChar) {
	mAppendString += newChar; // add the new character to the end of the text to be appended
	
	UpdateRenderString(); // ensure all characters are ready to be rendered
}

bool RenderString::RemoveText(const unsigned int& numChars) {
	if (mString.empty() && mAppendString.empty()) { // if there is nothing to remove...
		return false; // indicate nothing was removed
	}
	
	// if the string to be appended is longer than the number of characters we wish to remove...
	if (mAppendString.size() > numChars) {
		mAppendString.erase(mAppendString.size() - numChars, numChars); // remove characters from the end of the string to be appended
	}
	else { // otherwise we need to remove some of the existing string too...
		mAppendString = u""; // clear all of the text to be appended
		unsigned int removeCount = numChars - mAppendString.size();
		
		// if the existing string is longer than the number of characters we wish to remove...
		if (mString.size() > removeCount && mRenderChars.size() > removeCount) {
			mString.erase(mString.size() - removeCount, removeCount); // remove characters from the end of the existing string
			mRenderChars.resize(mRenderChars.size() - removeCount); // remove the matching render characters
			
			// retrieve the bounds from the latest render character the was added
			mTopLeft = mRenderChars.back().mTopLeft;
			mBottomRight = mRenderChars.back().mBottomRight;
			
			// recreate the full-sized local bounding box
			glm::vec2 dimensions = mBottomRight - mTopLeft;
			mLocalBoundingBoxFull.clear();
			mLocalBoundingBoxFull.emplace_back(0.0f, 0.0f);
			mLocalBoundingBoxFull.emplace_back(dimensions.x, 0.0f);
			mLocalBoundingBoxFull.emplace_back(dimensions.x, dimensions.y);
			mLocalBoundingBoxFull.emplace_back(0.0f, dimensions.y);
			
			mLocalBoundingBox.clear(); // clear the scaled local bounding box
			UpdateRenderString(); // create the scaled local bounding box and the global bounding box
			
			if (mCharsRendered > mRenderChars.size()) { // if we've removed some characters that were already rendered...
				mCharsRendered = mRenderChars.size(); // trim the render characters down
				
				// remove the associated vertices (batches of 4) and indices (batches of 6)
				mRBDData.mVertexData.resize(mCharsRendered * 4);
				mRBDData.mIndexData.resize(mCharsRendered * 6);
			}
		}
		else { // otherwise we can just reset everything back to default...
			mString = u""; // clear all of the exisiting text
			
			mRenderChars.clear(); // remove all generated render characters
			
			// reset all bounds and bounding boxes back to default
			mTopLeft = glm::vec2(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
			mBottomRight = glm::vec2(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());
			mLocalBoundingBoxFull.clear();
			mLocalBoundingBox.clear();
			
			// remove all existing render data
			mCharsRendered = 0u;
			mRBDData.mVertexData.clear();
			mRBDData.mIndexData.clear();
		}
	}
	
	return true; // indicate something was removed
}

std::u16string RenderString::GetText() const {
	// return the full string (including characters that haven't been prepared for rendering yet)
	return mString + mAppendString;
}

void RenderString::SetSize(const unsigned int& size) {
	mSize = size;
	
	if (mFont) { // if we have a valid font...
		float fontSize = static_cast<float>(mFont->GetFontSize()); // get the size of the font
		mTextScale = glm::vec2(mSize / fontSize, mSize / fontSize); // calculate the scale factor for the render string
	}
	
	mLocalBoundingBox.clear(); // clear the scaled local bounding box
	
	UpdateRenderString(); // ensure all characters are ready to be rendered
}

void RenderString::UpdateGlobalBoundingBox() {
	// get the transformation matrix that is applied to the render string
	glm::mat3 trans = mTransformation;
	
	trans *= util::GetTranslationMatrix(mPosition - mOrigin);
	
	trans *= util::GetTranslationMatrix(mOrigin);
	trans *= util::GetRotationMatrix(mRotation);
	trans *= util::GetSkewingMatrix(mSkew);
	trans *= util::GetScalingMatrix(mScale);
	trans *= util::GetTranslationMatrix(-mOrigin);
	
	mGlobalBoundingBox.clear(); // remove the existing global blounding box
	
	// for every point in the local bounding box...
	for (auto iter = mLocalBoundingBox.begin(); iter != mLocalBoundingBox.end(); ++iter) {
		glm::vec3 pos = trans * glm::vec3(*iter, 1.0f); // transform it by the transformation matrix
		mGlobalBoundingBox.emplace_back(pos.x, pos.y);
	}
}

void RenderString::UpdateGlobalMask() {
	if (mFont) { // if we have a valid font...
		float height = mFont->GetBearingMax().y * mTextScale.y;
		glm::mat3 trans = mTransformation;
		
		trans *= util::GetTranslationMatrix(mPosition - mOrigin);
		
		trans *= util::GetTranslationMatrix(mOrigin);
		trans *= util::GetRotationMatrix(mRotation);
		trans *= util::GetSkewingMatrix(mSkew);
		trans *= util::GetScalingMatrix(mScale);
		trans *= util::GetTranslationMatrix(-mOrigin);
		
		mGlobalMask.clear();
		for (auto iter = mLocalMask.begin(); iter != mLocalMask.end(); ++iter) {
			glm::vec3 pos = trans * glm::vec3(*iter, 1.0f);
			mGlobalMask.emplace_back(pos.x, pos.y - height);
		}
	}
}

void RenderString::CreateLocalMask() {
	mLocalMask.clear();
	
	if (mLocalBoundingBox.empty()) { // if we haven't scaled the bounding box yet...
		mLocalBoundingBox = mLocalBoundingBoxFull;
		
		for (auto vertex = mLocalBoundingBox.begin(); vertex != mLocalBoundingBox.end(); ++vertex) {
			vertex->x *= mTextScale.x;
			vertex->y *= mTextScale.y;
		}
	}
	
	if (mLocalBoundingBox.size() > 3) {
		mLocalMask.insert(mLocalMask.end(), mLocalBoundingBox.begin(), mLocalBoundingBox.end());
	}
}

std::list<RenderBatchData> RenderString::Upload() {
	UpdateRenderString(); // ensure all characters are ready to be rendered
	
	// for all newly added characters that haven't yet been rendered...
	for (auto renderChar = mRenderChars.begin() + mCharsRendered; renderChar != mRenderChars.end(); ++renderChar) {
		try {
			Shape& charShape = renderChar->mCharacter; // get the shape that represents the render character
			AnimationFrame frame = charShape.GetFrame(); // get the current animation frame (should only be one)
			
			std::vector<glm::vec2> vertices;
			
			const std::vector<Contour>& contours = charShape.GetContours(); // get reference to render character shape's contours
			if (!contours.empty()) { // if there is at least one contour...
				vertices = contours.front().GetPoints(); // get the points that form the first contour (should only be one)
			}
			
			std::vector<glm::vec2> texCoords = frame.mTexCoords; // get the texture coords for the current animation frame
			
			if (vertices.size() != 4u || texCoords.size() != 4u) { // if the shape isn't a valid quad or have valid texture coordinates...
				// add a degenerate shape so it doesn't interfere with future characters
				vertices = {glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f)};
				texCoords = {glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f)};
			}
			
			// create a vertex that represents the untransformed character
			for (unsigned int i = 0u; i < 4u; ++i) {
				VBOVertex vert; // create vertex suitable for rendering
				vert.mX = vertices.at(i).x + charShape.GetPosition().x; vert.mY = vertices.at(i).y + charShape.GetPosition().y;
				
				vert.mNormal = 0u;
				vert.mNormal = vert.mNormal | (0u << 30); // padding
				vert.mNormal = vert.mNormal | (1023u << 20); // z
				vert.mNormal = vert.mNormal | (0u << 10); // y
				vert.mNormal = vert.mNormal | (0u << 0); // x
				
				vert.mS = texCoords.at(i).x * 65535u; vert.mT = (1.0f - texCoords.at(i).y) * 65535u;
				
				vert.mLWTT = 0u;
				vert.mLWTT = vert.mLWTT | (1u << 30); // "is textured" flag
				vert.mLWTT = vert.mLWTT | (1u << 20); // render type (shape)
				vert.mLWTT = vert.mLWTT | (0u << 10); // wrap mode bit mask
				vert.mLWTT = vert.mLWTT | (frame.mLayer << 0); // layer (z coordinate of texture coordinates)
				
				mRBDData.mVertexData.push_back(std::move(vert));
			}
		} catch(std::exception& e) {
			// add a degenerate vertex so it doesn't interfere with future characters
			for (unsigned int i = 0u; i < 4u; ++i) {
				VBOVertex vert; // create vertex suitable for rendering
				vert.mX = 0.0f; vert.mY = 0.0f;
				
				vert.mNormal = 0u;
				vert.mNormal = vert.mNormal | (0u << 30); // padding
				vert.mNormal = vert.mNormal | (1023u << 20); // z
				vert.mNormal = vert.mNormal | (0u << 10); // y
				vert.mNormal = vert.mNormal | (0u << 0); // x
				
				vert.mS = 0u; vert.mT = 65535u;
				
				vert.mLWTT = 0u;
				vert.mLWTT = vert.mLWTT | (1u << 30); // "is textured" flag
				vert.mLWTT = vert.mLWTT | (1u << 20); // render type (shape)
				vert.mLWTT = vert.mLWTT | (0u << 10); // wrap mode bit mask
				vert.mLWTT = vert.mLWTT | (0u << 0); // layer (z coordinate of texture coordinates)
				
				mRBDData.mVertexData.push_back(std::move(vert));
			}
			
			std::cout << "invalid render character" << std::endl;
		}
		
		// create the indices to properly render the new quad
		unsigned int indexOffset = mCharsRendered * 4;
		mRBDData.mIndexData.insert(mRBDData.mIndexData.end(), {indexOffset, indexOffset + 1u, indexOffset + 2u,
				indexOffset, indexOffset + 2u, indexOffset + 3u});
		
		++mCharsRendered; // update the count of render characters
	}
	
	std::list<RenderBatchData> rbdList(1u, mRBDData); // make a copy of the untransformed batch data
	glm::mat3 transMat = mTransformation; // get the transform matrix for shape
	
	transMat *= util::GetTranslationMatrix(mPosition - mOrigin); // translate by position offset (take into account origin offset)
	
	transMat *= util::GetTranslationMatrix(mOrigin); // translate to origin...
	transMat *= util::GetRotationMatrix(mRotation); // ...rotate...
	transMat *= util::GetSkewingMatrix(mSkew); // ...skew...
	transMat *= util::GetScalingMatrix(mScale); // ...scale...
	transMat *= util::GetTranslationMatrix(-mOrigin); // ...and then translate back from origin
	
	transMat *= util::GetScalingMatrix(mTextScale); // apply the text scale last so as to not interfere with other transformations
	
	for (auto vertex = rbdList.back().mVertexData.begin(); vertex != rbdList.back().mVertexData.end(); ++vertex) {
		glm::vec3 pos = transMat * glm::vec3(vertex->mX, vertex->mY, 1.0f); // get position of transformed vertex
		vertex->mX = pos.x; vertex->mY = pos.y; vertex->mZ = mDepth; // set the position of the transformed character
		
		// set the colour of the transformed character
		vertex->mR = mColour.x * 255u; vertex->mG = mColour.y * 255u;
		vertex->mB = mColour.z * 255u; vertex->mA = mAlpha * 255u;
	}
	
	rbdList.back().mShader = GetShader(); // update the shader used to render this render string
	
	return rbdList;
}

void RenderString::UpdateRenderString() {
	if (!mAppendString.empty()) { // if we have text to append...
		CreateRenderCharacters(mAppendString); // create render character sructures for each of the chars in the string to append
		
		mString += mAppendString; // add the append string to the end of our existing string
		mAppendString = u""; // clear the append string
		
		mLocalBoundingBox.clear(); // since bounding box has been updated we need to rescale it
	}
	
	if (mLocalBoundingBox.empty()) { // if we haven't scaled the bounding box yet...
		mLocalBoundingBox = mLocalBoundingBoxFull;
		
		for (auto vertex = mLocalBoundingBox.begin(); vertex != mLocalBoundingBox.end(); ++vertex) {
			vertex->x *= mTextScale.x;
			vertex->y *= mTextScale.y;
		}
		
		UpdateGlobalBoundingBox();
	}
}

void RenderString::CreateRenderCharacters(const std::u16string& newString) {
	if (!newString.empty() && mFont) {
		int advanceAccum = 0; // the accumulated horizontal advance for the current line
		int kemingAccum = 0; // the accumulated kerning offset for the current line
		int lineOffset = 0; // the vertical offset of the current line
		int lineHeight = mFont->GetLineHeight(); // retrieve and store the new line offset
		
		if (!mString.empty() && !mRenderChars.empty()) { // if we already have characters entered...
			if (mString.back() == u'\n') { // if the last character entered was a new line...
				lineOffset = mRenderChars.back().mLineHeight + lineHeight; // adjust the line offset for following characters
			}
			else {
				// laod the states of the accumulators from the previous characters entered
				advanceAccum = mRenderChars.back().mAdvanceAccum;
				kemingAccum = mRenderChars.back().mKemingAccum;
				lineOffset = mRenderChars.back().mLineHeight;
				
				kemingAccum += mFont->GetKerning(mString.back(), newString.front()); // adjust the kerning offset for the new character
			}
		}
		
		for (unsigned int i = 0u; i < newString.size(); ++i) {
			char16_t codePoint = newString.at(i);
			
			if (codePoint == u'\n') { // if the current character is a new line...
				advanceAccum = 0; // reset the horizontal advance
				kemingAccum = 0; // reset the kerning offset
				lineOffset += lineHeight; // increment the vertical offset by the new line height
				continue; // go to the next character
			}
			
			try {
				const Font::Glyph& glyph = mFont->GetGlyph(codePoint); // get the glyph object for the current character
				Shape shape = glyph.mBaseShape; // get the glyph shape from the glyph object
				float height = 0.0f; // the height of the glyph shape used for positional offsetting
				
				{
					const std::vector<glm::vec2>& bbox = shape.GetLocalBoundingBox(); // get the bounding box of the glyph
					if (!bbox.empty()) { // if the bounding box is valid...
						height = bbox.at(2).y - bbox.at(0).y; // calculate the height of the glyph shape
					}
				}
				
				shape.SetPosition(glm::vec2((advanceAccum + kemingAccum) + glyph.mBearing.x,
						(mFont->GetBearingMax().y - glyph.mBearing.y) - mFont->GetBearingMax().y)); // position the glyph shape
				
				{ // update the local bounding box
					// get the bounding box of the newly added character
					std::vector<glm::vec2> bbox = shape.GetGlobalBoundingBox();
					
					if (bbox.size() > 3) { // if the bounding box is valid...
						mTopLeft.x = std::min(bbox.at(0).x, mTopLeft.x);
						mTopLeft.y = std::min(bbox.at(0).y, mTopLeft.y);
						
						mBottomRight.x = std::max(bbox.at(2).x, mBottomRight.x);
						mBottomRight.y = std::max(bbox.at(2).y, mBottomRight.y);
					}
				}
				
				advanceAccum += glyph.mAdvance; // increment the horizontal advance accumulator by the current glyph's advance metric
				if (i != newString.size() - 1) { // if this is not the final character in the string...
					kemingAccum += mFont->GetKerning(codePoint, newString.at(i + 1)); // increment the kerning offset by the kerning value of this and the next character
				}
				
				mRenderChars.push_back({std::move(shape), advanceAccum, kemingAccum, lineOffset,
						mTopLeft, mBottomRight});
			} catch(std::exception& e) {
				std::cout << "unknown codepoint: " << codePoint << std::endl;
			}
		}
		
		// update the full sized (not yet scaled) local bounding box
		glm::vec2 dimensions = mBottomRight - mTopLeft;
		mLocalBoundingBoxFull.clear();
		mLocalBoundingBoxFull.emplace_back(0.0f, 0.0f);
		mLocalBoundingBoxFull.emplace_back(dimensions.x, 0.0f);
		mLocalBoundingBoxFull.emplace_back(dimensions.x, dimensions.y);
		mLocalBoundingBoxFull.emplace_back(0.0f, dimensions.y);
	}
}
}
