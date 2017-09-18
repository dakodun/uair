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

#ifndef UIELEMENTS_HPP
#define UIELEMENTS_HPP

#include "init.hpp"

// a simple icon (quad) with optional border
class Icon {
	public :
		Icon() = default;
		
		Icon(const unsigned int& width, const unsigned int& height,
				const glm::vec3& colour, const bool& border = false);
	
	public :
		// shape representing the icon
		uair::Shape mShape;
		
		// element used for positioning within layout object
		uair::LayoutElement mElement;
};

// a bar that can be reduced and refills
class ResourceBar {
	public :
		ResourceBar(const glm::vec3& colour, const uair::Keyboard& hotkey);
		
		void Input();
		void Process(float deltaTime);
	
	public :
		// the hotkey used to reduce the bar
		uair::Keyboard mHotkey;
		
		// the current amount
		unsigned int mValue = 100u;
		
		// timer used to delay bar refill after a reduction
		float mTimerDelay = 0.0f;
		
		// timer used to slowly refill the bar
		float mTimerRefill = 0.0f;
		
		uair::Shape mShape;
		
		uair::LayoutElement mElement;
};

// an icon that can be activated (with a cooldown)
class HotbarIcon {
	public :
		HotbarIcon(const glm::vec3& colour, const uair::Keyboard& hotkey,
				const bool& shift = false);
		
		void Input();
		void Process(float deltaTime);
	
	public :
		uair::Keyboard mHotkey;
		
		// if the hotkey uses a shift modifier
		bool mShift = false;
		
		// is the hotbar icon on cooldown or not
		bool mAvailable = true;
		
		// timer used to reset the hotbar icon after press
		float mTimer = -1.0f;
		
		uair::Shape mShape;
		
		uair::LayoutElement mElement;
};

#endif
