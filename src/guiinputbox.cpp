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

#include "guiinputbox.hpp"

#include "font.hpp"
#include "renderbatch.hpp"
#include "windowmessages.hpp"
#include "openglstates.hpp"

namespace uair {
GUIInputBox::GUIInputBox(const Properties& properties) : mName(properties.mName), mPosition(properties.mPosition),
		mWidth(static_cast<float>(properties.mWidth)),
		mHeight(static_cast<float>(properties.mHeight)) {
	
	{ // set up members needed to render string offscreen
		unsigned int width = mWidth - 8.0f;
		unsigned int height = mHeight - 15.0f;
		
		std::vector<unsigned char> textureData(width * height * 4, 255u);
		
		mStringTexture.AddFromMemory(textureData, width, height);
		mStringTexture.CreateTexture();
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		
		mStringRenderBuffer.CreateRenderBuffer(GL_DEPTH24_STENCIL8, width, height);
		
		OpenGLStates::BindFBO(mStringFBO.GetFBOID());
		mStringFBO.AttachTexture(&mStringTexture, GL_COLOR_ATTACHMENT0, 0);
		mStringFBO.AttachRenderBuffer(&mStringRenderBuffer, GL_DEPTH_STENCIL_ATTACHMENT);
		mStringFBO.MapBuffers({GL_COLOR_ATTACHMENT0});
	}
	
	{ // set up the text box base shape
		Contour contour;
		contour.AddPoints({glm::vec2(0.0f, 0.0f), glm::vec2(mWidth, 0.0f), glm::vec2(mWidth, mHeight - 10.0f)});
		contour.AddBezier({glm::vec2(mWidth, mHeight - 4.0f), glm::vec2(mWidth - 6.0f, mHeight - 4.0f)});
		contour.AddPoint(glm::vec2(6.0f, mHeight - 4.0f));
		contour.AddBezier({glm::vec2(0.0f, mHeight - 4.0f), glm::vec2(0.0f, mHeight - 10.0f)});
		
		mInputBase.AddContour(contour);
		mInputBase.SetColour(properties.mInputColour);
	}
	
	// set up the text box highlight
	mInputHighlight.AddContour(Contour({glm::vec2(2.0f, 2.0f), glm::vec2(mWidth - 2.0f, 2.0f),
			glm::vec2(mWidth - 2.0f, mHeight - 9.0f), glm::vec2(2.0f, mHeight - 9.0f)}));
	mInputHighlight.SetColour(glm::vec3(0.87f));
	
	// set up the text box text area
	mInput.AddContour(Contour({glm::vec2(4.0f, 4.0f), glm::vec2(mWidth - 4.0f, 4.0f),
			glm::vec2(mWidth - 4.0f, mHeight - 11.0f), glm::vec2(4.0f, mHeight - 11.0f)}));
	mInput.SetColour(glm::vec3(1.0f));
	mInput.AddFrame(&mStringTexture, 0u);
	
	// set up the caret and position it within the text box via origin offsetting
	mCaret.AddContour(Contour({glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, properties.mTextSize), glm::vec2(0.0f, properties.mTextSize)}));
	mCaret.SetColour(glm::vec3(0.0f));
	mCaret.SetOrigin(glm::vec2(-5.0f, -4.0f - (((mHeight - 15.0f) - properties.mTextSize) / 2.0f)));
	mCaret.SetDepth(-20.0f);
	
	{ // set up the render string that is drawn to a seperate texture
		mInputString.SetFont(properties.mFont);
		mInputString.SetSize(properties.mTextSize);
		mInputString.SetText(properties.mDefaultText);
		mInputString.SetColour(glm::vec3(0.0f));
		mInputString.SetDepth(-10.0f);
		
		float height = properties.mFont->GetBearingMax().y * (properties.mTextSize / static_cast<float>(properties.mFont->GetFontSize()));
		float offset = (mHeight - 15.0f) / 2.0f;
		mInputString.SetPosition(glm::vec2(0.0f, offset + (height / 2.0f)));
	}
	
	UpdateStringTexture();
	SetPosition(properties.mPosition); // set the position of all elements of the text box
	
	mAllowedInput.insert(properties.mAllowedInput.begin(), properties.mAllowedInput.end());
	
	if (auto mSharedInputManagerPtr = GUI::mInputManagerPtr.lock()) {
		auto mouseCoords = mSharedInputManagerPtr->GetMouseCoords(CoordinateSpace::Local);
		
		// if the cursor is initially within the text box boundary...
		if (mouseCoords.x > mPosition.x && mouseCoords.x < mPosition.x + mWidth &&
				mouseCoords.y > mPosition.y && mouseCoords.y < mPosition.y + mHeight) {
			
			mInArea = true; // indicate that the cursor is within the boundary
			
			if (!mActive) { // if the text box isn't active by default...
				UpdateRenderState(1u); // set the render state to indicate hovering
			}
			else {
				UpdateRenderState(2u); // otherwise indicate active
			}
		}
	}
}

void GUIInputBox::HandleMessageQueue(const MessageQueue::Entry& e, GUI* caller) {
	using namespace WindowMessage;
	switch (e.GetTypeID()) {
		case MouseMoveMessage::GetTypeID() : {
			MouseMoveMessage msg = e.Deserialise<MouseMoveMessage>();
			
			// if the mouse cursor is inside the input box's bounding box...
			if (msg.mLocalX > mPosition.x && msg.mLocalX < mPosition.x + mWidth &&
					msg.mLocalY > mPosition.y && msg.mLocalY < mPosition.y + mHeight) {
				
				if (!mInArea) {
					mInArea = true;
					
					if (!mActive) {
						UpdateRenderState(1u);
					}
					
					if (caller) {
						caller->mUpdated = true;
					}
				}
			}
			else {
				if (mInArea) {
					mInArea = false;
					
					if (!mActive) {
						UpdateRenderState(0u);
					}
					
					if (caller) {
						caller->mUpdated = true;
					}
				}
			}
			
			break;
		}
		case TextInputMessage::GetTypeID() : {
			TextInputMessage msg = e.Deserialise<TextInputMessage>();
			
			if (mActive) {
				if (msg.mInput == 8) {
					if (mInputString.RemoveText(1u)) {
						mUpdated = true;
					}
				}
				else {
					if (mAllowedInput.find(msg.mInput) != mAllowedInput.end()) {
						mInputString.AddText(msg.mInput);
						mUpdated = true;
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

void GUIInputBox::Input(GUI* caller) {
	if (auto mSharedInputManagerPtr = GUI::mInputManagerPtr.lock()) {
		if (mSharedInputManagerPtr->GetMousePressed(Mouse::Left)) {
			if (mInArea && !mActive) {
				mActive = true;
				UpdateRenderState(2u);
				
				if (caller) {
					caller->mUpdated = true;
				}
			}
			else if (!mInArea && mActive) {
				mActive = false;
				UpdateRenderState(0u);
				
				if (caller) {
					caller->mUpdated = true;
				}
			}
		}
	}
}

void GUIInputBox::Process(float deltaTime, GUI* caller) {
	if (mUpdated) { // if the string has been updated since last loop...
		UpdateStringTexture(); // update the caret and string position and the string texture
		mUpdated = false;
		
		if (caller) {
			caller->mUpdated = true;
		}
	}
	
	if (mActive) { // if the input box is accepting input...
		// animate the caret (flash every second) to help indicate so
		mCaretTimer += deltaTime;
		
		if (mCaretTimer >= 0.5f) {
			mCaretTimer = 0.0f; 
			mCaretDisplay = !mCaretDisplay;
			
			if (caller) {
				caller->mUpdated = true;
			}
		}
	}
}

void GUIInputBox::PostProcess(const unsigned int& processed, float deltaTime, GUI* caller) {
	
}

void GUIInputBox::AddToBatch(RenderBatch& batch, GUI* caller) {
	batch.Add(mInputBase);
	batch.Add(mInputHighlight);
	batch.Add(mInput);
	
	if (mActive && mCaretDisplay) {
		batch.Add(mCaret);
	}
}

glm::vec2 GUIInputBox::GetPosition() const {
	return mPosition;
}

void GUIInputBox::SetPosition(const glm::vec2& newPos) {
	mPosition = newPos;
	mInputBase.SetPosition(mPosition);
	mInputHighlight.SetPosition(mPosition);
	mInput.SetPosition(mPosition);
	
	mCaret.SetPosition(mPosition);
}

std::u16string GUIInputBox::GetString() {
	return mInputString.GetText();
}

void GUIInputBox::SetString(const std::u16string& newString) {
	mInputString.SetText(newString);
	mUpdated = true;
}

unsigned int GUIInputBox::GetTypeID() {
	return static_cast<unsigned int>(GUIElements::InputBox);
}

void GUIInputBox::UpdateRenderState(const unsigned int& state) {
	switch (state) {
		case 0u : { // inactive
			mInputHighlight.SetColour(glm::vec3(0.82f));
			break;
		}
		case 1u : { // inactive - hover
			mInputHighlight.SetColour(glm::vec3(0.6f));
			break;
		}
		case 2u : { // active
			mInputHighlight.SetColour(glm::vec3(0.53f, 0.65f, 0.95f));
			break;
		}
		default :
			break;
	}
}

void GUIInputBox::UpdateStringTexture() {
	float stringWidth = 0.0f; // the width of the string on screen (from local bounding box)
	
	{ // calculate the width of the string from it's local bounding box
		auto bbox = mInputString.GetLocalBoundingBox();
		if (bbox.size() > 3u) {
			stringWidth = (bbox.at(2u).x - bbox.at(0u).x);
		}
	}
	
	
	if ((stringWidth < mWidth - 11.0f)) { // if the string width is smaller than the input box's input area...
		mInputString.SetPosition(glm::vec2(0.0f, mInputString.GetPosition().y)); // set the string to the left of the input box's input area
	}
	else { // otherwise if the string is wider...
		float offset = stringWidth - (mWidth - 11.0f); // find the difference in widths
		
		// position the right side of the string at the right side of the input box's input area
		mInputString.SetPosition(glm::vec2(-offset, mInputString.GetPosition().y));
	}
	
	// update the caret position to be at the end of the string
	mCaret.SetOrigin(glm::vec2(-(5.0f + (mInputString.GetPosition().x + stringWidth)), mCaret.GetOrigin().y));
	
	{ // render the string to a seperate texture to allow clipping
		RenderBatch batch;
		batch.Add(mInputString);
		batch.Upload();
		
		uair::OpenGLStates::mViewMatrix = glm::mat4(1.0f);
		uair::OpenGLStates::mModelMatrix = glm::mat4(1.0f);
		
		uair::OpenGLStates::BindFBO(mStringFBO.GetFBOID());
		glViewport(0, 0, mStringTexture.GetWidth(), mStringTexture.GetHeight());
		uair::OpenGLStates::mProjectionMatrix = glm::ortho(0.0f, static_cast<float>(mStringTexture.GetWidth()),
				static_cast<float>(mStringTexture.GetHeight()), 0.0f, 0.0f, -9999.0f);
		
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		batch.Draw(mStringFBO, 0u);
	}
}
}
