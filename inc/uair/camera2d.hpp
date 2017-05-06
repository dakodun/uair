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

#ifndef UAIRCAMERA2D_HPP
#define UAIRCAMERA2D_HPP

#include "init.hpp"

#include <glm/glm.hpp>

namespace uair {
class EXPORTDLL Camera2D {
	public :
		void Translate(const glm::vec2& translation);
		void Rotate(const float& rotation);
		void Scale(const float& scale);
		
		void SetPosition(const glm::vec2& position);
		glm::vec2 GetPosition() const;
		void SetRotation(const float& rotation);
		float GetRotation() const;
		void SetScale(const float& scale);
		float GetScale() const;
		void SetOrigin(const glm::vec2& origin);
		glm::vec2 GetOrigin() const;
		
		glm::mat4 GetMatrix();
	
	public :
		bool mUpdate = true;
		unsigned int mHalfWidth = 0u;
		unsigned int mHalfHeight = 0u;
		glm::mat4 mView;
		
		glm::vec2 mPosition;
		float mRotation = 0.0f;
		float mScale = 1.0f;
		glm::vec2 mOrigin;
};
}

#endif
