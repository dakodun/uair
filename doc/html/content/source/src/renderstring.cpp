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
#include "renderbatch.hpp"
#include "util.hpp"
#include "shape.hpp"

namespace uair {
std::string RenderString::GetTag() const {
	return "RenderString";
}

void RenderString::SetFont(ResourcePtr<Font> font) {
	mFont = font;
	SetSize(mSize);
	
	mQuads.clear();
	CreateQuads();
}

void RenderString::SetFont(Font* font) {
	mFont.Set(font);
	SetSize(mSize); // update the size of the font
	
	mQuads.clear();
	CreateQuads();
}

void RenderString::SetText(const std::u16string& newText) {
	mString = newText; // update the string
	
	mQuads.clear();
	CreateQuads();
}

std::u16string RenderString::GetText() const {
	return mString;
}

void RenderString::SetSize(const unsigned int& size) {
	mSize = size;
	
	if (mFont) { // if we have a valid font...
		float fontSize = static_cast<float>(mFont->GetFontSize()); // get the size of the font
		mTextScale = glm::vec2(mSize / fontSize, mSize / fontSize); // calculate the scale factor for the render string
	}
	
	mQuads.clear();
	CreateQuads();
}

void RenderString::UpdateGlobalBoundingBox() {
	// transform quads
	
	/* if (!mContours.empty() && !mContours.front().mPoints.empty()) { // transform all points, find new bounds, create bb
		glm::mat3 trans = mTransformation;
		
		trans *= util::GetTranslationMatrix(mPosition - mOrigin);
		
		trans *= util::GetTranslationMatrix(mOrigin);
		trans *= util::GetRotationMatrix(mRotation);
		trans *= util::GetSkewingMatrix(mSkew);
		trans *= util::GetScalingMatrix(mScale);
		trans *= util::GetTranslationMatrix(-mOrigin);
		
		std::vector<glm::vec2> transformedBounds{mContours.front().mPoints.front(), mContours.front().mPoints.front()};
		
		for (auto contour = mContours.begin(); contour != mContours.end(); ++contour) {
			for (auto point = contour->mPoints.begin(); point != contour->mPoints.end(); ++point) {
				glm::vec3 pos = trans * glm::vec3(*point, 1.0f);
				
				if (pos.x < transformedBounds.at(0).x) { // left
					transformedBounds.at(0).x = pos.x;
				}
				else if (pos.x > transformedBounds.at(1).x) { // right
					transformedBounds.at(1).x = pos.x;
				}
				
				if (pos.y < transformedBounds.at(0).y) { // top
					transformedBounds.at(0).y = pos.y;
				}
				else if (pos.y > transformedBounds.at(1).y) { // bottom
					transformedBounds.at(1).y = pos.y;
				}
			}
		}
		
		mGlobalBoundingBox.clear();
		mGlobalBoundingBox.emplace_back(transformedBounds.at(0).x, transformedBounds.at(0).y);
		mGlobalBoundingBox.emplace_back(transformedBounds.at(1).x, transformedBounds.at(0).y);
		mGlobalBoundingBox.emplace_back(transformedBounds.at(1).x, transformedBounds.at(1).y);
		mGlobalBoundingBox.emplace_back(transformedBounds.at(0).x, transformedBounds.at(1).y);
	} */
}

void RenderString::UpdateGlobalMask() {
	glm::mat3 trans = mTransformation;
	
	trans *= util::GetTranslationMatrix(mPosition - mOrigin);
	
	trans *= util::GetTranslationMatrix(mOrigin);
	trans *= util::GetRotationMatrix(mRotation);
	trans *= util::GetSkewingMatrix(mSkew);
	trans *= util::GetScalingMatrix(mScale);
	trans *= util::GetTranslationMatrix(-mOrigin);
	
	trans *= util::GetScalingMatrix(mTextScale);
	
	mGlobalMask.clear();
	for (auto iter = mLocalMask.begin(); iter != mLocalMask.end(); ++iter) {
		glm::vec3 pos = trans * glm::vec3(*iter, 1.0f);
		mGlobalMask.emplace_back(pos.x, pos.y);
	}
}

void RenderString::CreateLocalMask() {
	mLocalMask.clear();
	
	if (mLocalBoundingBox.size() > 3) {
		mLocalMask.insert(mLocalMask.end(), mLocalBoundingBox.begin(), mLocalBoundingBox.end());
	}
}

std::list<RenderBatchData> RenderString::Upload() {
	std::list<RenderBatchData> rbdList;
	
	glm::mat3 transMat = mTransformation; // get the transform matrix for shape
	
	transMat *= util::GetTranslationMatrix(mPosition - mOrigin); // translate by position offset (take into account origin offset)
	
	transMat *= util::GetTranslationMatrix(mOrigin); // translate to origin...
	transMat *= util::GetRotationMatrix(mRotation); // ...rotate...
	transMat *= util::GetSkewingMatrix(mSkew); // ...skew...
	transMat *= util::GetScalingMatrix(mScale); // ...scale...
	transMat *= util::GetTranslationMatrix(-mOrigin); // ...and then translate back from origin
	
	transMat *= util::GetScalingMatrix(mTextScale); // apply the text scale last so as to not interfere with other transformations
	
	for (auto quad = mQuads.begin(); quad != mQuads.end(); ++quad) {
		quad->SetDepth(GetDepth());
		quad->SetColour(GetColour());
		quad->SetAlpha(GetAlpha());
		quad->SetShader(GetShader());
		
		std::vector<glm::vec2> vertices; // all transformed vertices in glyph quad (from each contour)
		const std::vector<Contour>& contours = quad->GetContours(); // get reference to glyph quad's contours
		for (auto contour = contours.begin(); contour != contours.end(); ++contour) { // for all quad contours...
			const std::vector<glm::vec2>& points = contour->GetPoints(); // get const reference to vertices in countour
			
			for (auto vertex = points.begin(); vertex != points.end(); ++vertex) { // for all vertices in contour...
				glm::mat3 transMatQuad = transMat * util::GetTranslationMatrix(quad->GetPosition() - quad->GetOrigin()); // translate by position offset (take into account origin offset)
				glm::vec3 pos = transMatQuad * glm::vec3(*vertex, 1.0f); // get position of transformed vertex
				vertices.emplace_back(pos.x, pos.y); // add transformed vertex
			}
		}
		
		std::vector<glm::vec2> texCoords; // texture coordinates in font texture representing glyph
		if (quad->mCurrentFrame < quad->mFrames.size()) { // if glyph quad's current frame is valid...
			texCoords.insert(texCoords.end(), quad->mFrames.at(quad->mCurrentFrame).mTexCoords.begin(), quad->mFrames.at(quad->mCurrentFrame).mTexCoords.end()); // add texture coordinates
		}
		
		RenderBatchData rbd; // rendering data struct
		
		quad->CreateVBOVertices(rbd, vertices, texCoords, {}); // create vbo vertices of the shape
		for (auto vbovert = rbd.mVertexData.begin(); vbovert != rbd.mVertexData.end(); ++vbovert) { // for all new vbo vertices...
			vbovert->mType = 1.0f; // set the type of vertex to 1
		}
		
		rbd.mIndexData = {0u, 1u, 2u, 0u, 2u, 3u};
		
		rbd.mTextureID = mFont->GetTextureID(); // set the texture to the font's texture
		rbd.mRenderMode = GL_TRIANGLES;
		rbd.mTag = GetTag();
		
		rbd.mShader = GetShader();
		
		rbdList.push_back(std::move(rbd));
	}
	
	return rbdList;
}

void RenderString::CreateQuads() {
	if (mQuads.empty()) { // if the glyph quads haven't been created yet...
		int advanceAccum = 0; // the accumulated horizontal advance for the current line
		int kemingAccum = 0; // the accumulated kerning offset for the current line
		int lineOffset = 0; // the vertical offset of the current line
		int lineHeight = mFont->GetLineHeight(); // retrieve and store the new line offset
		
		glm::vec2 topLeft(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
		glm::vec2 bottomRight(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());
		
		for (unsigned int i = 0u; i < mString.size(); ++i) { // for all characters in the string...
			char16_t codePoint = mString.at(i); // get the current character
			
			if (codePoint == u'\n') { // if the current character is a new line...
				advanceAccum = 0; // reset the horizontal advance
				kemingAccum = 0; // reset the kerning offset
				lineOffset += lineHeight; // increment the vertical offset by the new line height
				continue; // go to the next character
			}
			
			try {
				const Font::Glyph& glyph = mFont->GetGlyph(codePoint); // get the glyph object for the current character
				Shape shape = glyph.mBaseShape; // get the glyph shape from the glyph object
				
				glm::vec2 pos = glm::vec2((advanceAccum + kemingAccum) + glyph.mBearing.x, glyph.mDrop + lineOffset); // calculate the position of the glyph shape
				float height = 0.0f; // the height of the glyph shape used for positional offsetting [!] get from dimensions?
				
				{
					const std::vector<glm::vec2>& bbox = shape.GetLocalBoundingBox(); // get the bounding box of the glyph
					if (!bbox.empty()) { // if the bounding box is valid...
						height = bbox.at(2).y - bbox.at(0).y; // calculate the height of the glyph shape
					}
				}
				
				shape.SetPosition(pos); // position the glyph shape
				shape.SetOrigin(glm::vec2(0.0f, height)); // set the origin offset of the glyph shape
				// shape.SetColour(mColour);
				// shape.SetAlpha(mAlpha);
				
				mQuads.push_back(std::move(shape)); // add the glyph shape to the stored glyph quad array
				
				{ // update the local bounding box
					std::vector<glm::vec2> bbox = mQuads.back().GetGlobalBoundingBox();
					
					if (bbox.size() > 3) {
						glm::mat3 trans;
						trans *= util::GetScalingMatrix(mTextScale);
						
						glm::vec3 bboxTopLeft = trans * glm::vec3(bbox.at(0), 1.0f);
						glm::vec3 bboxBottomRight = trans * glm::vec3(bbox.at(2), 1.0f);
						
						topLeft.x = std::min(bboxTopLeft.x, topLeft.x);
						topLeft.y = std::min(bboxTopLeft.y, topLeft.y);
						
						bottomRight.x = std::max(bboxBottomRight.x, bottomRight.x);
						bottomRight.y = std::max(bboxBottomRight.y, bottomRight.y);
					}
				}
				
				advanceAccum += glyph.mAdvance; // increment the horizontal advance accumulator by the current glyph's advance metric
				if (i != mString.size() - 1) { // if this is not the final character in the string...
					kemingAccum += mFont->GetKerning(codePoint, mString.at(i + 1)); // increment the kerning offset by the kerning value of this and the next character
				}
			} catch(std::exception& e) {
				std::cout << "unknown codepoint: " << codePoint << std::endl;
			}
		}
		
		bottomRight -= topLeft;
		mLocalBoundingBox.clear();
		mLocalBoundingBox.emplace_back(0.0f, 0.0f);
		mLocalBoundingBox.emplace_back(bottomRight.x, 0.0f);
		mLocalBoundingBox.emplace_back(bottomRight.x, bottomRight.y);
		mLocalBoundingBox.emplace_back(0.0f, bottomRight.y);
	}
}
}
