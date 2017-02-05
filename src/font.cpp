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

#include "font.hpp"

#include "signeddistancefield.hpp"
#include "util.hpp"
#include "openglstates.hpp"
#include "renderbatch.hpp"

namespace uair {
Font::Font() : FontBase(512u, glm::uvec2(255u, 0u)) {
	mCacheTag = "UFC";
}

Font::Font(const unsigned int& textureSize) : FontBase(textureSize, glm::uvec2(255u, 0u)) {
	mCacheTag = "UFC";
}

Font::Font(Font&& other) : Font() {
	swap(*this, other);
}

Font& Font::operator=(Font other) {
	swap(*this, other);
	return *this;
}

void swap(Font& first, Font& second) {
	std::swap(first.mFTFace, second.mFTFace);
	std::swap(first.mFontSize, second.mFontSize);
	std::swap(first.mLineHeight, second.mLineHeight);
	
	std::swap(first.mAdvanceMax, second.mAdvanceMax);
	std::swap(first.mDropMax, second.mDropMax);
	std::swap(first.mBearingMax, second.mBearingMax);
	
	std::swap(first.mGlyphs, second.mGlyphs);
	std::swap(first.mKernMap, second.mKernMap);
	
	std::swap(first.mTextureSize, second.mTextureSize);
	std::swap(first.mFBO, second.mFBO);
	std::swap(first.mTexture, second.mTexture);
	std::swap(first.mRenderBuffer, second.mRenderBuffer);
	
	std::swap(first.mRectangles, second.mRectangles);
	
	std::swap(first.mCacheTag, second.mCacheTag);
	
	// ensure glyph shapes point to the correct font texture
		for (auto glyph = first.mGlyphs.begin(); glyph != first.mGlyphs.end(); ++glyph) {
			Shape shp;
			shp.AddContour(Contour({glm::vec2(0.0f, 0.0f), glm::vec2(glyph->second.mDimensions.x, 0.0f),
					glm::vec2(glyph->second.mDimensions.x, glyph->second.mDimensions.y), glm::vec2(0.0f, glyph->second.mDimensions.y)}));
			shp.AddFrameCoords(&first.mTexture, glyph->second.mLayer, glyph->second.mTexCoords);
			
			glyph->second.mBaseShape = std::move(shp);
		}
		
		for (auto glyph = second.mGlyphs.begin(); glyph != second.mGlyphs.end(); ++glyph) {
			Shape shp;
			shp.AddContour(Contour({glm::vec2(0.0f, 0.0f), glm::vec2(glyph->second.mDimensions.x, 0.0f),
					glm::vec2(glyph->second.mDimensions.x, glyph->second.mDimensions.y), glm::vec2(0.0f, glyph->second.mDimensions.y)}));
			shp.AddFrameCoords(&second.mTexture, glyph->second.mLayer, glyph->second.mTexCoords);
			
			glyph->second.mBaseShape = std::move(shp);
		}
	//
}

unsigned int Font::GetTypeID() {
	return static_cast<unsigned int>(Resources::Font);
}

unsigned int Font::GetType() const {
	return 0u;
}

bool Font::SetSize(const unsigned int& fontSize) {
	FT_Error ftError = FT_Set_Pixel_Sizes(mFTFace, fontSize, fontSize);
	if (ftError != 0) { // if an error occured...
		FT_Done_Face(mFTFace); // clean up font face object
		mFTFace = nullptr; // set pointer to null
		std::cout << "error setting size: " << ftError << std::endl; // output error message
		return false; // return failure
	}
	
	return true;
}

void Font::CreateGlyph(const char16_t& charCode, std::vector< std::pair<char16_t, Glyph> >& glyphs,
		std::vector<Texture>& glyphTextures) {
	
	FT_Load_Char(mFTFace, charCode, FT_LOAD_RENDER); // load the glyph into the face
	FT_Bitmap bitmap = mFTFace->glyph->bitmap;
	if (mFTFace->glyph->format != FT_GLYPH_FORMAT_BITMAP) {
		std::cout << "" << std::endl;
		
		throw std::runtime_error("font: wrong glyph type");
	}
	
	//
		std::vector<unsigned char> data;
		for (int y = 0; y < bitmap.rows; ++y) {
			for (int x = 0; x < bitmap.width; ++x) {
				data.push_back(255);
				data.push_back(255);
				data.push_back(255);
				data.push_back(bitmap.buffer[x + (y * bitmap.width)]);
			}
		}
		
		glyphTextures.emplace_back(); // add a new texture to the array
		glyphTextures.back().AddFromMemory(data, bitmap.width, bitmap.rows, true);
		glyphTextures.back().CreateTexture();
		
		// add a contour matching the texture's dimensions
		Shape glyphShape; // a textured shape of the distance field of the glyph
		glyphShape.AddContour(Contour({glm::vec2(0.0f, 0.0f), glm::vec2(bitmap.width, 0.0f),
				glm::vec2(bitmap.width, bitmap.rows), glm::vec2(0.0f, bitmap.rows)}));
		glyphShape.AddFrameRect(&glyphTextures.back(), 0u, {glm::vec2(0.0f, 0.0f), glm::vec2(bitmap.width, bitmap.rows)}); // texture the new rectangle shape
		
		//
			Glyph glyph;
			
			// transform metrics from 26.6 format ("<< 6" or "/ 64")
			glyph.mAdvance = mFTFace->glyph->metrics.horiAdvance >> 6;
			glyph.mDrop = (mFTFace->glyph->metrics.height >> 6) - (mFTFace->glyph->metrics.horiBearingY >> 6);
			glyph.mBearing = glm::ivec2(mFTFace->glyph->metrics.horiBearingX >> 6, mFTFace->glyph->metrics.horiBearingY >> 6);
			
			mAdvanceMax = std::max(mAdvanceMax, glyph.mAdvance);
			mDropMax = std::max(mDropMax, glyph.mDrop);
			mBearingMax = glm::vec2(std::max(mBearingMax.x, glyph.mBearing.x), std::max(mBearingMax.y, glyph.mBearing.y));
			glyph.mBaseShape = std::move(glyphShape);
			
			glyphs.push_back(std::make_pair(charCode, glyph));
		//
	//
}
}
