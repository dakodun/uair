/* **************************************************************** **
**	Uair Engine
**	Copyright (c) 2014 Iain M. Crawford
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

#include "renderable.hpp"

#include "util.hpp"

namespace uair {
glm::vec2 Renderable::GetPosition() const {
	return mPosition;
}

void Renderable::SetPosition(const glm::vec2 & newPos) {
	mPosition = newPos;
	
	UpdateGlobalBoundingBox();
}

glm::vec2 Renderable::GetOrigin() const {
	return mOrigin;
}

void Renderable::SetOrigin(const glm::vec2 & newOrigin) {
	mOrigin = newOrigin;
	
	UpdateGlobalBoundingBox();
}

float Renderable::GetDepth() const {
	return mDepth;
}

void Renderable::SetDepth(const float & newDepth) {
	mDepth = newDepth;
}

glm::vec3 Renderable::GetColour() const {
	return mColour;
}

void Renderable::SetColour(const glm::vec3 & newColour) {
	mColour = newColour;
}

float Renderable::GetAlpha() const {
	return mAlpha;
}

void Renderable::SetAlpha(const float & newAlpha) {
	mAlpha = newAlpha;
}

glm::vec2 Renderable::GetScale() const {
	return mScale;
}

void Renderable::SetScale(const glm::vec2 & newScale) {
	mScale = newScale;
	
	UpdateGlobalBoundingBox();
}

float Renderable::GetRotation() const {
	return mRotation;
}

void Renderable::SetRotation(const float & newRotation) {
	mRotation = newRotation;
	
	UpdateGlobalBoundingBox();
}

glm::vec2 Renderable::GetSkew() const {
	return mSkew;
}

void Renderable::SetSkew(const glm::vec2 & newSkew) {
	mSkew = newSkew;
	
	UpdateGlobalBoundingBox();
}

std::string Renderable::GetName() const {
	return mName;
}

Polygon Renderable::GetLocalBoundingBox() const {
	return mLocalBoundingBox;
}

Polygon Renderable::GetGlobalBoundingBox() const {
	return mGlobalBoundingBox;
}

Polygon Renderable::GetLocalMask() const {
	return mLocalMask;
}

void Renderable::SetLocalMask() {
	CreateLocalMask();
	UpdateGlobalMask();
}

void Renderable::SetLocalMask(const Polygon & newMask) {
	mLocalMask = newMask;
	UpdateGlobalMask();
}

Polygon Renderable::GetGlobalMask() const {
	return mGlobalMask;
}
}
