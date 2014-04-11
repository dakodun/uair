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

#include "font.hpp"

#include <iostream>

#include "game.hpp"

namespace uair {
void Font::AddFromFile(const std::string& filename) {
	FT_Face ftFace;
	FT_Error ftError = FT_New_Face(GAME.GetFTLibrary(), filename.c_str(), 0, &ftFace);
	if (ftError != 0) {
		std::cout << "" << std::endl;
		return;
	}
	
	// ftError = FT_Set_Pixel_Sizes(ftFace, x, y); // set the (rough) pixel size
	// FT_HAS_KERNING(ftFace) // check if the font has kerning
	
	// for char map
		// glyphIndex = FT_Get_Char_Index(ftFace, charcode); // get the index of the current character code
		// ftError = FT_Load_Glyph(ftFace, glyphIndex, FT_LOAD_DEFAULT); // load the corresponding glyph
		// if (ftFace->glyph->format != FT_GLYPH_FORMAT_BITMAP) { // if the glyph isn't pre-rendered
			// ftError = FT_Render_Glyph(ftFace->glyph, FT_RENDER_MODE_NORMAL); // render the glyph
			// ftError = FT_Get_Glyph(ftFace->glyph, &ftGlyph); // extract the glyph from the face
			
			// covert the glyph to a bitmap glyph to access its contents easier
			// FT_BitmapGlyph ftBitmapGlyph = (FT_BitmapGlyph)ftGlyph;
			// store the width, height, advance, drop, left and texture data
	
	// need seperate texture for each size requested? yea probs lol
	// pass in charset too, it's easier
	// allow unicode (can freetype convert these codes itself? hopefully).
	// every font can have multpiple textures for each size, which are generated when needed? maybe
	// do bold, italic, and other styles
	// need some sort of struct to manage the texture, i.e. handle the offsets of each glyph
	// rendering uses multiple quads, each using the same texture
	// add fancy shit afterwards
	
	FT_Done_Face(ftFace);
}
}
