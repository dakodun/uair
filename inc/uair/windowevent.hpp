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

#ifndef UAIRWINDOWEVENT_HPP
#define UAIRWINDOWEVENT_HPP

#include <map>

#include "inputenums.hpp"
#include "inputmanager.hpp"

namespace uair {
class WindowEvent {
	public :
		enum Type { // the types this message can be
			CloseType,
			SizeType,
			MoveType,
			GainedFocusType,
			LostFocusType,
			DisplayChangeType,
			GainedCaptureType,
			LostCaptureType,
			KeyboardKeyType,
			MouseButtonType,
			MouseWheelType,
			MouseMoveType,
			DeviceChangedType,
			DeviceButtonType,
			DeviceControlType
		};
		
		struct Close {
			
		};
		
		struct Size {
			unsigned int mType;
			short mWidth;
			short mHeight;
		};
		
		struct Move {
			short mX;
			short mY;
		};
		
		struct GainedFocus {
			
		};
		
		struct LostFocus {
			
		};
		
		struct DisplayChange {
			short mWidth;
			short mHeight;
		};
		
		struct GainedCapture {
			
		};
		
		struct LostCapture {
			
		};
		
		struct KeyboardKey {
			Keyboard mKey;
			unsigned int mType;
		};
		
		struct MouseButton {
			Mouse mButton;
			unsigned int mType;
		};
		
		struct MouseWheel {
			int mAmount;
		};
		
		struct MouseMove {
			int mLocalX;
			int mLocalY;
			int mGlobalX;
			int mGlobalY;
		};
		
		struct DeviceChanged { // an input device has been connect or disconnected
			int mID; // the id of the input device
			bool mStatus; // the type of event (connected or disconnected)
			unsigned int mButtonCount; // the number of buttons the input device reports
			unsigned int mControlCount; // the number of controls the input device reports
			InputManager::InputDeviceCaps mCaps; // an array of the controls present on the input device and their ranges and link ids
		};
		
		struct DeviceButton { // a button on an input device has been pressed or released
			unsigned int mButton; // the id of the button
			unsigned int mType; // the type of event (pressed or released)
			int mID; // the id of the input device
		};
		
		struct DeviceControl { // the value of a control on an input device has changed
			Device mControl; // the type of control
			int mValue; // the value of the control
			int mID; // the id of the input device
		};
	public :
		Type mType; // the type that this message is
		
		union { // the data supplied with this message
			Close mClose;
			Size mSize;
			Move mMove;
			GainedFocus mGainedFocus;
			LostFocus mLostFocus;
			DisplayChange mDisplayChange;
			GainedCapture mGainedCapture;
			LostCapture mLostCapture;
			KeyboardKey mKeyboardKey;
			MouseButton mMouseButton;
			MouseWheel mMouseWheel;
			MouseMove mMouseMove;
			DeviceChanged mDeviceChanged;
			DeviceButton mDeviceButton;
			DeviceControl mDeviceControl;
		};
};
}

#endif
