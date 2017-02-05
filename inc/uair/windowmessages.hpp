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

#ifndef UAIRWINDOWMESSAGES_HPP
#define UAIRWINDOWMESSAGES_HPP

#include "init.hpp"
#include "messagequeue.hpp"
#include "inputenums.hpp"
#include "inputmanager.hpp"

namespace uair {
namespace WindowMessage {
class EXPORTDLL CloseMessage : public Message {
	public :
		void Serialise(cereal::BinaryOutputArchive& archive) const;
		void Serialise(cereal::BinaryInputArchive& archive);
		
		// return the unique typeid of this message (a constexpr means it can be used in switch statements)
		static constexpr unsigned int GetTypeID() {
			return 0u;
		}
};


class EXPORTDLL SizeMessage : public Message {
	public :
		// a default constructor means we can create a temporary blank object without specifying parameters
		SizeMessage() = default;
		
		SizeMessage(const unsigned int& type, const short& width, const short& height);
		
		void Serialise(cereal::BinaryOutputArchive& archive) const;
		void Serialise(cereal::BinaryInputArchive& archive);
		
		static constexpr unsigned int GetTypeID() {
			return 1u;
		}
	
	public :
		unsigned int mType;
		short mWidth;
		short mHeight;
};


class EXPORTDLL MoveMessage : public Message {
	public :
		MoveMessage() = default;
		MoveMessage(const short& x, const short& y);
		
		void Serialise(cereal::BinaryOutputArchive& archive) const;
		void Serialise(cereal::BinaryInputArchive& archive);
		
		static constexpr unsigned int GetTypeID() {
			return 2u;
		}
	
	public :
		short mX;
		short mY;
};


class EXPORTDLL GainedFocusMessage : public Message {
	public :
		void Serialise(cereal::BinaryOutputArchive& archive) const;
		void Serialise(cereal::BinaryInputArchive& archive);
		
		static constexpr unsigned int GetTypeID() {
			return 3u;
		}
};


class EXPORTDLL LostFocusMessage : public Message {
	public :
		void Serialise(cereal::BinaryOutputArchive& archive) const;
		void Serialise(cereal::BinaryInputArchive& archive);
		
		static constexpr unsigned int GetTypeID() {
			return 4u;
		}
};


class EXPORTDLL DisplayChangeMessage : public Message {
	public :
		DisplayChangeMessage() = default;
		DisplayChangeMessage(const short& width, const short& height);
		
		void Serialise(cereal::BinaryOutputArchive& archive) const;
		void Serialise(cereal::BinaryInputArchive& archive);
		
		static constexpr unsigned int GetTypeID() {
			return 5u;
		}
	
	public :
		short mWidth;
		short mHeight;
};


class EXPORTDLL LostCaptureMessage : public Message {
	public :
		void Serialise(cereal::BinaryOutputArchive& archive) const;
		void Serialise(cereal::BinaryInputArchive& archive);
		
		static constexpr unsigned int GetTypeID() {
			return 7u;
		}
};


class EXPORTDLL KeyboardKeyMessage : public Message {
	public :
		KeyboardKeyMessage() = default;
		KeyboardKeyMessage(const Keyboard& key, const unsigned int& type);
		
		void Serialise(cereal::BinaryOutputArchive& archive) const;
		void Serialise(cereal::BinaryInputArchive& archive);
		
		static constexpr unsigned int GetTypeID() {
			return 8u;
		}
	
	public :
		Keyboard mKey;
		unsigned int mType;
};


class EXPORTDLL TextInputMessage : public Message {
	public :
		TextInputMessage() = default;
		TextInputMessage(const char16_t& input);
		
		void Serialise(cereal::BinaryOutputArchive& archive) const;
		void Serialise(cereal::BinaryInputArchive& archive);
		
		static constexpr unsigned int GetTypeID() {
			return 9u;
		}
	
	public :
		char16_t mInput;
};


class EXPORTDLL MouseButtonMessage : public Message {
	public :
		MouseButtonMessage() = default;
		MouseButtonMessage(const Mouse& button, const unsigned int& type);
		
		void Serialise(cereal::BinaryOutputArchive& archive) const;
		void Serialise(cereal::BinaryInputArchive& archive);
		
		static constexpr unsigned int GetTypeID() {
			return 10u;
		}
	
	public :
		Mouse mButton;
		unsigned int mType;
};


class EXPORTDLL MouseWheelMessage : public Message {
	public :
		MouseWheelMessage() = default;
		MouseWheelMessage(const int& amount);
		
		void Serialise(cereal::BinaryOutputArchive& archive) const;
		void Serialise(cereal::BinaryInputArchive& archive);
		
		static constexpr unsigned int GetTypeID() {
			return 11u;
		}
	
	public :
		int mAmount;
};


class EXPORTDLL MouseMoveMessage : public Message {
	public :
		MouseMoveMessage() = default;
		MouseMoveMessage(const int& localX, const int& localY, const int& globalX, const int& globalY);
		
		void Serialise(cereal::BinaryOutputArchive& archive) const;
		void Serialise(cereal::BinaryInputArchive& archive);
		
		static constexpr unsigned int GetTypeID() {
			return 12u;
		}
	
	public :
		int mLocalX;
		int mLocalY;
		int mGlobalX;
		int mGlobalY;
};


class EXPORTDLL DeviceChangedMessage : public Message {
	public :
		DeviceChangedMessage() = default;
		DeviceChangedMessage(const int& id, const bool& status);
		DeviceChangedMessage(const int& id, const bool& status, const unsigned int& buttonCount,
				const unsigned int& controlCount, const std::vector<InputManager::InputDevice::ControlCaps>& caps);
		
		void Serialise(cereal::BinaryOutputArchive& archive) const;
		void Serialise(cereal::BinaryInputArchive& archive);
		
		static constexpr unsigned int GetTypeID() {
			return 13u;
		}
	
	public :
		int mID;
		bool mStatus;
		unsigned int mButtonCount;
		unsigned int mControlCount;
		std::vector<InputManager::InputDevice::ControlCaps> mCaps;
};


class EXPORTDLL DeviceButtonMessage : public Message {
	public :
		DeviceButtonMessage() = default;
		DeviceButtonMessage(const int& id, const unsigned int& button, const unsigned int& type);
		
		void Serialise(cereal::BinaryOutputArchive& archive) const;
		void Serialise(cereal::BinaryInputArchive& archive);
		
		static constexpr unsigned int GetTypeID() {
			return 14u;
		}
	
	public :
		int mID;
		unsigned int mButton;
		unsigned int mType;
};


class EXPORTDLL DeviceControlMessage : public Message {
	public :
		DeviceControlMessage() = default;
		DeviceControlMessage(const int& id, const Device& control, const int& value);
		
		void Serialise(cereal::BinaryOutputArchive& archive) const;
		void Serialise(cereal::BinaryInputArchive& archive);
		
		static constexpr unsigned int GetTypeID() {
			return 15u;
		}
	
	public :
		int mID;
		Device mControl;
		int mValue;
};
}
}

#endif
