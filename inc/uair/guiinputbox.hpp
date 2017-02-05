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

#ifndef UAIRGUIINPUTBOX_HPP
#define UAIRGUIINPUTBOX_HPP

#include "init.hpp"
#include "guiinputboxbase.hpp"
#include "shape.hpp"
#include "renderstring.hpp"

namespace uair {
class EXPORTDLL GUIInputBox : public GUIInputBoxBase {
	public :
		// the message that is sent to the main message queue when the input box's string is changed
		class StringChangedMessage : public Message {
			public :
				StringChangedMessage() = default;
				StringChangedMessage(const std::string& inputBoxName, const std::u16string& inputBoxString);
				
				void Serialise(cereal::BinaryOutputArchive& archive) const;
				void Serialise(cereal::BinaryInputArchive& archive);
				
				static constexpr unsigned int GetTypeID() {
					return 101u;
				}
			
			public :
				std::string mInputBoxName; // the (non-unique) name of the input box that sent this message
				std::u16string mNewString;
		};
	public :
		// an aggregate used for initialising an input box with various properties
		struct Properties {
			std::string mName; // the (non-unique) name of the input box
			
			glm::vec2 mPosition; // the position of the input box
			unsigned int mWidth; // the width of the input box
			unsigned int mHeight; // the height of the input box
			glm::vec3 mInputColour; // the colour of the input box's base
			
			ResourcePtr<FontBase> mFont; // a pointer to the font used to draw the text in the input box
			unsigned int mTextSize; // the size of the text in the input box
			std::u16string mDefaultText; // the starting text in the input box
			
			// an array of characters that are allowed to be entered into the input box
			std::vector<char16_t> mAllowedInput;
		};
	public :
		GUIInputBox(const Properties& properties);
		
		void Process(float deltaTime, GUI* caller = nullptr);
		void PostProcess(const unsigned int& processed, float deltaTime, GUI* caller = nullptr);
		
		void AddToBatch(RenderBatch& batch, GUI* caller = nullptr);
		
		glm::vec2 GetPosition() const;
		void SetPosition(const glm::vec2& newPos);
		
		static unsigned int GetTypeID();
	protected :
		void UpdateRenderState(const unsigned int& state);
		void UpdateStringTexture();
		
		void OnHoverChange();
		void OnStateChange();
		void OnTextRemoved();
		void OnTextAdded(const char16_t& newChar);
		void OnTextSet(const std::u16string& newString);
	
	protected :
		bool mStringUpdated = false; // indicates that the input box's string must be redrawn
		bool mUpdated = false;
		
		Shape mInputBase;
		Shape mInputHighlight;
		Shape mInput;
		
		float mCaretTimer = 0.0f;
		bool mCaretDisplay = false;
		unsigned int mCaretPosition = 0u; // the position of the caret within the string
		Shape mCaret; // the shape representing the caret
		RenderString mInputString; // the render string that represents the input box's text
		
		// members to allow the string to be rendered off screen first to allow clipping
		Texture mStringTexture;
		RenderBuffer mStringRenderBuffer;
		FBO mStringFBO;
};
}

#endif
