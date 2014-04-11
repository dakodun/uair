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
* \file		util.cpp
* \brief	
**/

#include "util.hpp"

#include <algorithm>
#include <iostream>

namespace uair {
namespace util {
extern glm::mat3 GetTranslationMatrix(const glm::vec2 & translation) {
	glm::mat3 result;
	result[0][0] = 1.0f; result[1][0] = 0.0f; result[2][0] = translation.x;
	result[0][1] = 0.0f; result[1][1] = 1.0f; result[2][1] = translation.y;
	result[0][2] = 0.0f; result[1][2] = 0.0f; result[2][2] = 1.0f;
	
	return result;
}

extern glm::mat3 GetRotationMatrix(const float & angle) {
	float angleRad = (angle * (uair::util::PI / 180)); // convert the angle to radians
	
	glm::mat3 result;
	result[0][0] = std::cos(angleRad);	result[0][1] = -std::sin(angleRad);	result[0][2] = 0.0f;
	result[1][0] = std::sin(angleRad);	result[1][1] = std::cos(angleRad);	result[1][2] = 0.0f;
	result[2][0] = 0.0f;				result[2][1] = 0.0f;				result[2][2] = 1.0f;
	
	return result;
}

extern glm::mat3 GetScalingMatrix(const glm::vec2 & scale) {
	glm::mat3 result;
	result[0][0] = scale.x;	result[0][1] = 0.0f;	result[0][2] = 0.0f;
	result[1][0] = 0.0f;	result[1][1] = scale.y;	result[1][2] = 0.0f;
	result[2][0] = 0.0f;	result[2][1] = 0.0f;	result[2][2] = 1.0f;
	
	return result;
}

extern glm::mat3 GetSkewingMatrix(const glm::vec2 & skew) {
	// clamp the skew tangent to -90 < angle < 90 on both axes
	glm::vec2 angleRad((std::max(-89.9f, std::min(skew.x, 89.9f)) * (uair::util::PI / 180)),
			(std::max(-89.9f, std::min(skew.y, 89.9f)) * (uair::util::PI / 180)));

	
	glm::mat3 result;
	result[0][0] = 1.0f;					result[0][1] = -std::tan(angleRad.y);	result[0][2] = 0.0f;
	result[1][0] = -std::tan(angleRad.x);	result[1][1] = 1.0f;					result[1][2] = 0.0f;
	result[2][0] = 0.0f;					result[2][1] = 0.0f;					result[2][2] = 1.0f;
	
	return result;
}

extern unsigned int NextPowerOf2(const unsigned int & input) {
	unsigned int final = input;
	final--;
	
	for (unsigned int i = 1; i < sizeof(input) * 8; i *= 2) {
		final |= final >> i;
	}
	
	final++;
	return final;
}
}
}
