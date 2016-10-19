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

#include "transformable3d.hpp"

namespace uair {
glm::vec3 Transformable3D::GetPosition() const {
	return mPosition;
}

void Transformable3D::SetPosition(const glm::vec3& newPos) {
	mPosition = newPos;
	
	UpdateGlobalBoundingBox();
}

glm::vec3 Transformable3D::GetOrigin() const {
	return mOrigin;
}

void Transformable3D::SetOrigin(const glm::vec3& newOrigin) {
	mOrigin = newOrigin;
	
	UpdateGlobalBoundingBox();
}

glm::vec3 Transformable3D::GetScale() const {
	return mScale;
}

void Transformable3D::SetScale(const glm::vec3& newScale) {
	mScale = newScale;
	
	UpdateGlobalBoundingBox();
}

glm::vec3 Transformable3D::GetRotation() const {
	return mRotation;
}

void Transformable3D::SetRotation(const glm::vec3& newRotation) {
	mRotation = newRotation;
	
	UpdateGlobalBoundingBox();
}

glm::vec3 Transformable3D::GetSkew() const {
	return mSkew;
}

void Transformable3D::SetSkew(const glm::vec3& newSkew) {
	mSkew = newSkew;
	
	UpdateGlobalBoundingBox();
}

glm::mat4 Transformable3D::GetTransformationMatrix() const {
	return mTransformation;
}

void Transformable3D::SetTransformationMatrix(const glm::mat4& newTrans) {
	mTransformation = newTrans;
	
	UpdateGlobalBoundingBox();
}

std::vector<glm::vec3> Transformable3D::GetLocalBoundingBox() const {
	return mLocalBoundingBox;
}

std::vector<glm::vec3> Transformable3D::GetGlobalBoundingBox() const {
	return mGlobalBoundingBox;
}
}
