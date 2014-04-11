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

#include "inputmanager.hpp"

#include <iostream>

namespace uair {
InputManager::InputManager() {
	mKeyboardStates.emplace(Keyboard::A, 0);
	mKeyboardStates.emplace(Keyboard::B, 0);
	mKeyboardStates.emplace(Keyboard::C, 0);
	mKeyboardStates.emplace(Keyboard::D, 0);
	mKeyboardStates.emplace(Keyboard::E, 0);
	mKeyboardStates.emplace(Keyboard::F, 0);
	mKeyboardStates.emplace(Keyboard::G, 0);
	mKeyboardStates.emplace(Keyboard::H, 0);
	mKeyboardStates.emplace(Keyboard::I, 0);
	mKeyboardStates.emplace(Keyboard::J, 0);
	mKeyboardStates.emplace(Keyboard::K, 0);
	mKeyboardStates.emplace(Keyboard::L, 0);
	mKeyboardStates.emplace(Keyboard::M, 0);
	mKeyboardStates.emplace(Keyboard::N, 0);
	mKeyboardStates.emplace(Keyboard::O, 0);
	mKeyboardStates.emplace(Keyboard::P, 0);
	mKeyboardStates.emplace(Keyboard::Q, 0);
	mKeyboardStates.emplace(Keyboard::R, 0);
	mKeyboardStates.emplace(Keyboard::S, 0);
	mKeyboardStates.emplace(Keyboard::T, 0);
	mKeyboardStates.emplace(Keyboard::U, 0);
	mKeyboardStates.emplace(Keyboard::V, 0);
	mKeyboardStates.emplace(Keyboard::W, 0);
	mKeyboardStates.emplace(Keyboard::X, 0);
	mKeyboardStates.emplace(Keyboard::Y, 0);
	mKeyboardStates.emplace(Keyboard::Z, 0);
	mKeyboardStates.emplace(Keyboard::Num0, 0);
	mKeyboardStates.emplace(Keyboard::Num1, 0);
	mKeyboardStates.emplace(Keyboard::Num2, 0);
	mKeyboardStates.emplace(Keyboard::Num3, 0);
	mKeyboardStates.emplace(Keyboard::Num4, 0);
	mKeyboardStates.emplace(Keyboard::Num5, 0);
	mKeyboardStates.emplace(Keyboard::Num6, 0);
	mKeyboardStates.emplace(Keyboard::Num7, 0);
	mKeyboardStates.emplace(Keyboard::Num8, 0);
	mKeyboardStates.emplace(Keyboard::Num9, 0);
	mKeyboardStates.emplace(Keyboard::Escape, 0);
	mKeyboardStates.emplace(Keyboard::LControl, 0);
	mKeyboardStates.emplace(Keyboard::LShift, 0);
	mKeyboardStates.emplace(Keyboard::LAlt, 0);
	mKeyboardStates.emplace(Keyboard::LSystem, 0);
	mKeyboardStates.emplace(Keyboard::RControl, 0);
	mKeyboardStates.emplace(Keyboard::RShift, 0);
	mKeyboardStates.emplace(Keyboard::RAlt, 0);
	mKeyboardStates.emplace(Keyboard::RSystem, 0);
	mKeyboardStates.emplace(Keyboard::Menu, 0);
	mKeyboardStates.emplace(Keyboard::LBracket, 0);
	mKeyboardStates.emplace(Keyboard::RBracket, 0);
	mKeyboardStates.emplace(Keyboard::SemiColon, 0);
	mKeyboardStates.emplace(Keyboard::Comma, 0);
	mKeyboardStates.emplace(Keyboard::FullStop, 0);
	mKeyboardStates.emplace(Keyboard::Quote, 0);
	mKeyboardStates.emplace(Keyboard::Slash, 0);
	mKeyboardStates.emplace(Keyboard::BackSlash, 0);
	mKeyboardStates.emplace(Keyboard::Tilde, 0);
	mKeyboardStates.emplace(Keyboard::Equal, 0);
	mKeyboardStates.emplace(Keyboard::Hyphen, 0);
	mKeyboardStates.emplace(Keyboard::Space, 0);
	mKeyboardStates.emplace(Keyboard::Return, 0);
	mKeyboardStates.emplace(Keyboard::Back, 0);
	mKeyboardStates.emplace(Keyboard::Tab, 0);
	mKeyboardStates.emplace(Keyboard::PageUp, 0);
	mKeyboardStates.emplace(Keyboard::PageDown, 0);
	mKeyboardStates.emplace(Keyboard::End, 0);
	mKeyboardStates.emplace(Keyboard::Home, 0);
	mKeyboardStates.emplace(Keyboard::Insert, 0);
	mKeyboardStates.emplace(Keyboard::Delete, 0);
	mKeyboardStates.emplace(Keyboard::Add, 0);
	mKeyboardStates.emplace(Keyboard::Subtract, 0);
	mKeyboardStates.emplace(Keyboard::Multiply, 0);
	mKeyboardStates.emplace(Keyboard::Divide, 0);
	mKeyboardStates.emplace(Keyboard::Left, 0);
	mKeyboardStates.emplace(Keyboard::Right, 0);
	mKeyboardStates.emplace(Keyboard::Up, 0);
	mKeyboardStates.emplace(Keyboard::Down, 0);
	mKeyboardStates.emplace(Keyboard::Numpad0, 0);
	mKeyboardStates.emplace(Keyboard::Numpad1, 0);
	mKeyboardStates.emplace(Keyboard::Numpad2, 0);
	mKeyboardStates.emplace(Keyboard::Numpad3, 0);
	mKeyboardStates.emplace(Keyboard::Numpad4, 0);
	mKeyboardStates.emplace(Keyboard::Numpad5, 0);
	mKeyboardStates.emplace(Keyboard::Numpad6, 0);
	mKeyboardStates.emplace(Keyboard::Numpad7, 0);
	mKeyboardStates.emplace(Keyboard::Numpad8, 0);
	mKeyboardStates.emplace(Keyboard::Numpad9, 0);
	mKeyboardStates.emplace(Keyboard::F1, 0);
	mKeyboardStates.emplace(Keyboard::F2, 0);
	mKeyboardStates.emplace(Keyboard::F3, 0);
	mKeyboardStates.emplace(Keyboard::F4, 0);
	mKeyboardStates.emplace(Keyboard::F5, 0);
	mKeyboardStates.emplace(Keyboard::F6, 0);
	mKeyboardStates.emplace(Keyboard::F7, 0);
	mKeyboardStates.emplace(Keyboard::F8, 0);
	mKeyboardStates.emplace(Keyboard::F9, 0);
	mKeyboardStates.emplace(Keyboard::F10, 0);
	mKeyboardStates.emplace(Keyboard::F11, 0);
	mKeyboardStates.emplace(Keyboard::F12, 0);
	mKeyboardStates.emplace(Keyboard::F13, 0);
	mKeyboardStates.emplace(Keyboard::F14, 0);
	mKeyboardStates.emplace(Keyboard::F15, 0);
	mKeyboardStates.emplace(Keyboard::Break, 0);
	mKeyboardStates.emplace(Keyboard::Unknown, 0);
	
	mMouseStates.emplace(Mouse::Left, 0);
	mMouseStates.emplace(Mouse::Middle, 0);
	mMouseStates.emplace(Mouse::Right, 0);
	mMouseStates.emplace(Mouse::M4, 0);
	mMouseStates.emplace(Mouse::M5, 0);
}

void InputManager::Process() {
	for (auto iter = mKeyboardStates.begin(); iter != mKeyboardStates.end(); ++iter) { // update all keyboard states
		if (iter->second == 2u) { // if state is pressed
			iter->second = 1u; // it is now down
		}
		else if (iter->second == 3u) { // if state is released
			iter->second = 0u; // it is now up
		}
	}
	
	for (auto iter = mMouseStates.begin(); iter != mMouseStates.end(); ++iter) { // update all mouse states
		if (iter->second == 2u) { // if state is pressed
			iter->second = 1u; // it is now down
		}
		else if (iter->second == 3u) { // if state is released
			iter->second = 0u; // it is now up
		}
	}
	
	mMouseWheel = 0;
}

bool InputManager::GetKeyboardDown(const Keyboard & key) const {
	auto iter = mKeyboardStates.find(key); // get the state of the key
	if (iter != mKeyboardStates.end()) { // if the key was valid
		if (iter->second == 1 || iter->second == 2) { // if the state is down or pressed
			return true; // key is down
		}
	}
	
	return false; // key is not down (released or up)
}

bool InputManager::GetKeyboardPressed(const Keyboard & key) const {
	auto iter = mKeyboardStates.find(key); // get the state of the key
	if (iter != mKeyboardStates.end()) { // if the key was valid
		if (iter->second == 2) { // if the state is pressed
			return true; // key was pressed
		}
	}
	
	return false; // key was not pressed
}

bool InputManager::GetKeyboardReleased(const Keyboard & key) const {
	auto iter = mKeyboardStates.find(key); // get the state of the key
	if (iter != mKeyboardStates.end()) { // if the key was valid
		if (iter->second == 3) { // if the state is released
			return true; // key was released
		}
	}
	
	return false; // key was not released
}

bool InputManager::GetMouseDown(const Mouse & button) const {
	auto iter = mMouseStates.find(button); // get the state of the button
	if (iter != mMouseStates.end()) { // if the button was valid
		if (iter->second == 1 || iter->second == 2) { // if the state is down or pressed
			return true; // button is down
		}
	}
	
	return false; // button is not down (released or up)
}

bool InputManager::GetMousePressed(const Mouse & button) const {
	auto iter = mMouseStates.find(button); // get the state of the button
	if (iter != mMouseStates.end()) { // if the button was valid
		if (iter->second == 2) { // if the state is pressed
			return true; // button was pressed
		}
	}
	
	return false; // button was not pressed
}

bool InputManager::GetMouseReleased(const Mouse & button) const {
	auto iter = mMouseStates.find(button); // get the state of the button
	if (iter != mMouseStates.end()) { // if the button was valid
		if (iter->second == 3) { // if the state is released
			return true; // button was released
		}
	}
	
	return false; // button was not released
}

int InputManager::GetMouseWheel() const {
	return mMouseWheel;
}

glm::ivec2 InputManager::GetLocalMouseCoords() const {
	return mLocalMouseCoords;
}

glm::ivec2 InputManager::GetGlobalMouseCoords() const {
	return mGlobalMouseCoords;
}

void InputManager::Reset() {
	for (auto iter = mKeyboardStates.begin(); iter != mKeyboardStates.end(); ++iter) { // update all keyboard states
		if (iter->second == 1u) { // if state is down
			iter->second = 3u; // it is now released
		}
	}
	
	for (auto iter = mMouseStates.begin(); iter != mMouseStates.end(); ++iter) { // update all mouse states
		if (iter->second == 1u) { // if state is down
			iter->second = 3u; // it is now released
		}
	}
}

void InputManager::HandleKeyboardKeys(const Keyboard & key, const unsigned int & type) {
	switch (type) {
		case 0 : // down
			if (mKeyboardStates[key] == 0 || mKeyboardStates[key] == 3) { // if state is up or released
				mKeyboardStates[key] = 2; // state is now pressed (note: not down)
			}
			
			break;
		case 1 : // up
			if (mKeyboardStates[key] == 1 || mKeyboardStates[key] == 2) { // if state is down or pressed
				mKeyboardStates[key] = 3; // state is now released (note: not up)
			}
			
			break;
	}
}

void InputManager::HandleMouseButtons(const Mouse & button, const unsigned int & type) {
	switch (type) {
		case 0 : // down
			if (mMouseStates[button] == 0 || mMouseStates[button] == 3) { // if state is up or released
				mMouseStates[button] = 2; // state is now pressed (note: not down)
			}
			
			break;
		case 1 : // up
			if (mMouseStates[button] == 1 || mMouseStates[button] == 2) { // if state is down or pressed
				mMouseStates[button] = 3; // state is now released (note: not up)
			}
			
			break;
		case 2 : // double-click (down)
			if (mMouseStates[button] == 0 || mMouseStates[button] == 3) { // if state is up or released
				mMouseStates[button] = 2; // state is now pressed (note: not down)
			}
			
			break;
	}
}

void InputManager::HandleMouseMove(const glm::ivec2 & local, const glm::ivec2 & global) {
	if (local.x != mLocalMouseCoords.x || local.y != mLocalMouseCoords.y) {
		mLocalMouseCoords.x = local.x;
		mLocalMouseCoords.y = local.y;
	}
	
	if (global.x != mGlobalMouseCoords.x ||global.y != mGlobalMouseCoords.y) {
		mGlobalMouseCoords.x = global.x;
		mGlobalMouseCoords.y = global.y;
	}
}
}
