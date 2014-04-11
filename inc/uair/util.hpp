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

/** 
* \file		util.hpp
* \brief	Utility functions.
**/

#ifndef UAIRUTIL_HPP
#define UAIRUTIL_HPP

#include "init.hpp"

#if defined(UAIRWIN32)
	#include "utilwin32.hpp"
#elif defined(UAIRLINUX)
	#include "utillinux.hpp"
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace uair {
namespace util {
static const float EPSILON = 1e-6f;
static const double PI = 3.1415926535897;

extern glm::mat3 GetTranslationMatrix(const glm::vec2 & translation);
extern glm::mat3 GetRotationMatrix(const float & angle);
extern glm::mat3 GetScalingMatrix(const glm::vec2 & scale);
extern glm::mat3 GetSkewingMatrix(const glm::vec2 & skew);

extern unsigned int NextPowerOf2(const unsigned int & input);

template<typename T>
extern int SignOf(const T & input) {
	if (input < 0) {
		return -1;
	}
	else if (input > 0) {
		return 1;
	}
	
	return 0;
}
}
}

#endif
