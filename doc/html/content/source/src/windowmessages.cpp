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

#include "windowmessages.hpp"

namespace uair {
namespace WindowMessage {
void CloseMessage::Serialise(cereal::BinaryOutputArchive& archive) const {
	archive(0); // nothing to deserialise for this message
}

void CloseMessage::Serialise(cereal::BinaryInputArchive& archive) {
	archive(0);
}


SizeMessage::SizeMessage(const unsigned int& type, const short& width, const short& height) :
		mType(type), mWidth(width), mHeight(height) {
	
	
}

void SizeMessage::Serialise(cereal::BinaryOutputArchive& archive) const {
	archive(mType, mWidth, mHeight);
}

void SizeMessage::Serialise(cereal::BinaryInputArchive& archive) {
	archive(mType, mWidth, mHeight);
}


MoveMessage::MoveMessage(const short& x, const short& y) :
		mX(x), mY(y) {
	
	
}

void MoveMessage::Serialise(cereal::BinaryOutputArchive& archive) const {
	archive(mX, mY);
}

void MoveMessage::Serialise(cereal::BinaryInputArchive& archive) {
	archive(mX, mY);
}


void GainedFocusMessage::Serialise(cereal::BinaryOutputArchive& archive) const {
	archive(0);
}

void GainedFocusMessage::Serialise(cereal::BinaryInputArchive& archive) {
	archive(0);
}


void LostFocusMessage::Serialise(cereal::BinaryOutputArchive& archive) const {
	archive(0);
}

void LostFocusMessage::Serialise(cereal::BinaryInputArchive& archive) {
	archive(0);
}


DisplayChangeMessage::DisplayChangeMessage(const short& width, const short& height) :
		mWidth(width), mHeight(height) {
	
	
}

void DisplayChangeMessage::Serialise(cereal::BinaryOutputArchive& archive) const {
	archive(mWidth, mHeight);
}

void DisplayChangeMessage::Serialise(cereal::BinaryInputArchive& archive) {
	archive(mWidth, mHeight);
}


void LostCaptureMessage::Serialise(cereal::BinaryOutputArchive& archive) const {
	archive(0);
}

void LostCaptureMessage::Serialise(cereal::BinaryInputArchive& archive) {
	archive(0);
}


KeyboardKeyMessage::KeyboardKeyMessage(const Keyboard& key, const unsigned int& type) :
		mKey(key), mType(type) {
	
	
}

void KeyboardKeyMessage::Serialise(cereal::BinaryOutputArchive& archive) const {
	archive(mKey, mType);
}

void KeyboardKeyMessage::Serialise(cereal::BinaryInputArchive& archive) {
	archive(mKey, mType);
}


TextInputMessage::TextInputMessage(const char16_t& input) :
		mInput(input) {
	
	
}

void TextInputMessage::Serialise(cereal::BinaryOutputArchive& archive) const {
	archive(mInput);
}

void TextInputMessage::Serialise(cereal::BinaryInputArchive& archive) {
	archive(mInput);
}


MouseButtonMessage::MouseButtonMessage(const Mouse& button, const unsigned int& type) :
		mButton(button), mType(type) {
	
	
}

void MouseButtonMessage::Serialise(cereal::BinaryOutputArchive& archive) const {
	archive(mButton, mType);
}

void MouseButtonMessage::Serialise(cereal::BinaryInputArchive& archive) {
	archive(mButton, mType);
}


MouseWheelMessage::MouseWheelMessage(const int& amount) :
		mAmount(amount) {
	
	
}

void MouseWheelMessage::Serialise(cereal::BinaryOutputArchive& archive) const {
	archive(mAmount);
}

void MouseWheelMessage::Serialise(cereal::BinaryInputArchive& archive) {
	archive(mAmount);
}


MouseMoveMessage::MouseMoveMessage(const int& localX, const int& localY, const int& globalX, const int& globalY) :
		mLocalX(localX), mLocalY(localY), mGlobalX(globalX), mGlobalY(globalY) {
	
	
}

void MouseMoveMessage::Serialise(cereal::BinaryOutputArchive& archive) const {
	archive(mLocalX, mLocalY, mGlobalX, mGlobalY);
}

void MouseMoveMessage::Serialise(cereal::BinaryInputArchive& archive) {
	archive(mLocalX, mLocalY, mGlobalX, mGlobalY);
}


DeviceChangedMessage::DeviceChangedMessage(const int& id, const bool& status) :
		mID(id), mStatus(status) {
	
	
}

DeviceChangedMessage::DeviceChangedMessage(const int& id, const bool& status, const unsigned int& buttonCount,
		const unsigned int& controlCount, const std::vector<InputManager::InputDevice::ControlCaps>& caps) :
		mID(id), mStatus(status), mButtonCount(buttonCount), mControlCount(controlCount),
		mCaps(caps) {
	
	
}

void DeviceChangedMessage::Serialise(cereal::BinaryOutputArchive& archive) const {
	archive(mID, mStatus, mButtonCount, mControlCount, mCaps);
}

void DeviceChangedMessage::Serialise(cereal::BinaryInputArchive& archive) {
	archive(mID, mStatus, mButtonCount, mControlCount, mCaps);
}


DeviceButtonMessage::DeviceButtonMessage(const int& id, const unsigned int& button, const unsigned int& type) :
		mID(id), mButton(button), mType(type) {
	
	
}

void DeviceButtonMessage::Serialise(cereal::BinaryOutputArchive& archive) const {
	archive(mID, mButton, mType);
}

void DeviceButtonMessage::Serialise(cereal::BinaryInputArchive& archive) {
	archive(mID, mButton, mType);
}


DeviceControlMessage::DeviceControlMessage(const int& id, const Device& control, const int& value) :
		mID(id), mControl(control), mValue(value) {
	
	
}

void DeviceControlMessage::Serialise(cereal::BinaryOutputArchive& archive) const {
	archive(mID, mControl, mValue);
}

void DeviceControlMessage::Serialise(cereal::BinaryInputArchive& archive) {
	archive(mID, mControl, mValue);
}
}
}
