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

#ifndef UAIRGUIINPUTBOXBASE_HPP
#define UAIRGUIINPUTBOXBASE_HPP

#include "gui.hpp"

namespace uair {
class GUIInputBoxBase : public GUIElement {
	public :
		void HandleMessageQueue(const MessageQueue::Entry& e, GUI* caller = nullptr);
		void Input(GUI* caller = nullptr);
		
		std::u16string GetString();
		void SetString(const std::u16string& newString);
	protected :
		virtual void OnHoverChange() = 0;
		virtual void OnStateChange() = 0;
		virtual void OnTextRemoved() = 0;
		virtual void OnTextAdded(const char16_t& newChar) = 0;
		virtual void OnTextSet(const std::u16string& newString) = 0;
	
	protected :
		std::string mName;
		
		bool mInArea = false; // indicates if the cursor is within the input box's bounding box (hovering)
		
		glm::vec2 mPosition;
		float mWidth = 0.0f;
		float mHeight = 0.0f;
		
		bool mActive = false; // indicates if the input box is accepting text input
		
		std::set<char16_t> mAllowedInput; // a set used for character lookup when entering text (permitted characters)
		std::u16string mString;
};
}

#endif
