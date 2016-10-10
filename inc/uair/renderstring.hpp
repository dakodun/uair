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

#ifndef UAIRRENDERSTRING_HPP
#define UAIRRENDERSTRING_HPP

#include "transformable2d.hpp"
#include "renderable.hpp"
#include "resourceptr.hpp"
#include "font.hpp"
#include "vbo.hpp"

namespace uair {
class RenderBatch;

class RenderString : public Transformable2D, public Renderable {
	friend class RenderBatch;
	
	public :
		std::string GetTag() const;
		
		void SetFont(ResourcePtr<Font> font); // set the font from a resource pointer
		void SetFont(Font* font); // set the font from a raw pointer
		void SetText(const std::u16string& newText); // change the string
		std::u16string GetText() const;
		void SetSize(const unsigned int& size); // set the size of the string
	protected :
		void UpdateGlobalBoundingBox();
		void UpdateGlobalMask();
		void CreateLocalMask();
		
		std::list<RenderBatchData> Upload();
	private :
		void CreateQuads();
	private :
		ResourcePtr<Font> mFont; // the font associated with this render string
		std::u16string mString; // the content of the render string
		unsigned int mSize = 1u; // the size the string is to be rendered at
		
		std::list<Shape> mQuads; // a list of individual glyph quads store for efficiency
		glm::vec2 mTextScale = glm::vec2(1.0f, 1.0f); // scale factor to size quads to match desired text size
};
}

#endif
