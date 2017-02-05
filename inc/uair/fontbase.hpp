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

#ifndef UAIRFONTBASE_HPP
#define UAIRFONTBASE_HPP

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
// namespace holding vectors containing the values of specific unicode character sets
namespace charset {
EXPORTDLL extern const std::vector<char16_t> BASICLATIN;
}

// abstract base class for most logic pertaining to font handling except rendering a single
// glyph to a texture (before packing it into the main texture atlas)
class EXPORTDLL FontBase : public Resource {
	protected :
		typedef std::pair<glm::ivec2, glm::ivec2> Rectangle; // a pair of ivec2 representing the top-left and bottom-right coords of a rectangle
		typedef std::pair<Rectangle, unsigned int> RectangleIndexPair; // a rectangle paired with an index into a glyph vector
	
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
	
	public :
		FontBase(const unsigned int& textureSize, const glm::uvec2& texColour);
		FontBase(const FontBase& other) = delete;
		FontBase(FontBase&& other);
		virtual ~FontBase() = 0;
		
		friend void swap(FontBase& first, FontBase& second);
		
		void CreateCache(const std::string& cacheFilename); // create a cache file of the current font
		bool LoadFromCache(const std::string& cacheFilename); // create the font from a cache file
		bool LoadFromFile(const std::string& filename, const unsigned int& pointSize = 72u); // load a font file for glyph loading
		void LoadGlyph(const char16_t& charCode); // load a single glyph from the font file and add it to the store (and texture)
		void LoadGlyphs(const std::vector<char16_t>& charCodes); // load a group of glyphs from the font file and add them to the store (and texture)
		
		Glyph GetGlyph(const char16_t& codePoint); // return the glyph object corresponding to a character
		int GetKerning(const char16_t& firstCodePoint, const char16_t& secondCodePoint); // return the kerning value between two characters
		
		unsigned int GetFontSize() const; // return the base size of the font
		int GetLineHeight() const; // return the vertical offset for a new line
		unsigned int GetTextureID() const; // return the assigned id of the font's texture
		unsigned int GetTextureWidth() const; // return the font's texture's width
		unsigned int GetTextureHeight() const; // return the font's texture's height
		
		int GetAdvanceMax() const; // return the biggest horziontal advance value (horizontal spacing)
		int GetDropMax() const; // return the biggest vertical drop value (vertical offset from baseline)
		glm::ivec2 GetBearingMax() const; // return the biggest horizontal and vertical offset (essentially top-left of glyph)
		
		virtual unsigned int GetType() const = 0; // return the type of font for rendering purposed (vertex attribute)
	protected :
		void UpdateTexture(const std::vector< std::pair<char16_t, Glyph> >& glyphs); // add the new glyphs to the store and texture
		void UpdateKerning(const std::vector< std::pair<char16_t, Glyph> >& glyphs); // update the kerning map for the new glyphs (and old)
		
		static bool SortGlyph(const RectangleIndexPair& first, const RectangleIndexPair& second); // sorts two rectangles by their area
		
		// prepares a glyph to be added into the main texture by positioning its base shape
		// and setting texture information
		Shape PackGlyph(Glyph& glyph);
		
		virtual bool SetSize(const unsigned int& fontSize) = 0;
		virtual void CreateGlyph(const char16_t& charCode, std::vector< std::pair<char16_t, Glyph> >& glyphs,
				std::vector<Texture>& glyphTextures) = 0;
	
	public :
		static FT_Library* mFTLibraryPtr;
	protected :
		FT_Face mFTFace = nullptr; // pointer to the face object of a font file
		unsigned int mFontSize = 1u; // the base size of the font
		int mLineHeight = 0; // the vertical offset of a new line
		
		int mAdvanceMax = 0; // the longest horizontal advance of all glyphs
		int mDropMax = 0; // the biggest drop of all glyphs
		glm::ivec2 mBearingMax = glm::ivec2(); // the widest and highest bearings of all glyphs
		
		std::map<char16_t, Glyph> mGlyphs; // glyph objects containing metrics and rendering data for a character
		std::map<std::pair<char16_t, char16_t>, int> mKernMap; // kerning values between two characters
		
		glm::uvec2 mTextureColour; // the initial colour (and alpha value) of the font's texture
		unsigned int mTextureSize; // the width and height of the font's texture
		FBO mFBO; // the fbo used to render to the font's texture
		Texture mTexture; // the font's texture containing all loaded glyphs
		RenderBuffer mRenderBuffer; // render buffer associated with font's texture
		
		std::list< std::list<Rectangle> > mRectangles; // all rectangular areas that are free in the texture (used for packing)
		
		std::string mCacheTag = ""; // the tag used to identify the type of font that created the cache
};
}

#endif
