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

#ifndef UAIRGUIBUTTONBASE_HPP
#define UAIRGUIBUTTONBASE_HPP

#include "init.hpp"
#include "gui.hpp"

namespace uair {
class Font;

class EXPORTDLL GUIButtonBase : public GUIElement {
	public :
		void HandleMessageQueue(const MessageQueue::Entry& e, GUI* caller = nullptr);
		void Input(GUI* caller = nullptr);
	protected :
		virtual void OnHoverChange() = 0;
		virtual void OnStateChange() = 0;
	
	protected :
		std::string mName;
		
		glm::vec2 mPosition;
		float mWidth = 0.0f;
		float mHeight = 0.0f;
		
		bool mInArea = false;
		unsigned int mState = 0u;
};
}

#endif
