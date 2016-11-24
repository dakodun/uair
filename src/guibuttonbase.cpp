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

#include "guibuttonbase.hpp"

#include "windowmessages.hpp"

namespace uair {
void GUIButtonBase::HandleMessageQueue(const MessageQueue::Entry& e, GUI* caller) {
	using namespace WindowMessage;
	switch (e.GetTypeID()) { // depending on the type of message being handled...
		case MouseMoveMessage::GetTypeID() : { // if the message is a mouse move message...
			MouseMoveMessage msg = e.Deserialise<MouseMoveMessage>(); // deserialise the message object
			
			// if the new mouse coordinates are within the check box's bounding box...
			if (msg.mLocalX > mPosition.x && msg.mLocalX < mPosition.x + mWidth &&
					msg.mLocalY > mPosition.y && msg.mLocalY < mPosition.y + mHeight) {
				
				if (!mInArea) { // if the cursor is not already in the check box's bounding box...
					mInArea = true; // indicate the cursor is now in the check box's bounding box
					OnHoverChange();
				}
			}
			else { // otherwise the new mouse coordinates are outwith the check box's bounding box...
				if (mInArea) {
					mInArea = false; // indicate the cursor is no longer in the check box's bounding box
					OnHoverChange();
				}
			}
			
			break;
		}
		default : {
			break;
		}
	}
}

void GUIButtonBase::Input(GUI* caller) {
	if (auto mSharedInputManagerPtr = GUI::mInputManagerPtr.lock()) { // if the input manager pointer is valid...
		if (mSharedInputManagerPtr->GetMousePressed(Mouse::Left)) { // if the left mouse button was pressed...
			if (mInArea && mState == 0u) { // if the cursor is in the check box's bounding box and the check box is up...
				mState = 1u; // set the check box as down
				OnStateChange();
			}
		}
		
		if (mSharedInputManagerPtr->GetMouseReleased(Mouse::Left)) { // if the left mouse button was released...
			if (mState == 1u) { // if the check box is down...
				mState = 0u; // set the check box as up
				OnStateChange();
			}
		}
	}
}
}
