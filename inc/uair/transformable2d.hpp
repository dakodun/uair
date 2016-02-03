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

#ifndef UAIRTRANSFORMABLE2D_HPP
#define UAIRTRANSFORMABLE2D_HPP

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace uair {
class Transformable2D {
	public :
		virtual ~Transformable2D() = default;
		
		glm::vec2 GetPosition() const;
		void SetPosition(const glm::vec2& newPos);
		glm::vec2 GetOrigin() const;
		void SetOrigin(const glm::vec2& newOrigin);
		
		glm::vec2 GetScale() const;
		void SetScale(const glm::vec2& newScale);
		float GetRotation() const;
		void SetRotation(const float& newRotation);
		glm::vec2 GetSkew() const;
		void SetSkew(const glm::vec2& newSkew);
		
		glm::mat3 GetTransformationMatrix() const;
		void SetTransformationMatrix(const glm::mat3& newTrans);
		
		std::vector<glm::vec2> GetLocalBoundingBox() const;
		std::vector<glm::vec2> GetGlobalBoundingBox() const;
		
		std::vector<glm::vec2> GetLocalMask() const;
		void SetLocalMask();
		void SetLocalMask(const std::vector<glm::vec2>& newMask);
		std::vector<glm::vec2> GetGlobalMask() const;
	protected :
		virtual void UpdateGlobalBoundingBox() = 0;
		virtual void UpdateGlobalMask() = 0;
		virtual void CreateLocalMask() = 0;
	protected :
		glm::vec2 mPosition = glm::vec2();
		glm::vec2 mOrigin = glm::vec2();
		
		glm::mat3 mTransformation = glm::mat3();
		glm::vec2 mScale = glm::vec2(1.0f, 1.0f);
		float mRotation = 0;
		glm::vec2 mSkew = glm::vec2();
		
		std::vector<glm::vec2> mLocalBoundingBox;
		std::vector<glm::vec2> mGlobalBoundingBox;
		
		std::vector<glm::vec2> mLocalMask;
		std::vector<glm::vec2> mGlobalMask;
};
}

#endif
