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
#include "exception.hpp"

namespace uair {
std::string RenderString::GetTag() const {
	return "RenderString";
}

void RenderString::SetFont(ResourcePtr<Font> font) {
	mFont = font;
	mQuads.clear();
	SetSize(mSize);
}

void RenderString::SetFont(Font* font) {
	mFont.SetResource(font);
	mQuads.clear(); // reset the stored glyph quads
	SetSize(mSize); // update the size of the font
}

void RenderString::SetText(const std::u32string& newText) {
	mString = newText; // update the string
	mQuads.clear();
}

void RenderString::SetSize(const unsigned int& size) {
	mSize = size;
	
	if (mFont.IsValid()) { // if we have a valid font...
		float fontSize = static_cast<float>(mFont.GetResource()->GetFontSize()); // get the size of the font
		mScale = glm::vec2(mSize / fontSize, mSize / fontSize); // calculate the scale factor for the render string
	}
}

void RenderString::UpdateGlobalBoundingBox() {
	
}

void RenderString::UpdateGlobalMask() {
	
}

void RenderString::CreateLocalMask() {
	
}

std::list<RenderBatchData> RenderString::Upload() {
	std::list<RenderBatchData> rbdList;
	
	if (mQuads.empty()) { // if the glyph quads haven't been created yet...
		int advanceAccum = 0; // the accumulated horizontal advance for the current line
		int kemingAccum = 0; // the accumulated kerning offset for the current line
		int lineOffset = 0; // the vertical offset of the current line
		int lineHeight = mFont.GetResource()->GetLineHeight(); // retrieve and store the new line offset
		
		for (unsigned int i = 0u; i < mString.size(); ++i) { // for all characters in the string...
			char32_t codePoint = mString.at(i); // get the current character
			
			if (codePoint == U'\n') { // if the current character is a new line...
				advanceAccum = 0; // reset the horizontal advance
				kemingAccum = 0; // reset the kerning offset
				lineOffset += lineHeight; // increment the vertical offset by the new line height
				continue; // go to the next character
			}
			
			try {
				const Font::Glyph& glyph = mFont.GetResource()->GetGlyph(codePoint); // get the glyph object for the current character
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
				shape.SetColour(mColour);
				shape.SetAlpha(mAlpha);
				
				mQuads.push_back(std::move(shape)); // add the glyph shape to the stored glyph quad array
				
				advanceAccum += glyph.mAdvance; // increment the horizontal advance accumulator by the current glyph's advance metric
				if (i != mString.size() - 1) { // if this is not the final character in the string...
					kemingAccum += mFont.GetResource()->GetKerning(codePoint, mString.at(i + 1)); // increment the kerning offset by the kerning value of this and the next character
				}
			} catch(UairException& e) {
				std::cout << "unknown codepoint: " << codePoint << std::endl;
			}
		}
	}
	
	glm::mat3 transMat = mTransformation; // get the transform matrix for shape
	transMat *= util::GetTranslationMatrix(mPosition - mOrigin); // translate by position offset (take into account origin offset)
	
	transMat *= util::GetTranslationMatrix(mOrigin); // translate to origin...
	transMat *= util::GetRotationMatrix(mRotation); // ...rotate...
	transMat *= util::GetSkewingMatrix(mSkew); // ...skew...
	transMat *= util::GetScalingMatrix(mScale); // ...scale...
	transMat *= util::GetTranslationMatrix(-mOrigin); // ...and then translate back from origin
	
	for (auto quad = mQuads.begin(); quad != mQuads.end(); ++quad) {
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
		for (auto vbovert = rbd.mVertData.begin(); vbovert != rbd.mVertData.end(); ++vbovert) { // for all new vbo vertices...
			vbovert->mType = 1.0f; // set the type of vertex to 1
		}
		
		rbd.mIndData = {0u, 1u, 2u, 0u, 2u, 3u};
		
		rbd.mTexID = mFont.GetResource()->GetTextureID(); // set the texture to the font's texture
		rbd.mRenderMode = GL_TRIANGLES;
		rbd.mTag = GetTag();
		
		rbdList.push_back(std::move(rbd));
	}
	
	return rbdList;
}
}
