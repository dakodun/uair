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

#include "camera2d.hpp"

#include <iostream>
#include "util.hpp"

namespace uair {
void Camera2D::Translate(const glm::vec2& translation) {
	glm::mat3 mat(1.0f,        0.0f, 0.0f,
				  0.0f,        1.0f, 0.0f,
		   mPosition.x, mPosition.y, 1.0f);
	
	mat *= util::GetRotationMatrix(mRotation);
	glm::vec3 adj = (glm::vec3(-translation, 1.0f) * mat);
	mPosition += glm::vec2(adj.x, adj.y);
	mUpdate = true; // indicate a view matrix update is required
}

void Camera2D::Rotate(const float& rotation) {
	if (!util::CompareFloats(rotation, util::Equals, 0.0f)) { // if the change in angle is not zero (avoid unnecessary calculations)...
		mRotation = std::fmod(mRotation + rotation, 360.0f); // add the change in angle to the existing angle and clip it between 0 and 360
		mUpdate = true; // indicate a view matrix update is required
	}
}

void Camera2D::Scale(const float& scale) {
	if (!util::CompareFloats(scale, util::Equals, 1.0f)) { // if the change in scale is not one (100%) (avoid unnecessary calculations)...
		mScale *= scale; // modify the scale by a percentage
		mUpdate = true; // indicate a view matrix update is required
	}
}

void Camera2D::SetPosition(const glm::vec2& position) {
	mPosition = glm::vec2(0.0f, 0.0f);
	float tempRotation = mRotation;
	mRotation = 0.0f;
	
	Translate(position);
	
	mRotation = tempRotation;
}

glm::vec2 Camera2D::GetPosition() const {
	return mPosition;
}

void Camera2D::SetRotation(const float& rotation) {
	mRotation = rotation;
}

float Camera2D::GetRotation() const {
	return mRotation;
}

void Camera2D::SetScale(const float& scale) {
	mScale = scale;
}

float Camera2D::GetScale() const {
	return mScale;
}

glm::mat4 Camera2D::GetMatrix(const unsigned int& width, const unsigned int& height) {
	if (mUpdate || (width != mWidth || height != mHeight)) {
		mWidth = width;
		mHeight = height;
		
		glm::mat3 mat(1.0f,        0.0f, 0.0f,
					  0.0f,        1.0f, 0.0f,
			   mPosition.x, mPosition.y, 1.0f);
		
		mat *= util::GetTranslationMatrix(glm::vec2(mWidth - mPosition.x, mHeight - mPosition.y));
		mat *= util::GetRotationMatrix(mRotation);
		mat *= util::GetScalingMatrix(glm::vec2(mScale, mScale));
		mat *= util::GetTranslationMatrix(glm::vec2(-(mWidth - mPosition.x), -(mHeight - mPosition.y)));
		
		mView = glm::mat4(mat[0][0], mat[0][1], 0.0f, mat[0][2],
						  mat[1][0], mat[1][1], 0.0f, mat[1][2],
							   0.0f,      0.0f, 1.0f,      0.0f,
						  mat[2][0], mat[2][1], 0.0f, mat[2][2]);
		
		mUpdate = false;
	}
	
	return mView;
}
}
