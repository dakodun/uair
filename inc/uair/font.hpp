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

#ifndef UAIRFONT_HPP
#define UAIRFONT_HPP

#include <string>
#include <vector>
#include <list>

#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "init.hpp"
#include "fbo.hpp"
#include "shape.hpp"

namespace uair {
namespace charset {
EXPORTDLL extern const std::vector<char16_t> BASICLATIN;
}

class RenderBatch;
class Polygon;

class EXPORTDLL Font : public Resource {
	public :
		struct Glyph {
			Shape mBaseShape = Shape(); // a textured shape representing the glyph
			
			glm::ivec2 mDimensions = glm::ivec2(); // the width and height of the glyph
			std::vector<glm::vec2> mTexCoords = {}; // the texture coordinates (in the font's texture) for the glyph
			unsigned int mLayer = 0u; // the layer (in the font's texture) for the glyph
			
			int mAdvance = 0; // the horizontal advance metric for the character
			int mDrop = 0; // the vertical drop metric for the character
			glm::ivec2 mBearing = glm::ivec2(); // the x and y bearing (offset from the origin) for the character
		};
		
		Font(const unsigned int& textureSize);
		Font();
		Font(const Font& other) = delete;
		Font(Font&& other);
		~Font();
		
		Font& operator=(Font other);
		
		friend void swap(Font& first, Font& second);
		
		void CreateCache(const std::string& cacheFilename); // create a cache file of the current font
		bool LoadFromCache(const std::string& cacheFilename); // create the font from a cache file
		bool LoadFromFile(const std::string& filename, const unsigned int& pointSize = 72u); // load a font file for glyph loading
		void LoadGlyph(const char16_t& charCode); // add a glyph to the font's texture
		void LoadGlyphs(const std::vector<char16_t>& charCodes); // add glyphs to the font's texture
		
		Glyph GetGlyph(const char16_t& codePoint); // return the glyph object corresponding to a character
		int GetKerning(const char16_t& firstCodePoint, const char16_t& secondCodePoint); // return the kerning value between two characters
		
		unsigned int GetFontSize() const; // return the base size of the font
		int GetLineHeight() const; // return the vertical offset for a new line
		unsigned int GetTextureID() const; // return the assigned id of the font's texture
		unsigned int GetTextureWidth() const; // return the font's texture's width
		unsigned int GetTextureHeight() const; // return the font's texture's height
		
		int GetAdvanceMax() const;
		int GetDropMax() const;
		glm::ivec2 GetBearingMax() const;
		
		static unsigned int GetTypeID();
	private :
		Shape CreateGlyphShape(const char16_t& charCode, Glyph& glyphObject); // create a shape of a glyph from a font file
		void UpdateTexture(const std::vector< std::pair<char16_t, Shape> >& newShapes); // update the font's texture with new glyph shapes
		void UpdateKerningMap(const std::vector< std::pair<char16_t, Shape> >& newShapes); // updated the kerning map with new glyphs
		
		Glyph Pack(Shape& shape); // pack a flyph shape into the font's texture
		void PositionBase(Shape& baseShape, const unsigned int& padding); // position a glyph shape at the origin
	
	public :
		static FT_Library* mFTLibraryPtr;
	private :
		FT_Face mFTFace = nullptr; // pointer to the face object of a font file
		unsigned int mFontSize = 1u; // the base size of the font
		int mLineHeight = 0; // the vertical offset of a new line
		
		int mAdvanceMax = 0; // the longest horizontal advance of all glyphs
		int mDropMax = 0; // the biggest drop of all glyphs
		glm::ivec2 mBearingMax = glm::ivec2(); // the widest and highest bearings of all glyphs
		
		std::map<char16_t, Glyph> mGlyphs; // glyph objects containing metrics and rendering data for a character
		std::map<std::pair<char16_t, char16_t>, int> mKernMap; // kerning values between two characters
		
		unsigned int mTextureSize; // the width and height of the font's texture
		FBO mFBO; // the fbo used to render to the font's texture
		Texture mTexture; // the font's texture containing all loaded glyphs
		RenderBuffer mRenderBuffer; // render buffer associated with font's texture
		
		typedef std::pair<glm::ivec2, glm::ivec2> Rectangle; // a pair of ivec2 representing the top-left and bottom-right coords of a rectangle
		std::list< std::list<Rectangle> > mRectangles; // all rectangular areas that are free in the texture (used for packing)
};
}

#endif
