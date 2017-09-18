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

#include "uielements.hpp"

Icon::Icon(const unsigned int& width, const unsigned int& height,
		const glm::vec3& colour, const bool& border) :
		mElement(width, height) {
	
	// add the initial quad shape
	mShape.AddContour(uair::Contour({glm::vec2(0.0f, 0.0f),
			glm::vec2(width, 0.0f), glm::vec2(width, height),
			glm::vec2(0.0f, height)}));
	mShape.SetColour(colour);
	mShape.SetDepth(-100.0f);
	
	if (border) { // if the icon is to have a border...
		// add the border shape and set the winding rule to only
		// render the interior (i.e., not border)
		mShape.AddContour(uair::Contour({glm::vec2(2.0f, 2.0f),
				glm::vec2(width - 2u, 2.0f),
				glm::vec2(width - 2u, height - 2u),
				glm::vec2(2.0f, height - 2u)}));
		mShape.SetWindingRule(uair::WindingRule::AbsGeqTwo);
	}
}


ResourceBar::ResourceBar(const glm::vec3& colour,
		const uair::Keyboard& hotkey) : mHotkey(hotkey),
		mElement(240u, 10u) {
	
	mShape.AddContour(uair::Contour({glm::vec2(0.0f, 0.0f),
			glm::vec2(240.0f, 0.0f), glm::vec2(240.0f, 10.0f),
			glm::vec2(0.0f, 10.0f)}));
	mShape.SetColour(colour);
	mShape.SetDepth(-100.0f);
}

void ResourceBar::Input() {
	if (GAME.GetKeyboardPressed(mHotkey)) { // if the registered hotkey
		// is pressed...
		
		if (mValue > 0u) { // if the bar isn't empty...
			// decrease the bar size by 10% (or to 0% if less than
			// 10% remains) and activate the delayed refill timer
			mValue -= std::min(mValue, 10u);
			mTimerDelay = 1.0f;
			
			// calculate the new width of the bar shape and update
			// the bar shape with the new value
			float width = 240.0f * (mValue / 100.0f);
			uair::Shape shape;
			shape.AddContour(uair::Contour({glm::vec2(0.0f, 0.0f),
					glm::vec2(width, 0.0f), glm::vec2(width, 10.0f),
					glm::vec2(0.0f, 10.0f)}));
			shape.SetColour(mShape.GetColour());
			shape.SetDepth(-100.0f);
			shape.SetPosition(mShape.GetPosition());
			mShape = std::move(shape);
		}
	}
}

void ResourceBar::Process(float deltaTime) {
	if (mValue < 100u) { // if the bar isn't full...
		if (mTimerDelay <= 0.0f) { // if the refill delay has elapsed...
			mTimerRefill += deltaTime;
			while (mTimerRefill >= 0.05f) {
				mValue += 1u;
				mTimerRefill -= 0.05f;
				
				if (mValue == 100u) {
					mTimerRefill = 0.0f;
				}
			}
			
			float width = 240.0f * (mValue / 100.0f);
			uair::Shape shape;
			shape.AddContour(uair::Contour({glm::vec2(0.0f, 0.0f),
					glm::vec2(width, 0.0f), glm::vec2(width, 10.0f),
					glm::vec2(0.0f, 10.0f)}));
			shape.SetColour(mShape.GetColour());
			shape.SetDepth(-100.0f);
			shape.SetPosition(mShape.GetPosition());
			mShape = std::move(shape);
		}
		else {
			mTimerDelay -= deltaTime;
		}
	}
}


HotbarIcon::HotbarIcon(const glm::vec3& colour,
		const uair::Keyboard& hotkey, const bool& shift) :
		mHotkey(hotkey), mShift(shift), mElement(48u, 48u) {
	
	mShape.AddContour(uair::Contour({glm::vec2(0.0f, 0.0f),
			glm::vec2(48.0f, 0.0f), glm::vec2(48.0f, 48.0f),
			glm::vec2(0.0f, 48.0f)}));
	mShape.SetColour(colour);
	mShape.SetDepth(-100.0f);
}

void HotbarIcon::Input() {
	// if the registered hotkey is pressed (including the shift
	// modifier if required)...
	if (GAME.GetKeyboardPressed(mHotkey) &&
			
			((!mShift &&
			(!GAME.GetKeyboardDown(uair::Keyboard::LShift) &&
			!GAME.GetKeyboardDown(uair::Keyboard::RShift))) ||
			
			((mShift &&
			(GAME.GetKeyboardDown(uair::Keyboard::LShift) ||
			GAME.GetKeyboardDown(uair::Keyboard::RShift)))))) {
		
		if (mAvailable) { // if the hotbar icon is not on cooldown...
			// 'activate' the hotbar icon
			mAvailable = false;
			mShape.SetAlpha(0.5f);
			mTimer = 5.0f;
		}
	}
}

void HotbarIcon::Process(float deltaTime) {
	if (!mAvailable && mTimer > 0.0f) { // if the hotbar icon is on
		// cooldown and the timer hasn't expired...
		
		mTimer -= deltaTime;
		if (mTimer <= 0.0f) {
			mAvailable = true;
			mShape.SetAlpha(1.0f);
		}
	}
}
