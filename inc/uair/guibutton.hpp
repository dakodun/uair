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

#ifndef UAIRGUIBUTTON_HPP
#define UAIRGUIBUTTON_HPP

#include "gui.hpp"
#include "shape.hpp"
#include "renderstring.hpp"

namespace uair {
class Font;

class GUIButton : public GUIElement {
	public :
		// the message that is sent to the main message queue when the button is slicked
		class ClickedMessage : public Message {
			public :
				ClickedMessage() = default;
				ClickedMessage(const std::string& buttonName);
				
				void Serialise(cereal::BinaryOutputArchive& archive) const;
				void Serialise(cereal::BinaryInputArchive& archive);
				
				static constexpr unsigned int GetTypeID() {
					return 1000u;
				}
			
			public :
				std::string mButtonName; // the (non-unique) name of the button that sent this message
		};
	public :
		// an aggregate used for initialising a button with various properties
		struct Properties {
			std::string mName; // the (non-unique) name of the button
			
			glm::vec2 mPosition; // the position of the button
			unsigned int mWidth; // the width of the button
			unsigned int mHeight; // the height of the button AT IT'S HEIGHEST (including drop shadow)
			glm::vec3 mButtonColourTop; // the colour of the top (dynamic) part of the button
			glm::vec3 mButtonColourBase; // the colour of the bottom (drop shadow) of the button
			
			ResourcePtr<Font> mFont; // a pointer to the font used to draw the text on the button
			std::u16string mText; // the string associated with the text drawn on the button
			unsigned int mTextSize; // the size of the text drawn on the button
			glm::vec3 mTextColour; // the colour of the text drawn on the button
			glm::vec2 mTextOffset; // the offset (from the centre) of the text drawn on the button
		};
	public :
		GUIButton(const Properties& properties);
		
		void HandleMessageQueue(const MessageQueue::Entry& e, GUI* caller = nullptr);
		void Input(GUI* caller = nullptr);
		void Process(float deltaTime, GUI* caller = nullptr);
		void PostProcess(const unsigned int& processed, float deltaTime, GUI* caller = nullptr);
		
		void AddToBatch(RenderBatch& batch, GUI* caller = nullptr);
		
		glm::vec2 GetPosition() const;
		void SetPosition(const glm::vec2& newPos);
		
		static unsigned int GetTypeID();
	protected :
		// update the various renderables used to draw the button
		void UpdateRenderState(const unsigned int& state);
	
	protected :
		std::string mName;
		
		bool mInArea = false; // indicates if the cursor is within the button's bounding box (hovering)
		unsigned int mState = 0u; // the current state of the button (up, down or hovering)
		
		glm::vec2 mPosition;
		float mWidth = 0.0f;
		float mHeight = 0.0f;
		glm::vec2 mTextOffset;
		
		std::string mMessage; // the pre-serialised message that is sent when this button is clicked
	private :
		Shape mButtonTop;
		Shape mButtonBottom;
		RenderString mButtonText;
};
}

#endif
