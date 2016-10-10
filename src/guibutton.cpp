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

#include "guibutton.hpp"

#include "font.hpp"
#include "renderbatch.hpp"
#include "windowmessages.hpp"

namespace uair {
GUIButton::ClickedMessage::ClickedMessage(const std::string& buttonName) : mButtonName(buttonName){
	
}

void GUIButton::ClickedMessage::Serialise(cereal::BinaryOutputArchive& archive) const {
	archive(mButtonName);
}

void GUIButton::ClickedMessage::Serialise(cereal::BinaryInputArchive& archive) {
	archive(mButtonName);
}


GUIButton::GUIButton(const Properties& properties) : mName(properties.mName), mPosition(properties.mPosition),
		mWidth(static_cast<float>(properties.mWidth)),
		mHeight(static_cast<float>(std::max(10u, properties.mHeight))),
		mTextOffset(properties.mTextOffset) {
	
	{ // create and setup the shapes and renderstring the represent the button
		// create the contour that represents both button shapes
		Contour contour;
		contour.AddPoints({glm::vec2(0.0f, 0.0f), glm::vec2(mWidth, 0.0f), glm::vec2(mWidth, mHeight - 10.0f)});
		contour.AddBezier({glm::vec2(mWidth, mHeight - 4.0f), glm::vec2(mWidth - 6.0f, mHeight - 4.0f)});
		contour.AddPoint(glm::vec2(6.0f, mHeight - 4.0f));
		contour.AddBezier({glm::vec2(0.0f, mHeight - 4.0f), glm::vec2(0.0f, mHeight - 10.0f)});
		
		mButtonTop.AddContour(contour);
		mButtonTop.SetOrigin(glm::vec2(0.0f, -2.0f)); // set the offset of the top button to be slightly higher than the base
		mButtonTop.SetColour(properties.mButtonColourTop);
		
		mButtonBottom.AddContour(contour);
		mButtonBottom.SetOrigin(glm::vec2(0.0f, -4.0f));
		mButtonBottom.SetColour(properties.mButtonColourBase);
		
		mButtonText.SetFont(properties.mFont);
		mButtonText.SetText(properties.mText);
		mButtonText.SetSize(properties.mTextSize);
		mButtonText.SetOrigin(glm::vec2(0.0f, -2.0f)); // set the offset of the button text to match the top button
		mButtonText.SetColour(properties.mTextColour);
	}
	
	SetPosition(properties.mPosition); // store the position of the button
	UpdateRenderState(1u); // update the button shapes and render string
	
	if (auto mSharedInputManagerPtr = GUI::mInputManagerPtr.lock()) { // if the input manager pointer is valid...
		// retrieve the current mouse cursor coordinates and update the button state depending on whether they
		// are within the button's bounding box or not
		auto mouseCoords = mSharedInputManagerPtr->GetMouseCoords(CoordinateSpace::Local);
		if (mouseCoords.x > mPosition.x && mouseCoords.x < mPosition.x + mWidth &&
				mouseCoords.y > mPosition.y && mouseCoords.y < mPosition.y + mHeight) {
			
			mInArea = true;
			UpdateRenderState(2u);
		}
	}
	
	{ // serialise the clicked message now as it won't change and we can reuse it
		std::stringstream strStream;
		ClickedMessage msg(mName);
		
		{
			// create an output archive using cereal and our stringstream and serialise the message
			cereal::BinaryOutputArchive archiveOutBinary(strStream);
			archiveOutBinary(msg);
		} // flush the archive on destruction
		
		mMessage = strStream.str(); // store the serialised string
	}
}

void GUIButton::HandleMessageQueue(const MessageQueue::Entry& e, GUI* caller) {
	using namespace WindowMessage;
	switch (e.GetTypeID()) { // depending on the type of message being handled...
		case MouseMoveMessage::GetTypeID() : { // if the message is a mouse move message...
			MouseMoveMessage msg = e.Deserialise<MouseMoveMessage>(); // deserialise the message object
			
			// if the new mouse coordinates are within the button's bounding box...
			if (msg.mLocalX > mPosition.x && msg.mLocalX < mPosition.x + mWidth &&
					msg.mLocalY > mPosition.y && msg.mLocalY < mPosition.y + mHeight) {
				
				if (!mInArea) { // if the cursor is not already in the button's bounding box...
					mInArea = true; // indicate the cursor is now in the button's bounding box
					if (mState == 0u) { // if the button is currently up...
						UpdateRenderState(2u); // update the button shapes and render string (hovering)
						
						if (caller) { // if the button belongs to a gui object...
							caller->mUpdated = true; // indicate that a redraw is necessary
						}
					}
					else if (mState == 1u) { // otherwise if the button is currently down...
						UpdateRenderState(0u); // update the button shapes and render string
						
						if (caller) {
							caller->mUpdated = true;
						}
					}
				}
			}
			else { // otherwise the new mouse coordinates are outwith the button's bounding box...
				if (mInArea) {
					mInArea = false; // indicate the cursor is no longer in the button's bounding box
					UpdateRenderState(1u);
					
					if (caller) {
						caller->mUpdated = true;
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

void GUIButton::Input(GUI* caller) {
	if (auto mSharedInputManagerPtr = GUI::mInputManagerPtr.lock()) { // if the input manager pointer is valid...
		if (mSharedInputManagerPtr->GetMousePressed(Mouse::Left)) { // if the left mouse button was pressed...
			if (mInArea && mState == 0u) { // if the cursor is in the button's bounding box and the button is up...
				mState = 1u; // set the button as down
				UpdateRenderState(0u); // update the button shapes and render string
				
				if (caller) { // if the button belongs to a gui object...
					caller->mUpdated = true; // indicate that a redraw is necessary
				}
			}
		}
		
		if (mSharedInputManagerPtr->GetMouseReleased(Mouse::Left)) { // if the left mouse button was released...
			if (mState == 1u) { // if the button is down...
				mState = 0u; // set the button as up
				
				if (mInArea) {
					UpdateRenderState(2u);
					
					if (GUI::mMessageQueuePtr) { // if the message queue pointer is valid...
						// dispatch the pre-serialised message to the message queue
						GUI::mMessageQueuePtr->PushMessageString(ClickedMessage::GetTypeID(), mMessage);
					}
				}
				else {
					UpdateRenderState(1u);
				}
				
				if (caller) {
					caller->mUpdated = true;
				}
			}
		}
	}
}

void GUIButton::Process(GUI* caller) {
	
}

void GUIButton::PostProcess(const unsigned int& processed, GUI* caller) {
	
}

void GUIButton::AddToBatch(RenderBatch& batch, GUI* caller) {
	batch.Add(mButtonBottom);
	batch.Add(mButtonTop);
	batch.Add(mButtonText);
}

glm::vec2 GUIButton::GetPosition() const {
	return mPosition;
}

void GUIButton::SetPosition(const glm::vec2& newPos) {
	// update the stored position and the position of both button shapes
	mPosition = newPos;
	mButtonTop.SetPosition(mPosition);
	mButtonBottom.SetPosition(mPosition);
	
	// get the bounding box of the renderstring and adjust the position so that it sits centrally within the button
	std::vector<glm::vec2> bbox = mButtonText.GetLocalBoundingBox();
	if (bbox.size() > 3) {
		mButtonText.SetPosition(mPosition + glm::vec2(
				(mWidth / 2) - ((bbox.at(2).x - bbox.at(0).x) / 2) + mTextOffset.x,
				(mHeight / 2) + ((bbox.at(2).y - bbox.at(0).y) / 2) + mTextOffset.y
				));
	}
}

unsigned int GUIButton::GetTypeID() {
	return static_cast<unsigned int>(GUIElements::Button);
}

void GUIButton::UpdateRenderState(const unsigned int& state) {
	switch (state) { // depending on the current state of the button...
		case 0u : { // down
			// adjust the origins of the top button shape and renderstring to add dynamic effect
			mButtonTop.SetOrigin(glm::vec2(0.0f, -4.0f));
			mButtonText.SetOrigin(glm::vec2(0.0f, -4.0f));
			break;
		}
		case 1u : { // up
			mButtonTop.SetOrigin(glm::vec2(0.0f, -2.0f));
			mButtonText.SetOrigin(glm::vec2(0.0f, -2.0f));
			break;
		}
		case 2u : { // hover
			mButtonTop.SetOrigin(glm::vec2(0.0f, 0.0f));
			mButtonText.SetOrigin(glm::vec2(0.0f, 0.0f));
			break;
		}
		default :
			break;
	}
}
}
