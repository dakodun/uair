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

#ifndef UAIRFONT_HPP
#define UAIRFONT_HPP

#include <string>
#include <vector>
#include <list>

#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "fbo.hpp"

namespace uair {
class RenderBatch;
class Polygon;
class Shape;

class Font : public Resource<Font> {
	public :
		struct Glyph { // associated with charcode in a map
			// tex coords in texture
			// font metrics (kerning, drop, ...)
		};
		
		Font(const unsigned int& textureSize);
		Font();
		Font(const Font& other) = delete;
		Font(Font&& other);
		~Font();
		
		Font& operator=(Font other);
		
		friend void swap(Font& first, Font& second);
		
		bool LoadFromFile(const std::string& filename, const unsigned int& pointSize = 36u);
		void LoadGlyph(const char& charCode);
		void LoadGlyphs(const std::vector<char>& charCodes);
	private :
		Shape CreateGlyphShape(const char& charCode);
		void UpdateTexture(const std::vector<Shape>& newShapes);
		
		void Pack(Shape& shape);
		void PositionBase(Shape& baseShape, const unsigned int& outerOffsetCount, const float& offsetInc);
		std::vector<Shape> CreateGradient(const Shape& baseShape, const unsigned int& innerOffsetCount, const unsigned int& outerOffsetCount, const float& offsetInc);
		void RenderToFBO(FBO& fbo, RenderBatch& batch, const unsigned int& width, const unsigned int& height);
	private :
		FT_Face mFTFace = nullptr;
		
		unsigned int mTextureSize;
		FBO mFBO;
		Texture mTexture;
		RenderBuffer mRenderBuffer;
		
		typedef std::pair<glm::ivec2, glm::ivec2> Rectangle;
		std::list<Rectangle> mRectangles;
};
}

#endif
