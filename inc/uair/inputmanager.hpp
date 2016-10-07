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

#ifndef UAIRINPUTMANAGER_HPP
#define UAIRINPUTMANAGER_HPP

#include <memory>
#include <vector>
#include <map>
#include <set>
#include <glm/glm.hpp>

#include "polygon.hpp"
#include "inputenums.hpp"

namespace uair {
class Window;

class InputManager {
	friend class Game;
	
	public :
		struct InputDeviceCaps { // a store containing the controls that an input device possesses
			struct ControlCaps { // the capabilities of an individual control
				Device mDevice; // the id of the control
				int mLowerRange; // the lower end of the input device's value range
				int mUpperRange; // the upper end of the input device's value range
				unsigned int mCollectionID; // the id of the link collection that the control belongs to
			};
			
			std::array<ControlCaps, DEVICECOUNT> mControls; // an array that holds the capabilities for all controls present on the device
		};
		
		class InputDevice { // an input device connected to the system
			friend class InputManager;
			
			public :
				InputDevice();
				InputDevice(const unsigned int& buttonCount, const unsigned int& controlCount,
						const InputDeviceCaps& caps);
				
				unsigned int GetButtonCount() const; // get the number of buttons reported by the input device
				bool GetButtonDown(const unsigned int& button) const; // returns true if the button is held down
				bool GetButtonPressed(const unsigned int& button) const; // returns true if the button has been pressed (fires once)
				bool GetButtonReleased(const unsigned int& button) const; // returns true if the button has been released (fires once)
				unsigned int GetButtonState(const unsigned int& button) const;
				
				unsigned int GetControlCount() const; // get the number of controls reported by the input device
				bool HasControl(const Device& control) const; // check if the specified control is present on the input device
				int GetControl(const Device& control) const; // returns the value of the specified control
				int GetControlScaled(const Device& control, std::pair<int, int> range = std::make_pair(0, 255)) const; // returns the value of the specified control scaled into the range specified
				std::pair<int, int> GetControlRange(const Device& control) const; // returns the range of the specified control
				
				std::vector<Device> GetLinkedDevices(const unsigned int& collectionID) const; // get an array of all controls sharing the specified link id (if any)
				unsigned int GetLinkID(const Device& control) const; // returns the link id associated with the specified control
			private :
				unsigned int mButtonCount; // the reported number of buttons present on the input device
				unsigned int mControlCount; // the reported number of controls present on the input device
				std::map<Device, InputDeviceCaps::ControlCaps> mControlCaps; // the capabilities of controls present on the device
				
				std::map<unsigned int, unsigned int> mButtonStates; // the current state of all buttons
				std::map<Device, int> mControlStates; // the current value of all controls
				
				std::map<unsigned int, std::vector<Device> > mLinkCollections; // all present controls grouped by their link id
		};
	public :
		InputManager();
		
		void Process();
		
		bool GetKeyboardDown(const Keyboard& key) const;
		bool GetKeyboardPressed(const Keyboard& key) const;
		bool GetKeyboardReleased(const Keyboard& key) const;
		unsigned int GetKeyboardState(const Keyboard& key) const;
		
		std::u16string GetInputString() const;
		
		bool GetMouseDown(const Mouse& button) const;
		bool GetMousePressed(const Mouse& button) const;
		bool GetMouseReleased(const Mouse& button) const;
		unsigned int GetMouseState(const Mouse& button) const;
		
		int GetMouseWheel() const;
		void SetMouseCoords(const glm::ivec2& newCoords, const CoordinateSpace& coordinateSpace = CoordinateSpace::Local);
		glm::ivec2 GetMouseCoords(const CoordinateSpace& coordinateSpace = CoordinateSpace::Local) const;
		
		bool DeviceExists(const unsigned int& deviceID) const; // returns true if the input device with the specified id exists
		const InputDevice& GetDevice(const unsigned int& deviceID) const; // returns the input device with the associated id (or the default device if id is invalid)
		
		unsigned int GetDeviceButtonCount(const int& deviceID) const; // helper function
		bool GetDeviceButtonDown(const int& deviceID, const unsigned int& button) const; // helper function
		bool GetDeviceButtonPressed(const int& deviceID, const unsigned int& button) const; // helper function
		bool GetDeviceButtonReleased(const int& deviceID, const unsigned int& button) const; // helper function
		unsigned int GetDeviceButtonState(const int& deviceID, const unsigned int& button) const;
		
		unsigned int GetDeviceControlCount(const int& deviceID) const; // helper function
		bool DeviceHasControl(const int& deviceID, const Device& control) const; // helper function
		int GetDeviceControl(const int& deviceID, const Device& control) const; // helper function
		int GetDeviceControlScaled(const int& deviceID, const Device& control, std::pair<int, int> range = std::make_pair(0, 255)) const; // helper function
		std::pair<int, int> GetDeviceControlRange(const int& deviceID, const Device& control) const; // helper function
		
		std::vector<Device> GetDeviceLinkedDevices(const int& deviceID, const unsigned int& collectionID) const; // helper function
		unsigned int GetDeviceLinkID(const int& deviceID, const Device& control) const; // helper function
	private :
		void Reset();
		
		void HandleKeyboardKeys(const Keyboard& key, const unsigned int& type);
		
		void HandleTextInput(const char16_t& input);
		
		void HandleMouseButtons(const Mouse& button, const unsigned int& type);
		
		void AddDevice(const int& deviceID, const unsigned int& buttonCount, const unsigned int& controlCount,
				const InputDeviceCaps& caps); // adds an input device to the store when it is connected to the system
		void RemoveDevice(const int& deviceID); // removes an input device from the store when it is disconnected from the system
		void HandleDeviceButtons(const unsigned int& button, const unsigned int& type, const int& deviceID); // handles the updating of the states of input device controls
		void HandleDeviceControls(const Device& control, const int& value, const int& deviceID); // handles the updating of the states of input device controls
	
	public :
		static std::weak_ptr<Window> mWindowPtr;
	private :
		std::map<Keyboard, unsigned int> mKeyboardStates;
		
		std::map<Mouse, unsigned int> mMouseStates;
		
		int mMouseWheel;
		
		std::map<int, InputDevice> mInputDevices; // a store of all input devices currently connected to the system
		InputDevice mDefaultDevice; // the default device that is used when an invalid device is requested
		
		std::u16string mInputString;
};

typedef std::shared_ptr<InputManager> InputManagerPtr;
}

#endif
