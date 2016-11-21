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

#ifndef UAIRGUIINPUTBOX_HPP
#define UAIRGUIINPUTBOX_HPP

#include "gui.hpp"
#include "shape.hpp"
#include "renderstring.hpp"
#include "renderbuffer.hpp"

namespace uair {
class Font;

class GUIInputBox : public GUIElement {
	public :
		// an aggregate used for initialising an input box with various properties
		struct Properties {
			std::string mName; // the (non-unique) name of the input box
			
			glm::vec2 mPosition; // the position of the input box
			unsigned int mWidth; // the width of the input box
			unsigned int mHeight; // the height of the input box
			glm::vec3 mInputColour; // the colour of the text in the input box
			
			ResourcePtr<Font> mFont; // a pointer to the font used to draw the text in the input box
			unsigned int mTextSize; // the size of the text in the input box
			std::u16string mDefaultText; // the starting text in the input box
			
			// an array of characters that are allowed to be entered into the input box
			std::vector<char16_t> mAllowedInput;
		};
	public :
		GUIInputBox(const Properties& properties);
		
		void HandleMessageQueue(const MessageQueue::Entry& e, GUI* caller = nullptr);
		void Input(GUI* caller = nullptr);
		void Process(float deltaTime, GUI* caller = nullptr);
		void PostProcess(const unsigned int& processed, float deltaTime, GUI* caller = nullptr);
		
		void AddToBatch(RenderBatch& batch, GUI* caller = nullptr);
		
		glm::vec2 GetPosition() const;
		void SetPosition(const glm::vec2& newPos);
		
		std::u16string GetString();
		void SetString(const std::u16string& newString);
		
		static unsigned int GetTypeID();
	protected :
		void UpdateRenderState(const unsigned int& state);
		void UpdateStringTexture();
	
	protected :
		std::string mName;
		
		bool mInArea = false; // indicates if the cursor is within the input box's bounding box (hovering)
		
		glm::vec2 mPosition;
		float mWidth = 0.0f;
		float mHeight = 0.0f;
		
		bool mActive = false; // indicates if the input box is accepting text input
		bool mUpdated = false; // indicates that the input box's string must be redrawn
		
		float mCaretTimer = 0.0f;
		bool mCaretDisplay = false;
		unsigned int mCaretPosition = 0u; // the position of the caret within the string
		Shape mCaret; // the shape representing the caret
		RenderString mInputString; // the render string that represents the input box's text
		
		// members to allow the string to be rendered off screen first to allow clipping
		Texture mStringTexture;
		RenderBuffer mStringRenderBuffer;
		FBO mStringFBO;
		
		std::set<char16_t> mAllowedInput; // a set used for character lookup when entering text (permitted characters)
	private :
		Shape mInputBase;
		Shape mInputHighlight;
		Shape mInput;
};
}

#endif
