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

#include "inputmanager.hpp"

#include <iostream>

#include "window.hpp"

namespace uair {
std::weak_ptr<Window> InputManager::mWindowPtr = std::weak_ptr<Window>();

InputManager::InputDevice::InputDevice() : mButtonCount(0u), mControlCount(0u) {
	
}

InputManager::InputDevice::InputDevice(const unsigned int& buttonCount, const unsigned int& controlCount,
		const InputDeviceCaps& caps) : mButtonCount(buttonCount), mControlCount(controlCount) {
	
	for (unsigned int i = 0u; i < buttonCount; ++i) { // for all buttons...
		mButtonStates.emplace(i, 0); // add a state value to the store
	}
	
	for (unsigned int i = 0u; i < controlCount; ++i) { // for all controls...
		mControlCaps.emplace(caps.mControls.at(i).mDevice, caps.mControls.at(i)); // store the capabilities of the control
		mControlStates.emplace(caps.mControls.at(i).mDevice, 0); // add a value to the store
		
		std::vector<Device> tempLinkCollection; // create a new link collection
		auto linkCollection = mLinkCollections.emplace(caps.mControls.at(i).mCollectionID, std::move(tempLinkCollection)); // add it to the store or retrieve existing entry
		linkCollection.first->second.emplace_back(caps.mControls.at(i).mDevice); // add the control to the link collection
	}
}

unsigned int InputManager::InputDevice::GetButtonCount() const {
	return mButtonCount; // return the number of buttons reported by the device
}

bool InputManager::InputDevice::GetButtonDown(const unsigned int& button) const {
	auto buttonState = mButtonStates.find(button); // get the state of the button
	if (buttonState != mButtonStates.end()) { // if the button was valid...
		if (buttonState->second == 1 || buttonState->second == 2) { // if the state is down or pressed...
			return true; // button is down
		}
	}
	
	return false; // button is not down
}

bool InputManager::InputDevice::GetButtonPressed(const unsigned int& button) const {
	auto buttonState = mButtonStates.find(button); // get the state of the button
	if (buttonState != mButtonStates.end()) { // if the button was valid...
		if (buttonState->second == 2) { // if the state is pressed...
			return true; // button is pressed
		}
	}
	
	return false; // button is not pressed
}

bool InputManager::InputDevice::GetButtonReleased(const unsigned int& button) const {
	auto buttonState = mButtonStates.find(button); // get the state of the button
	if (buttonState != mButtonStates.end()) { // if the button was valid...
		if (buttonState->second == 3) { // if the state is released...
			return true; // button is released
		}
	}
	
	return false; // button is not released
}

unsigned int InputManager::InputDevice::GetButtonState(const unsigned int& button) const {
	auto buttonState = mButtonStates.find(button); // get the state of the button
	if (buttonState != mButtonStates.end()) { // if the button was valid...
		return buttonState->second; // return the current state of the button
	}
	
	return 0u; // otherwise return default of 'up'
}

unsigned int InputManager::InputDevice::GetControlCount() const {
	return mControlCount; // return the number of controls reported by the device
}

bool InputManager::InputDevice::HasControl(const Device& control) const {
	auto controlState = mControlStates.find(control); // try to get the state of the control
	if (controlState != mControlStates.end()) { // if the control was valid...
		return true; // the control exists
	}
	
	return false; // the control doesn't exist
}

int InputManager::InputDevice::GetControl(const Device& control) const {
	auto controlState = mControlStates.find(control); // try to get the state of the control
	if (controlState != mControlStates.end()) { // if the control was valid...
		return controlState->second; // return the current value of the control
	}
	
	return 0; // return 0 (invalid control)
}

int InputManager::InputDevice::GetControlScaled(const Device& control, std::pair<int, int> range) const {
	auto controlState = mControlStates.find(control); // try to get the state of the control
	if (controlState != mControlStates.end()) { // if the control was valid...
		auto controlRange = mControlCaps.find(control);
		if (controlRange != mControlCaps.end()) { // if the control was valid...
			int rangeMin = controlRange->second.mLowerRange; // get the control's upper range
			int rangeMax = controlRange->second.mUpperRange; // get the control's lower range
			float scaleFactor = (controlState->second - rangeMin) / static_cast<float>(rangeMax - rangeMin); // calculate the factor required to scale the value by
			
			int scaledValue = (scaleFactor * (range.second - range.first)) + range.first; // scale the value into the range specified
			
			return scaledValue; // return the new scaled value
		}
	}
	
	return 0; // return 0 (invalid control)
}

std::pair<int, int> InputManager::InputDevice::GetControlRange(const Device& control) const {
	auto controlRange = mControlCaps.find(control); // try to get the range of the control
	if (controlRange != mControlCaps.end()) { // if the control was valid...
		return std::make_pair(controlRange->second.mLowerRange, controlRange->second.mUpperRange); // return the range of the control
	}
	
	return std::make_pair(0, 0); // return (0, 0) (invalid control)
}

std::vector<Device> InputManager::InputDevice::GetLinkedDevices(const unsigned int& collectionID) const {
	auto collection = mLinkCollections.find(collectionID); // get the collection associated with the specified id
	if (collection != mLinkCollections.end()) { // if the collection id was valid...
		return collection->second; // return the collection
	}
	
	return {}; // return empty collection
}

unsigned int InputManager::InputDevice::GetLinkID(const Device& control) const {
	if (HasControl(control)) { // if the control exists...
		auto controlLinkID = mControlCaps.find(control); // try to get the link id of the control
		if (controlLinkID != mControlCaps.end()) { // if the control was valid...
			return controlLinkID->second.mCollectionID; // return the link id of the control
		}
	}
	
	return 0;
}

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
		if (iter->second == 2u) { // if state is pressed...
			iter->second = 1u; // it is now down
		}
		else if (iter->second == 3u) { // if state is released...
			iter->second = 0u; // it is now up
		}
	}
	
	for (auto iter = mMouseStates.begin(); iter != mMouseStates.end(); ++iter) { // update all mouse states
		if (iter->second == 2u) { // if state is pressed...
			iter->second = 1u; // it is now down
		}
		else if (iter->second == 3u) { // if state is released...
			iter->second = 0u; // it is now up
		}
	}
	
	mMouseWheel = 0;
	
	for (auto device = mInputDevices.begin(); device != mInputDevices.end(); ++device) {
		for (auto iter = device->second.mButtonStates.begin(); iter != device->second.mButtonStates.end(); ++iter) { // update all device button states
			if (iter->second == 2u) { // if state is pressed...
				iter->second = 1u; // it is now down
			}
			else if (iter->second == 3u) { // if state is released...
				iter->second = 0u; // it is now up
			}
		}
	}
	
	mInputString = u"";
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

unsigned int InputManager::GetKeyboardState(const Keyboard& key) const {
	auto iter = mKeyboardStates.find(key); // get the state of the key
	if (iter != mKeyboardStates.end()) { // if the key was valid
		return iter->second; // return the current state of the key
	}
	
	return 0u; // otherwise return default of 'up'
}

std::u16string InputManager::GetInputString() const {
	return mInputString;
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

unsigned int InputManager::GetMouseState(const Mouse& button) const {
	auto iter = mMouseStates.find(button); // get the state of the button
	if (iter != mMouseStates.end()) { // if the button was valid
		return iter->second; // return the current state of the button
	}
	
	return 0u; // otherwise return default of 'up'
}

int InputManager::GetMouseWheel() const {
	return mMouseWheel;
}

void InputManager::SetMouseCoords(const glm::ivec2& newCoords, const CoordinateSpace& coordinateSpace) {
	if (auto mSharedWindowPtr = mWindowPtr.lock()) {
		mSharedWindowPtr->SetMouseCoords(newCoords, coordinateSpace);
	}
	else {
		std::cout << "pointer to window object is invalid" << std::endl;
	}
}

glm::ivec2 InputManager::GetMouseCoords(const CoordinateSpace& coordinateSpace) const {
	if (auto mSharedWindowPtr = mWindowPtr.lock()) {
		if (coordinateSpace == CoordinateSpace::Global) {
			return mSharedWindowPtr->GetMouseCoords().second;
		}
		
		return mSharedWindowPtr->GetMouseCoords().first;
	}
	else {
		std::cout << "pointer to window object is invalid" << std::endl;
		return glm::ivec2(0, 0);
	}
}

bool InputManager::DeviceExists(const unsigned int& deviceID) const {
	auto device = mInputDevices.find(deviceID); // get the input device associate with the specified id
	if (device != mInputDevices.end()) { // if the input device id was valid...
		return true; // the device exists
	}
	
	return false; // the device doesn't exist
}

const InputManager::InputDevice& InputManager::GetDevice(const unsigned int& deviceID) const {
	auto device = mInputDevices.find(deviceID); // get the input device associate with the specified id
	if (device != mInputDevices.end()) { // if the input device id was valid...
		return device->second; // return the requested device
	}
	
	return mDefaultDevice; // return the default device
}

unsigned int InputManager::GetDeviceButtonCount(const int& deviceID) const {
	return GetDevice(deviceID).GetButtonCount();
}

bool InputManager::GetDeviceButtonDown(const int& deviceID, const unsigned int& button) const {
	return GetDevice(deviceID).GetButtonDown(button);
}

bool InputManager::GetDeviceButtonPressed(const int& deviceID, const unsigned int& button) const {
	return GetDevice(deviceID).GetButtonPressed(button);
}

bool InputManager::GetDeviceButtonReleased(const int& deviceID, const unsigned int& button) const {
	return GetDevice(deviceID).GetButtonReleased(button);
}

unsigned int InputManager::GetDeviceButtonState(const int& deviceID, const unsigned int& button) const {
	return GetDevice(deviceID).GetButtonState(button);
}

unsigned int InputManager::GetDeviceControlCount(const int& deviceID) const {
	return GetDevice(deviceID).GetControlCount();
}

bool InputManager::DeviceHasControl(const int& deviceID, const Device& control) const {
	return GetDevice(deviceID).HasControl(control);
}

int InputManager::GetDeviceControl(const int& deviceID, const Device& control) const {
	return GetDevice(deviceID).GetControl(control);
}

int InputManager::GetDeviceControlScaled(const int& deviceID, const Device& control, std::pair<int, int> range) const {
	return GetDevice(deviceID).GetControlScaled(control, range);
}

std::pair<int, int> InputManager::GetDeviceControlRange(const int& deviceID, const Device& control) const {
	return GetDevice(deviceID).GetControlRange(control);
}

std::vector<Device> InputManager::GetDeviceLinkedDevices(const int& deviceID, const unsigned int& collectionID) const {
	return GetDevice(deviceID).GetLinkedDevices(collectionID);
}

unsigned int InputManager::GetDeviceLinkID(const int& deviceID, const Device& control) const {
	return GetDevice(deviceID).GetLinkID(control);
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
	
	mMouseWheel = 0;
	
	for (auto device = mInputDevices.begin(); device != mInputDevices.end(); ++device) {
		for (auto iter = device->second.mButtonStates.begin(); iter != device->second.mButtonStates.end(); ++iter) { // update all device button states
			if (iter->second == 1u) { // if state is down
				iter->second = 3u; // it is now released
			}
		}
	}
	
	mInputString = u"";
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

void InputManager::HandleTextInput(const char16_t& input) {
	mInputString += input;
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

void InputManager::AddDevice(const int& deviceID, const unsigned int& buttonCount, const unsigned int& controlCount,
		const InputDeviceCaps& caps) {
	
	InputDevice inputDevice(buttonCount, controlCount, caps); // create a new input device
	mInputDevices.emplace(deviceID, std::move(inputDevice)); // add the new input device to the store
}

void InputManager::RemoveDevice(const int& deviceID) {
	auto device = mInputDevices.find(deviceID); // get the input device associated with the specified id
	if (device != mInputDevices.end()) { // if the input device exists...
		mInputDevices.erase(device); // remove the input device from the store
	}
}

void InputManager::HandleDeviceButtons(const unsigned int& button, const unsigned int& type, const int& deviceID) {
	auto device = mInputDevices.find(deviceID); // get the input device associated with the specified id
	if (device != mInputDevices.end()) { // if the input device exists...
		auto buttonState = device->second.mButtonStates.find(button); // get the state of the button requested
		if (buttonState != device->second.mButtonStates.end()) { // if the button was valid...
			switch (type) {
				case 0 : // down
					if (buttonState->second == 0 || buttonState->second == 3) { // if state is up or released
						buttonState->second = 2; // state is now pressed (note: not down)
					}
					
					break;
				case 1 : // up
					if (buttonState->second == 1 || buttonState->second == 2) { // if state is down or pressed
						buttonState->second = 3; // state is now released (note: not up)
					}
					
					break;
			}
		}
	}
}

void InputManager::HandleDeviceControls(const Device& control, const int& value, const int& deviceID) {
	auto device = mInputDevices.find(deviceID); // get the input device associated with the specified id
	if (device != mInputDevices.end()) { // if the input device exists...
		auto controlState = device->second.mControlStates.find(control); // get the value of the control requested
		if (controlState != device->second.mControlStates.end()) { // if the control was valid...
			controlState->second = value; // update the control's value
		}
	}
}
}
