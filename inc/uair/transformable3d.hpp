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

#ifndef UAIRTRANSFORMABLE3D_HPP
#define UAIRTRANSFORMABLE3D_HPP

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "init.hpp"

namespace uair {
class EXPORTDLL Transformable3D {
	public :
		virtual ~Transformable3D() = default;
		
		glm::vec3 GetPosition() const;
		void SetPosition(const glm::vec3& newPos);
		glm::vec3 GetOrigin() const;
		void SetOrigin(const glm::vec3& newOrigin);
		
		glm::vec3 GetScale() const;
		void SetScale(const glm::vec3& newScale);
		glm::vec3 GetRotation() const;
		void SetRotation(const glm::vec3& newRotation);
		glm::vec3 GetSkew() const;
		void SetSkew(const glm::vec3& newSkew);
		
		glm::mat4 GetTransformationMatrix() const;
		void SetTransformationMatrix(const glm::mat4& newTrans);
		
		std::vector<glm::vec3> GetLocalBoundingBox() const;
		std::vector<glm::vec3> GetGlobalBoundingBox() const;
	protected :
		virtual void UpdateGlobalBoundingBox() = 0;
	protected :
		glm::vec3 mPosition = glm::vec3();
		glm::vec3 mOrigin = glm::vec3();
		
		glm::mat4 mTransformation = glm::mat4();
		glm::vec3 mScale = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 mRotation = glm::vec3();
		glm::vec3 mSkew = glm::vec3();
		
		std::vector<glm::vec3> mLocalBoundingBox;
		std::vector<glm::vec3> mGlobalBoundingBox;
};
}

#endif
