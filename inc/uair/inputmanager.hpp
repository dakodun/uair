/* **************************************************************** **
**	Uair Engine
**	Copyright (c) 2013 Iain M. Crawford
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

/** 
* \file		inputmanager.hpp
* \brief	InputManager.
**/

#ifndef UAIRINPUTMANAGER_HPP
#define UAIRINPUTMANAGER_HPP

#include <memory>
#include <map>
#include <glm/glm.hpp>

#include "inputenums.hpp"

namespace uair {
class InputManager {
	friend class Game;
	
	public :
		InputManager();
		
		void Process();
		
		bool GetKeyboardDown(const Keyboard & key) const;
		bool GetKeyboardPressed(const Keyboard & key) const;
		bool GetKeyboardReleased(const Keyboard & key) const;
		
		bool GetMouseDown(const Mouse & button) const;
		bool GetMousePressed(const Mouse & button) const;
		bool GetMouseReleased(const Mouse & button) const;
		
		int GetMouseWheel() const;
		glm::ivec2 GetLocalMouseCoords() const;
		glm::ivec2 GetGlobalMouseCoords() const;
	private :
		void Reset();
		
		void HandleKeyboardKeys(const Keyboard & key, const unsigned int & type);
		
		void HandleMouseButtons(const Mouse & button, const unsigned int & type);
		void HandleMouseMove(const glm::ivec2 & local, const glm::ivec2 & global);
	private :
		typedef std::map<Keyboard, unsigned int> KeyboardMap;
		KeyboardMap mKeyboardStates;
		
		typedef std::map<Mouse, unsigned int> MouseMap;
		MouseMap mMouseStates;
		
		int mMouseWheel;
		glm::ivec2 mLocalMouseCoords;
		glm::ivec2 mGlobalMouseCoords;
};

typedef std::shared_ptr<InputManager> InputManagerPtr;
}

#endif
