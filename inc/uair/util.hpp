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

#ifndef UAIRUTIL_HPP
#define UAIRUTIL_HPP

#include <sstream>
#include <string>

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
extern unsigned int LOGLEVEL;
extern std::string LOGLOCATION;

enum {
	LessThan = 0u,
	Equals,
	GreaterThan
};

extern bool CompareFloats(const float& first, const unsigned int& comparison, const float& second, const float& variance = util::EPSILON);

extern glm::mat3 GetTranslationMatrix(const glm::vec2& translation);
extern glm::mat3 GetRotationMatrix(const float& angle);
extern glm::mat3 GetScalingMatrix(const glm::vec2& scale);
extern glm::mat3 GetSkewingMatrix(const glm::vec2& skew);

extern unsigned int NextPowerOf2(const unsigned int& input);
extern int IsConvex(const glm::vec2& pointA, const glm::vec2& pointB, const glm::vec2& pointC);
extern glm::vec2 ReflectPointByLine(const glm::vec2& pointA, const glm::vec2& pointB, const glm::vec2& pointC);

extern std::string GetGLErrorStatus();
extern void LogMessage(const unsigned int& level, const std::string& message);
extern void LogMessage(const unsigned int& level, const std::string& message, const std::string& filename);

template<typename T>
extern int SignOf(const T& input) {
	if (input < 0) {
		return -1;
	}
	else if (input > 0) {
		return 1;
	}
	
	return 0;
}

template <class T>
extern std::string ToString(const T& t, int precision = -1) {
	std::stringstream ss;
	
	if (precision >= 0) {
		ss.precision(precision);
	}
	
	ss << t;
	
	return ss.str();
}

template <class T>
extern T FromString(const std::string& s, std::ios_base& (*f)(std::ios_base&) = std::dec) {
	T t;
	
	std::istringstream iss(s);
	iss >> f >> t;
	
	return t;
}
}
}

#endif
