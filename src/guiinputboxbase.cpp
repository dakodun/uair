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

#include "guiinputboxbase.hpp"

#include "windowmessages.hpp"

namespace uair {
void GUIInputBoxBase::HandleMessageQueue(const MessageQueue::Entry& e, GUI* caller) {
	using namespace WindowMessage;
	switch (e.GetTypeID()) {
		case MouseMoveMessage::GetTypeID() : {
			MouseMoveMessage msg = e.Deserialise<MouseMoveMessage>();
			
			// if the mouse cursor is inside the input box's bounding box...
			if (msg.mLocalX > mPosition.x && msg.mLocalX < mPosition.x + mWidth &&
					msg.mLocalY > mPosition.y && msg.mLocalY < mPosition.y + mHeight) {
				
				if (!mInArea) {
					mInArea = true;
					OnHoverChange();
				}
			}
			else {
				if (mInArea) {
					mInArea = false;
					OnHoverChange();
				}
			}
			
			break;
		}
		case TextInputMessage::GetTypeID() : {
			TextInputMessage msg = e.Deserialise<TextInputMessage>();
			
			if (mActive) {
				if (msg.mInput == 8) {
					if (!mString.empty()) {
						mString.resize(mString.size() - 1);
						OnTextRemoved();
					}
				}
				else {
					if (mAllowedInput.find(msg.mInput) != mAllowedInput.end()) {
						mString += msg.mInput;
						OnTextAdded(msg.mInput);
					}
				}
			}
			
			break;
		}
		default : {
			break;
		}
	}
}

void GUIInputBoxBase::Input(GUI* caller) {
	if (auto mSharedInputManagerPtr = GUI::mInputManagerPtr.lock()) {
		if (mSharedInputManagerPtr->GetMousePressed(Mouse::Left)) {
			if (mInArea && !mActive) {
				mActive = true;
				OnStateChange();
			}
			else if (!mInArea && mActive) {
				mActive = false;
				OnStateChange();
			}
		}
	}
}

std::u16string GUIInputBoxBase::GetString() {
	return mString;
}

void GUIInputBoxBase::SetString(const std::u16string& newString) {
	mString = newString;
	OnTextSet(newString);
}
}
