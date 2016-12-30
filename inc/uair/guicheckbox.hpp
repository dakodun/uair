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

#ifndef UAIRGUICHECKBOX_HPP
#define UAIRGUICHECKBOX_HPP

#include "init.hpp"
#include "guibuttonbase.hpp"
#include "shape.hpp"
#include "renderstring.hpp"

namespace uair {
class EXPORTDLL GUICheckBox : public GUIButtonBase {
	public :
		// the message that is sent to the main message queue when the check box's state is changed
		class StateChangedMessage : public Message {
			public :
				StateChangedMessage() = default;
				StateChangedMessage(const std::string& checkBoxName, const bool& checkBoxState);
				
				void Serialise(cereal::BinaryOutputArchive& archive) const;
				void Serialise(cereal::BinaryInputArchive& archive);
				
				static constexpr unsigned int GetTypeID() {
					return 102u;
				}
			
			public :
				std::string mCheckBoxName; // the (non-unique) name of the check box that sent this message
				bool mNewState;
		};
	public :
		struct Properties {
			std::string mName;
			
			glm::vec2 mPosition;
			unsigned int mSize;
			glm::vec3 mInputColour;
			
			ResourcePtr<Font> mFont;
			unsigned int mTextSize;
			std::u16string mLabelText;
			
			bool mDefaultState;
		};
	public :
		GUICheckBox(const Properties& properties);
		
		void Process(float deltaTime, GUI* caller = nullptr);
		void PostProcess(const unsigned int& processed, float deltaTime, GUI* caller = nullptr);
		
		void AddToBatch(RenderBatch& batch, GUI* caller = nullptr);
		
		glm::vec2 GetPosition() const;
		void SetPosition(const glm::vec2& newPos);
		
		static unsigned int GetTypeID();
	protected :
		void OnHoverChange();
		void OnStateChange();
	
	protected :
		bool mChecked = false;
		bool mUpdated = false;
		
		Shape mCheckBase;
		Shape mCheckTop;
		Shape mCheckHighlight;
		Shape mCheckMark;
		Shape mCheckMarkHighlight;
};
}

#endif
