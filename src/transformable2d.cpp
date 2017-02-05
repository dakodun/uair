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

#include "transformable2d.hpp"

namespace uair {
glm::vec2 Transformable2D::GetPosition() const {
	return mPosition;
}

void Transformable2D::SetPosition(const glm::vec2& newPos) {
	mPosition = newPos;
	
	UpdateGlobalBoundingBox();
	UpdateGlobalMask();
}

glm::vec2 Transformable2D::GetOrigin() const {
	return mOrigin;
}

void Transformable2D::SetOrigin(const glm::vec2& newOrigin) {
	mOrigin = newOrigin;
	
	UpdateGlobalBoundingBox();
	UpdateGlobalMask();
}

glm::vec2 Transformable2D::GetScale() const {
	return mScale;
}

void Transformable2D::SetScale(const glm::vec2& newScale) {
	mScale = newScale;
	
	UpdateGlobalBoundingBox();
	UpdateGlobalMask();
}

float Transformable2D::GetRotation() const {
	return mRotation;
}

void Transformable2D::SetRotation(const float& newRotation) {
	mRotation = newRotation;
	
	UpdateGlobalBoundingBox();
	UpdateGlobalMask();
}

glm::vec2 Transformable2D::GetSkew() const {
	return mSkew;
}

void Transformable2D::SetSkew(const glm::vec2& newSkew) {
	mSkew = newSkew;
	
	UpdateGlobalBoundingBox();
	UpdateGlobalMask();
}

glm::mat3 Transformable2D::GetTransformationMatrix() const {
	return mTransformation;
}

void Transformable2D::SetTransformationMatrix(const glm::mat3& newTrans) {
	mTransformation = newTrans;
	
	UpdateGlobalBoundingBox();
	UpdateGlobalMask();
}

std::vector<glm::vec2> Transformable2D::GetLocalBoundingBox() const {
	return mLocalBoundingBox;
}

std::vector<glm::vec2> Transformable2D::GetGlobalBoundingBox() const {
	return mGlobalBoundingBox;
}

std::vector<glm::vec2> Transformable2D::GetLocalMask() const {
	return mLocalMask;
}

void Transformable2D::SetLocalMask() {
	CreateLocalMask();
	UpdateGlobalMask();
}

void Transformable2D::SetLocalMask(const std::vector<glm::vec2>& newMask) {
	mLocalMask.clear();
	mLocalMask.insert(mLocalMask.end(), newMask.begin(), newMask.end());
	UpdateGlobalMask();
}

std::vector<glm::vec2> Transformable2D::GetGlobalMask() const {
	return mGlobalMask;
}
}
