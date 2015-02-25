/* **************************************************************** **
**	Uair Engine
**	Copyright (c) 2013 Iain M. Crawford
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

/** 
* \file		windowevent.hpp
* \brief	WindowEvent.
**/

#ifndef UAIRWINDOWEVENT_HPP
#define UAIRWINDOWEVENT_HPP

#include "inputenums.hpp"

namespace uair {
class WindowEvent {
	public :
		enum Type {
			CloseType,
			SizeType,
			MoveType,
			GainedFocusType,
			LostFocusType,
			DisplayChangeType,
			GainedCaptureType,
			LostCaptureType,
			KeyboardKeyType,
			MouseButtonType,
			MouseWheelType,
			MouseMoveType
		};
		
		Type type;
		
		struct Close {
			
		};
		
		struct Size {
			unsigned int type;
			short width;
			short height;
		};
		
		struct Move {
			short x;
			short y;
		};
		
		struct GainedFocus {
			
		};
		
		struct LostFocus {
			
		};
		
		struct DisplayChange {
			short width;
			short height;
		};
		
		struct GainedCapture {
			
		};
		
		struct LostCapture {
			
		};
		
		struct KeyboardKey {
			Keyboard key;
			unsigned int type;
		};
		
		struct MouseButton {
			Mouse button;
			unsigned int type;
		};
		
		struct MouseWheel {
			int amount;
		};
		
		struct MouseMove {
			int localX;
			int localY;
			int globalX;
			int globalY;
		};
		
		union {
			Close close;
			Size size;
			Move move;
			GainedFocus gainedFocus;
			LostFocus lostFocus;
			DisplayChange displayChange;
			GainedCapture gainedCapture;
			LostCapture lostCapture;
			KeyboardKey keyboardKey;
			MouseButton mouseButton;
			MouseWheel mouseWheel;
			MouseMove mouseMove;
		};
};
}

#endif
