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

#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "resource.hpp"
#include "texture.hpp"

// LOAD FONT VECTORS FROM FILE <-- IN PROGRESS: IS DONE? [X]
// CREATE OUTLINE SHAPES OF THEM <-- IN PROGRESS: IS DONE? [X]
	// SIDE: ADD SUPPORT FOR HOLES IN SHAPES <-- IN PROGRESS: IS DONE? [X]
// TEST VIA DRAWING THE GLYPHS <-- IN PROGRESS: IS DONE? [X]
	// SIDE: ADD TRIANGULATION/TESSELATION <-- IN PROGRESS: IS DONE? [X]
	// SIDE: ADD SUPPORT FOR INSETTING/OUTSETTING SHAPE <-- IN PROGRESS: IS DONE? [X]
// OUTSET AND INSET THE GLYPHS TO CREATE A GRADIENT AT THE EDGES <-- IN PROGRESS: IS DONE? [ ]
// DRAW TO TEXTURE AS A SIGNED DISTANCE FIELD
// ADD SUPPORT FOR RENDERING VIA SIGNED DISTANCE FIELD
// CLEAN UP

namespace uair {
class Polygon;

class Font : public Resource<Font> {
	public :
		Font() = default;
		Font(const Font& other) = delete;
		Font(Font&& other);
		~Font();
		
		Font& operator=(Font other);
		
		friend void swap(Font& first, Font& second);
		
		bool LoadFromFile(const std::string& filename, const unsigned int& pointSize = 36u);
		Polygon LoadGlyph(const char& charCode);
	private :
		FT_Face mFTFace = nullptr;
		
		// std::string mCharMap;
		// Texture mTex;
};
}

#endif
