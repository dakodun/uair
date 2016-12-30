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

#include "init.hpp"
#include "transformable2d.hpp"
#include "renderable.hpp"
#include "renderbatch.hpp"
#include "resourceptr.hpp"
#include "font.hpp"
#include "vbo.hpp"

namespace uair {
class EXPORTDLL RenderString : public Transformable2D, public Renderable {
	private :
		// aggregate that holds a rendered characters glyph shape and information about the renderstring
		// when it was created (to allow faster roll back)
		struct RenderCharacter {
			Shape mCharacter; // the shape that represents the character visually
			
			// store the state of the accumulators so we can work backwards through the string
			int mAdvanceAccum;
			int mKemingAccum;
			int mLineHeight;
			
			// store the current bounds so we can recreate bounding boxes quickly
			glm::vec2 mTopLeft;
			glm::vec2 mBottomRight;
		};
	
	public :
		RenderString() = default;
		RenderString(ResourcePtr<Font> font, const unsigned int& size, const std::u16string& text = u"");
		RenderString(Font* font, const unsigned int& size, const std::u16string& text = u"");
		
		std::string GetTag() const;
		
		void SetFont(ResourcePtr<Font> font); // set the font from a resource pointer
		void SetFont(Font* font); // set the font from a raw pointer
		void SetText(const std::u16string& newText); // change the string completely
		void AddText(const std::u16string& newText); // add a string to the end of the existing string
		void AddText(const char16_t& newChar); // add a character to the end of the existing string
		bool RemoveText(const unsigned int& numChars); // remove a number of characters from the end of the existing string
		std::u16string GetText() const; // returning the current existing string
		void SetSize(const unsigned int& size); // set the size of the string
		
		std::list<RenderBatchData> Upload();
	protected :
		void UpdateGlobalBoundingBox();
		void UpdateGlobalMask();
		void CreateLocalMask();
	private :
		void UpdateRenderString();
		void CreateRenderCharacters(const std::u16string& newString);
	
	private :
		ResourcePtr<Font> mFont; // the font associated with this render string
		unsigned int mSize = 1u; // the size the string is to be rendered at
		std::u16string mString = u""; // the content of the render string
		
		// the new text that has been added to the renderstring that hasn't been converted into render characters yet
		std::u16string mAppendString = u"";
		
		std::vector<RenderCharacter> mRenderChars; // a list of individual renderable character structures stored for efficiency
		glm::vec2 mTextScale = glm::vec2(1.0f, 1.0f); // scale factor to size quads to match desired text size
		
		// the current bounds of the renderstring as well as the full-sized local bounding box
		glm::vec2 mTopLeft = glm::vec2(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
		glm::vec2 mBottomRight = glm::vec2(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());
		std::vector<glm::vec2> mLocalBoundingBoxFull;
		
		unsigned int mCharsRendered = 0u; // the number of characters in the renderstring that have been rendered
		RenderBatchData mRBDData; // untransformed data for the renderbatch that is used to draw this renderstring
};
}

#endif
