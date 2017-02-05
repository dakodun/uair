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

#include "guicheckbox.hpp"

#include "fontbase.hpp"
#include "renderbatch.hpp"
#include "windowmessages.hpp"

namespace uair {
GUICheckBox::StateChangedMessage::StateChangedMessage(const std::string& checkBoxName, const bool& checkBoxState) :
		mCheckBoxName(checkBoxName), mNewState(checkBoxState) {
	
	
}

void GUICheckBox::StateChangedMessage::Serialise(cereal::BinaryOutputArchive& archive) const {
	archive(mCheckBoxName, mNewState);
}

void GUICheckBox::StateChangedMessage::Serialise(cereal::BinaryInputArchive& archive) {
	archive(mCheckBoxName, mNewState);
}


GUICheckBox::GUICheckBox(const Properties& properties) {
	mName = properties.mName;
	mPosition = properties.mPosition;
	mWidth = static_cast<float>(properties.mSize);
	mHeight = static_cast<float>(properties.mSize);
	
	{
		Contour contour;
		contour.AddPoints({glm::vec2(0.0f, 0.0f), glm::vec2(mWidth - 7.0f, 0.0f), glm::vec2(mWidth - 7.0f, mHeight - 10.0f)});
		contour.AddBezier({glm::vec2(mWidth - 7.0f, mHeight - 4.0f), glm::vec2(mWidth - 13.0f, mHeight - 4.0f)});
		contour.AddPoint(glm::vec2(6.0f, mHeight - 4.0f));
		contour.AddBezier({glm::vec2(0.0f, mHeight - 4.0f), glm::vec2(0.0f, mHeight - 10.0f)});
		
		mCheckBase.AddContour(contour);
		mCheckBase.SetColour(properties.mInputColour);
	}
	
	mCheckHighlight.AddContour(Contour({glm::vec2(2.0f, 2.0f), glm::vec2(mWidth - 9.0f, 2.0f),
			glm::vec2(mWidth - 9.0f, mHeight - 9.0f), glm::vec2(2.0f, mHeight - 9.0f)}));
	mCheckHighlight.SetColour(glm::vec3(0.87f, 0.87f, 0.87f));
	
	mCheckTop.AddContour(Contour({glm::vec2(4.0f, 4.0f), glm::vec2(mWidth - 11.0f, 4.0f),
			glm::vec2(mWidth - 11.0f, mHeight - 11.0f), glm::vec2(4.0f, mHeight - 11.0f)}));
	mCheckTop.SetColour(glm::vec3(1.0f, 1.0f, 1.0f));
	
	float step = 2.0f * ((mWidth - 15.0f) / 10.0f);
	mCheckMark.AddContour(Contour({glm::vec2(4.0 + step, 4.0f + step), glm::vec2((mWidth - 11.0f) - step, 4.0f + step),
			glm::vec2((mWidth - 11.0f) - step, (mHeight - 11.0f) - step), glm::vec2(4.0f + step, (mHeight - 11.0f) - step)}));
	mCheckMark.SetColour(glm::vec3(0.0f, 0.0f, 0.0f));
	
	mCheckMarkHighlight = mCheckMark;
	mCheckMarkHighlight.SetColour(glm::vec3(0.8f, 0.8f, 0.8f));
	
	mChecked = properties.mDefaultState;
	SetPosition(properties.mPosition);
	
	if (auto mSharedInputManagerPtr = GUI::mInputManagerPtr.lock()) {
		auto mouseCoords = mSharedInputManagerPtr->GetMouseCoords(CoordinateSpace::Local);
		
		// if the cursor is initially within the check box boundary...
		if (mouseCoords.x > mPosition.x && mouseCoords.x < mPosition.x + mWidth &&
				mouseCoords.y > mPosition.y && mouseCoords.y < mPosition.y + mHeight) {
			
			mInArea = true; // indicate that the cursor is within the boundary
			mCheckHighlight.SetColour(glm::vec3(0.6f, 0.6f, 0.6f));
		}
	}
}

void GUICheckBox::Process(float deltaTime, GUI* caller) {
	if (mUpdated) {
		if (caller) { // if the check box belongs to a gui object...
			caller->mUpdated = true; // indicate that a redraw is necessary
		}
		
		mUpdated = false;
	}
}

void GUICheckBox::PostProcess(const unsigned int& processed, float deltaTime, GUI* caller) {
	
}

void GUICheckBox::AddToBatch(RenderBatch& batch, GUI* caller) {
	batch.Add(mCheckBase);
	batch.Add(mCheckHighlight);
	batch.Add(mCheckTop);
	
	if (mChecked) {
		batch.Add(mCheckMark);
	}
	
	if (mInArea && mState == 1u) {
		batch.Add(mCheckMarkHighlight);
	}
}

glm::vec2 GUICheckBox::GetPosition() const {
	return mPosition;
}

void GUICheckBox::SetPosition(const glm::vec2& newPos) {
	// update the stored position and the position of both check box shapes
	mPosition = newPos;
	mCheckTop.SetPosition(mPosition);
	mCheckBase.SetPosition(mPosition);
	mCheckHighlight.SetPosition(mPosition);
	mCheckMark.SetPosition(mPosition);
	mCheckMarkHighlight.SetPosition(mPosition);
}

unsigned int GUICheckBox::GetTypeID() {
	return static_cast<unsigned int>(GUIElements::CheckBox);
}

void GUICheckBox::OnHoverChange() {
	if (mInArea) { // if the cursor is not in the check box's bounding box...
		mCheckHighlight.SetColour(glm::vec3(0.6f, 0.6f, 0.6f));
		
		if (mState == 1u) { // otherwise if the check box is currently down...
			
		}
		
		mUpdated = true;
	}
	else {
		mCheckHighlight.SetColour(glm::vec3(0.87f, 0.87f, 0.87f));
		
		mUpdated = true;
	}
}

void GUICheckBox::OnStateChange() {
	if (mState == 0u) {
		if (mInArea) {
			mChecked = !mChecked;
			mCheckHighlight.SetColour(glm::vec3(0.6f, 0.6f, 0.6f));
			
			if (GUI::mMessageQueuePtr) { // if the message queue pointer is valid...
				std::stringstream strStream;
				StateChangedMessage msg(mName, mChecked);
				
				{
					// create an output archive using cereal and our stringstream and serialise the message
					cereal::BinaryOutputArchive archiveOutBinary(strStream);
					archiveOutBinary(msg);
				} // flush the archive on destruction
				
				// send a message indicating a state change
				GUI::mMessageQueuePtr->PushMessageString(StateChangedMessage::GetTypeID(), strStream.str());
			}
		}
		else {
			mCheckHighlight.SetColour(glm::vec3(0.87f, 0.87f, 0.87f));
		}
		
		mUpdated = true;
	}
	else if (mState == 1u) {
		mUpdated = true;
	}
}
}
