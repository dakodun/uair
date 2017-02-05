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

#include "fontbase.hpp"
#include "renderbatch.hpp"
#include "windowmessages.hpp"

namespace uair {
GUIButton::ClickedMessage::ClickedMessage(const std::string& buttonName) : mButtonName(buttonName) {
	
}

void GUIButton::ClickedMessage::Serialise(cereal::BinaryOutputArchive& archive) const {
	archive(mButtonName);
}

void GUIButton::ClickedMessage::Serialise(cereal::BinaryInputArchive& archive) {
	archive(mButtonName);
}


GUIButton::GUIButton(const Properties& properties) : mTextOffset(properties.mTextOffset) {
	mName = properties.mName;
	mPosition = properties.mPosition;
	mWidth = static_cast<float>(properties.mWidth);
	mHeight = static_cast<float>(properties.mHeight);
	
	{ // create and setup the shapes and renderstring the represent the button
		// create the contour that represents both button shapes
		Contour contour;
		contour.AddPoints({glm::vec2(0.0f, 0.0f), glm::vec2(mWidth, 0.0f), glm::vec2(mWidth, mHeight - 10.0f)});
		contour.AddBezier({glm::vec2(mWidth, mHeight - 4.0f), glm::vec2(mWidth - 6.0f, mHeight - 4.0f)});
		contour.AddPoint(glm::vec2(6.0f, mHeight - 4.0f));
		contour.AddBezier({glm::vec2(0.0f, mHeight - 4.0f), glm::vec2(0.0f, mHeight - 10.0f)});
		
		mButtonTop.AddContour(contour);
		mButtonTop.SetOrigin(glm::vec2(0.0f, -2.0f));
		mButtonTop.SetColour(properties.mButtonColourTop);
		
		mButtonBottom.AddContour(contour);
		mButtonBottom.SetOrigin(glm::vec2(0.0f, -4.0f));
		mButtonBottom.SetColour(properties.mButtonColourBase);
		
		mButtonText.SetFont(properties.mFont);
		mButtonText.SetText(properties.mText);
		mButtonText.SetSize(properties.mTextSize);
		mButtonText.SetOrigin(glm::vec2(0.0f, -2.0f));
		mButtonText.SetColour(properties.mTextColour);
	}
	
	SetPosition(properties.mPosition); // store the position of the button
	
	if (auto mSharedInputManagerPtr = GUI::mInputManagerPtr.lock()) { // if the input manager pointer is valid...
		// retrieve the current mouse cursor coordinates and update the button state depending on whether they
		// are within the button's bounding box or not
		auto mouseCoords = mSharedInputManagerPtr->GetMouseCoords(CoordinateSpace::Local);
		if (mouseCoords.x > mPosition.x && mouseCoords.x < mPosition.x + mWidth &&
				mouseCoords.y > mPosition.y && mouseCoords.y < mPosition.y + mHeight) {
			
			mInArea = true;
			mButtonTop.SetOrigin(glm::vec2(0.0f, 0.0f));
			mButtonText.SetOrigin(glm::vec2(0.0f, 0.0f));
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

void GUIButton::Process(float deltaTime, GUI* caller) {
	if (mUpdated) {
		if (caller) { // if the button belongs to a gui object...
			caller->mUpdated = true; // indicate that a redraw is necessary
		}
		
		mUpdated = false;
	}
}

void GUIButton::PostProcess(const unsigned int& processed, float deltaTime, GUI* caller) {
	
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
		mButtonText.SetPosition(mPosition + glm::vec2((mWidth / 2) - ((bbox.at(2).x - bbox.at(0).x) / 2) + mTextOffset.x,
				(mHeight / 2) + ((bbox.at(2).y - bbox.at(0).y) / 2) + mTextOffset.y));
	}
}

unsigned int GUIButton::GetTypeID() {
	return static_cast<unsigned int>(GUIElements::Button);
}

void GUIButton::OnHoverChange() {
	if (mInArea) { // if the cursor is not in the button's bounding box...
		if (mState == 0u) { // if the button is currently up...
			mButtonTop.SetOrigin(glm::vec2(0.0f, 0.0f));
			mButtonText.SetOrigin(glm::vec2(0.0f, 0.0f));
			
			mUpdated = true;
		}
		else if (mState == 1u) { // otherwise if the button is currently down...
			mButtonTop.SetOrigin(glm::vec2(0.0f, -4.0f));
			mButtonText.SetOrigin(glm::vec2(0.0f, -4.0f));
			
			mUpdated = true;
		}
	}
	else {
		mButtonTop.SetOrigin(glm::vec2(0.0f, -2.0f));
		mButtonText.SetOrigin(glm::vec2(0.0f, -2.0f));
		
		mUpdated = true;
	}
}

void GUIButton::OnStateChange() {
	if (mState == 0u) {
		if (mInArea) {
			mButtonTop.SetOrigin(glm::vec2(0.0f, 0.0f));
			mButtonText.SetOrigin(glm::vec2(0.0f, 0.0f));
			
			if (GUI::mMessageQueuePtr) { // if the message queue pointer is valid...
				// dispatch the pre-serialised message to the message queue
				GUI::mMessageQueuePtr->PushMessageString(ClickedMessage::GetTypeID(), mMessage);
			}
		}
		else {
			mButtonTop.SetOrigin(glm::vec2(0.0f, -2.0f));
			mButtonText.SetOrigin(glm::vec2(0.0f, -2.0f));
		}
		
		mUpdated = true;
	}
	else if (mState == 1u) {
		mButtonTop.SetOrigin(glm::vec2(0.0f, -4.0f));
		mButtonText.SetOrigin(glm::vec2(0.0f, -4.0f));
		
		mUpdated = true;
	}
}
}
