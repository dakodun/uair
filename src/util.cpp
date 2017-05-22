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

#include "util.hpp"

#include <algorithm>
#include <iostream>
#include <fstream>

namespace uair {
namespace util {
const float EPSILON = 1e-6f;
const double PI = 3.1415926535897;
const double PIOVER180 = PI / 180;
const double C180OVERPI = 180 / PI;
unsigned int LOGLEVEL = 0u;
std::string LOGLOCATION = "";

extern bool CompareFloats(const float& first, const unsigned int& comparison, const float& second, const float& variance) {
	switch (comparison) {
		case 0 : { // less than
			float diff = first - second;
			if (diff < -variance) {
				return true;
			}
			
			break;
		}
		case 1 : { // less than
			float diff = first - second;
			// std::cout << diff << " " << -variance << std::endl;
			if (diff > -variance && diff < variance) {
				return true;
			}
			
			break;
		}
		case 2 : { // greater than
			float diff = first - second;
			// std::cout << diff << " " << -variance << std::endl;
			if (diff > variance) {
				return true;
			}
			
			break;
		}
	}
	
	return false;
}

extern int FloatToInt(const float& f) {
	if (f < 0) {
		return static_cast<int>(f - 0.5f);
	}
	else {
		return static_cast<int>(f + 0.5f);
	}
}

extern glm::mat3 GetTranslationMatrix(const glm::vec2& translation) {
	glm::mat3 result;
	result[0][0] = 1.0f; result[1][0] = 0.0f; result[2][0] = translation.x;
	result[0][1] = 0.0f; result[1][1] = 1.0f; result[2][1] = translation.y;
	result[0][2] = 0.0f; result[1][2] = 0.0f; result[2][2] = 1.0f;
	
	return result;
}

extern glm::mat3 GetRotationMatrix(const float& angle) {
	float angleRad = (angle * (util::PI / 180)); // convert the angle to radians
	
	glm::mat3 result;
	result[0][0] = std::cos(angleRad);	result[0][1] = -std::sin(angleRad);	result[0][2] = 0.0f;
	result[1][0] = std::sin(angleRad);	result[1][1] = std::cos(angleRad);	result[1][2] = 0.0f;
	result[2][0] = 0.0f;				result[2][1] = 0.0f;				result[2][2] = 1.0f;
	
	return result;
}

extern glm::mat3 GetScalingMatrix(const glm::vec2& scale) {
	glm::mat3 result;
	result[0][0] = scale.x;	result[0][1] = 0.0f;	result[0][2] = 0.0f;
	result[1][0] = 0.0f;	result[1][1] = scale.y;	result[1][2] = 0.0f;
	result[2][0] = 0.0f;	result[2][1] = 0.0f;	result[2][2] = 1.0f;
	
	return result;
}

extern glm::mat3 GetSkewingMatrix(const glm::vec2& skew) {
	// clamp the skew tangent to -90 < angle < 90 on both axes
	glm::vec2 angleRad((std::max(-89.9f, std::min(skew.x, 89.9f)) * (util::PI / 180)),
			(std::max(-89.9f, std::min(skew.y, 89.9f)) * (util::PI / 180)));

	
	glm::mat3 result;
	result[0][0] = 1.0f;					result[0][1] = -std::tan(angleRad.y);	result[0][2] = 0.0f;
	result[1][0] = -std::tan(angleRad.x);	result[1][1] = 1.0f;					result[1][2] = 0.0f;
	result[2][0] = 0.0f;					result[2][1] = 0.0f;					result[2][2] = 1.0f;
	
	return result;
}

extern unsigned int NextPowerOf2(const unsigned int& input) {
	unsigned int final = input;
	final--;
	
	for (unsigned int i = 1; i < sizeof(input) * 8; i *= 2) {
		final |= final >> i;
	}
	
	final++;
	return final;
}

extern int IsConvex(const glm::vec2& pointA, const glm::vec2& pointB, const glm::vec2& pointC) {
	// ||v1 x v2|| = ||v1|| * ||v2|| * abs(sin(a))
	// size of cross product = size of first * size of second * absolute value of sin of angle
	// since 2d, size of cross product is scalar
	// cross = ||v1|| * ||v2|| * sin(a)
	// cross / (||v1|| * ||v2||) = sin(a)
	// a = arcsin(cross / (||v1|| * ||v2||))
	// so if cross is > 0, angle is < 180 and thus convex
	// since origin is top-left (not bottom-left), and points are clockwise this is reversed
	
	glm::vec2 BA = pointA - pointB;
	glm::vec2 BC = pointC - pointB;
	
	float BACrossBC = (BA.x * BC.y) - (BC.x * BA.y);
	
	if (util::CompareFloats(BACrossBC, util::LessThan, 0.0f)) {
		return 1;
	}
	else if (util::CompareFloats(BACrossBC, util::GreaterThan, 0.0f)) {
		return -1;
	}
	else {
		return 0;
	}
}

extern glm::vec2 ReflectPointByLine(const glm::vec2& pointA, const glm::vec2& pointB, const glm::vec2& pointC) {
	// obtaining a reflection of a point over a line via reflection: http://math.stackexchange.com/a/41299
	
	glm::vec2 AB = pointB - pointA;
	glm::vec2 AC = pointC - pointA;
	
	glm::vec2 projection = ((glm::dot(AC, AB)) / (AB.x * AB.x + AB.y * AB.y)) * AB;
	glm::vec2 intersection = projection - AC;
	
	glm::vec2 reflection = pointC + (2.0f * intersection);
	return reflection;
}

extern glm::vec3 RotatePointAroundAxis(const glm::vec3& pointA, const glm::vec3& axis, const float& angle) {
	// rotation around an arbitrary axis using rodriguez formula: https://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle
	
	float angleRads = angle * static_cast<float>(util::PIOVER180);
	glm::vec3 unit = glm::normalize(axis);
	
	glm::mat3 identity(1,  0,  0,
					   0,  1,  0,
					   0,  0,  1);
	
	glm::mat3 outerProduct(unit.x * unit.x,  unit.x * unit.y,  unit.x * unit.z,
						   unit.x * unit.y,  unit.y * unit.y,  unit.y * unit.z,
						   unit.x * unit.z,  unit.y * unit.z,  unit.z * unit.z);
	
	glm::mat3 crossProduct(      0,  -unit.z,   unit.y,
							unit.z,        0,  -unit.x,
						   -unit.y,   unit.x,        0);
	
	glm::mat3 rotation = (static_cast<float>(cos(angleRads)) * identity) + (static_cast<float>(sin(angleRads)) * crossProduct) + (static_cast<float>((1 - cos(angleRads))) * outerProduct);
	
	return glm::normalize(rotation * pointA);
}

extern float AngleBetweenVectors(const glm::vec2& vecA, const glm::vec2& vecB) {
	float dot = (vecA.x * vecB.x) + (vecA.y * vecB.y);
	float det = (vecA.x * vecB.y) - (vecA.y * vecB.x);
	
	return (std::atan2(-det, -dot) + PI) * C180OVERPI;
}

extern std::string GetGLErrorStatus() {
	GLenum err = glGetError();
	switch (err) {
		case GL_INVALID_ENUM :
			return "invalid enum";
		case GL_INVALID_VALUE :
			return "invalid value";
		case GL_INVALID_OPERATION :
			return "invalid operation";
		case GL_INVALID_FRAMEBUFFER_OPERATION :
			return "invalid framebuffer operation";
		case GL_OUT_OF_MEMORY :
			return "out of memory";
		default :
			return "no error";
	}
}

extern void LogMessage(const unsigned int& level, const std::string& message) {
	if (level <= LOGLEVEL) {
		std::cerr << message << '\n';
	}
}

extern void LogMessage(const unsigned int& level, const std::string& message, const std::string& filename) {
	if (level <= LOGLEVEL) {
		std::streambuf *storedBuffer;
		std::ofstream file;
		file.open(filename.c_str(), std::ios::out | std::ios_base::app);
		
		if (file.is_open()) {
			storedBuffer = std::cerr.rdbuf();
			
			std::cerr.rdbuf(file.rdbuf());
			std::cerr << message << '\n';
			std::cerr.rdbuf(storedBuffer);
		}
	}
}

extern std::vector<std::string> SplitString(const std::string& string, const char& delimiter) {
	std::vector<std::string> strParts;
	std::stringstream strStream(string);
	std::string part;
	
	while(std::getline(strStream, part, delimiter)) {
		strParts.push_back(part);
	}
	
	return strParts;
}

extern std::u16string UTF8toUTF16(const std::string& input) {
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
	return convert.from_bytes(input);
}

extern std::string UTF16toUTF8(const std::u16string& input) {
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
	return convert.to_bytes(input);
}
}
}
