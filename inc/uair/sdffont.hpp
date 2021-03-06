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

#ifndef UAIRSDFFONT_HPP
#define UAIRSDFFONT_HPP

#include "fontbase.hpp"

namespace uair {
class EXPORTDLL SDFFont : public FontBase {
	public :
		SDFFont();
		SDFFont(const unsigned int& textureSize);
		SDFFont(const SDFFont& other) = delete;
		SDFFont(SDFFont&& other);
		
		SDFFont& operator=(SDFFont other);
		
		friend void swap(SDFFont& first, SDFFont& second);
		
		static unsigned int GetTypeID();
		
		unsigned int GetType() const;
	protected :
		bool SetSize(const unsigned int& fontSize);
		void CreateGlyph(const char16_t& charCode, std::vector< std::pair<char16_t, Glyph> >& glyphs,
				std::vector<Texture>& glyphTextures);
};
}

#endif
