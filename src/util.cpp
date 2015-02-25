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

#include "util.hpp"

#include <algorithm>
#include <iostream>

namespace uair {
namespace util {
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

extern glm::mat3 GetTranslationMatrix(const glm::vec2& translation) {
	glm::mat3 result;
	result[0][0] = 1.0f; result[1][0] = 0.0f; result[2][0] = translation.x;
	result[0][1] = 0.0f; result[1][1] = 1.0f; result[2][1] = translation.y;
	result[0][2] = 0.0f; result[1][2] = 0.0f; result[2][2] = 1.0f;
	
	return result;
}

extern glm::mat3 GetRotationMatrix(const float& angle) {
	float angleRad = (angle * (uair::util::PI / 180)); // convert the angle to radians
	
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
	glm::vec2 angleRad((std::max(-89.9f, std::min(skew.x, 89.9f)) * (uair::util::PI / 180)),
			(std::max(-89.9f, std::min(skew.y, 89.9f)) * (uair::util::PI / 180)));

	
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
	
	glm::vec2 projecction = ((glm::dot(AC, AB)) / (AB.x * AB.x + AB.y * AB.y)) * AB;
	glm::vec2 intersection = projecction - AC;
	
	glm::vec2 reflection = pointC + (2.0f * intersection);
	return reflection;
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
			return "our of memory";
		default :
			return "no error";
	}
}

/*
bool LineLineIntersection(const glm::vec2& firstPt, const glm::vec2& secondPt, const glm::vec2& thirdPt, const glm::vec2& fourthPt) {
	int polyFirstPt = PointSideOfLine(firstPt, secondPt, thirdPt);
	int polySecondPt = PointSideOfLine(firstPt, secondPt, fourthPt);
	int lineFirstPt = PointSideOfLine(thirdPt, fourthPt, firstPt);
	int lineSecondPt = PointSideOfLine(thirdPt, fourthPt, secondPt);
	
	if (polyFirstPt != 0 && polySecondPt != 0 && polyFirstPt != polySecondPt) {
		if (lineFirstPt != 0 && lineSecondPt != 0 && lineFirstPt != lineSecondPt) {
			return true;
		}
	}
	
	return false;
}

int PointSideOfLine(const glm::vec2& firstPt, const glm::vec2& secondPt, const glm::vec2& checkPt) {
	float cross = ((secondPt.x - firstPt.x) * (checkPt.y - firstPt.y)) - ((secondPt.y - firstPt.y) * (checkPt.x - firstPt.x)); // get 2d cross-product
	if (cross >= -util::EPSILON && cross <= util::EPSILON) { // if cross-product is within (-EPSILON <= cross <= EPSILON)...
		return 0; // point is on line
	}
	else {
		return util::SignOf(cross); // return side of point
	}
}

float Area(const std::vector<glm::vec2>& points) {
	float area = 0.0f;
	for (unsigned int i = 0u; i < points.size(); ++i) { // for all points...
		const glm::vec2& curr = points.at(i); // get current point
		const glm::vec2& next = points.at((i + 1u) % points.size()); // get next point
		
		area += (curr.x * next.y) - (next.x * curr.y); // add to current total area
	}
	
	return area; // return total area
}

bool PointInTriangle(const glm::vec2& prevPt, const glm::vec2& currPt, const glm::vec2& nextPt, const glm::vec2& checkPt) {
	glm::vec2 vecCurrNext(nextPt.x - currPt.x, nextPt.y - currPt.y);
	glm::vec2 vecNextPrev(prevPt.x - nextPt.x, prevPt.y - nextPt.y);
	glm::vec2 vecPrevCurr(currPt.x - prevPt.x, currPt.y - prevPt.y);
	
	glm::vec2 vecPrevCheck(checkPt.x - prevPt.x, checkPt.y - prevPt.y);
	glm::vec2 vecCurrCheck(checkPt.x - currPt.x, checkPt.y - currPt.y);
	glm::vec2 vecNextCheck(checkPt.x - nextPt.x, checkPt.y - nextPt.y);
	
	if ((((vecCurrNext.x * vecCurrCheck.y) - (vecCurrNext.y * vecCurrCheck.x)) > util::EPSILON) &&
			(((vecPrevCurr.x * vecPrevCheck.y) - (vecPrevCurr.y * vecPrevCheck.x)) > util::EPSILON) &&
			(((vecNextPrev.x * vecNextCheck.y) - (vecNextPrev.y * vecNextCheck.x)) > util::EPSILON)) {
		
		return true;
	}
	
	return false;
}

bool LineIntersectsPolygon(const std::vector<glm::vec2>& vertices, const std::vector<unsigned int>& indices, const glm::vec2& firstPt, const glm::vec2& secondPt) {
	for (unsigned int i = 0u; i < indices.size(); ++i) { // for all indices in polygon...
		const glm::vec2& curr = vertices.at(indices.at(i)); // get current vertex
		const glm::vec2& next = vertices.at(indices.at((i + 1u) % indices.size())); // get next point
		
		int polyFirstPt = PointSideOfLine(firstPt, secondPt, curr); // get side current vertex (polygon) is on
		int polySecondPt = PointSideOfLine(firstPt, secondPt, next); // get side next vertex (polygon) is on
		int lineFirstPt = PointSideOfLine(curr, next, firstPt); // get side first vertex (line) is on
		int lineSecondPt = PointSideOfLine(curr, next, secondPt); // get side second vertex (line) is on
		
		if (polyFirstPt != 0 && polySecondPt != 0 && polyFirstPt != polySecondPt) { // if sides are different (polygon)...
			if (lineFirstPt != 0 && lineSecondPt != 0 && lineFirstPt != lineSecondPt) { // if sides are different (line)...
				return true; // we have intersection
			}
		}
	}
	
	return false; // no intersection
}
*/
}
}
