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

#include "polygon.hpp"

#include "util.hpp"

namespace uair {
std::vector<glm::vec2> Polygon::GetPoints() const {
	return mPoints;
}

std::vector<glm::vec2> Polygon::GetAbsolutePoints() const {
	std::vector<glm::vec2> absolute = GetPoints();
	for (auto iter = absolute.begin(); iter != absolute.end(); ++iter) {
		*iter += mPosition;
	}
	
	return absolute;
}

void Polygon::AddPoint(const glm::vec2 & point) {
	mPoints.push_back(point);
	
	// check if the current point is outwith any of the bounds and if so adjust the bounds
	if (point.x < mBounds.at(0).x) { // left
		mBounds.at(0).x = point.x;
	}
	else if (point.x > mBounds.at(1).x) { // right
		mBounds.at(1).x = point.x;
	}
	
	if (point.y < mBounds.at(0).y) { // top
		mBounds.at(0).y = point.y;
	}
	else if (point.y > mBounds.at(1).y) { // bottom
		mBounds.at(1).y = point.y;
	}
}

void Polygon::AddPoints(const std::vector<glm::vec2> & points) {
	mPoints.insert(mPoints.end(), points.begin(), points.end());
	
	// check if the current point is outwith any of the bounds and if so adjust the bounds
	for (auto iter = points.begin(); iter != points.end(); ++iter) {
		if (iter->x < mBounds.at(0).x) { // left
			mBounds.at(0).x = iter->x;
		}
		else if (iter->x > mBounds.at(1).x) { // right
			mBounds.at(1).x = iter->x;
		}
		
		if (iter->y < mBounds.at(0).y) { // top
			mBounds.at(0).y = iter->y;
		}
		else if (iter->y > mBounds.at(1).y) { // bottom
			mBounds.at(1).y = iter->y;
		}
	}
}

void Polygon::AddAbsolutePoint(const glm::vec2 & point) {
	glm::vec2 relative = point - mPosition;
	AddPoint(relative);
}

void Polygon::AddAbsolutePoints(const std::vector<glm::vec2> & points) {
	std::vector<glm::vec2> relative;
	for (auto iter = points.begin(); iter != points.end(); ++iter) {
		relative.push_back(*iter - mPosition);
	}
	
	AddPoints(relative);
}

void Polygon::Clear() {
	mPoints.clear();
	
	mBounds = {glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f)};
}

glm::vec2 Polygon::GetPosition() const {
	return mPosition;
}

void Polygon::SetPosition(const glm::vec2 & newPos) {
	mPosition = newPos;
}

void Polygon::MakeRectangle(const glm::vec2 & position, const glm::vec2 & size) {
	Clear(); // clear the current polygon
	
	// create a rectangle of the specified dimensions
	SetPosition(position);
	AddPoint(glm::vec2(size.x,      0));
	AddPoint(glm::vec2(size.x, size.y));
	AddPoint(glm::vec2(     0, size.y));
}

void Polygon::MakeCircle(const glm::vec2 & position, const float & radius, const unsigned int & numPoints) {
	Clear(); // clear the current polygon
	
	SetPosition(position); // set the position to the specified
	
	float angle = 0; // the current angle
	float angleInc = (360 / numPoints) * (util::PI / 180); // the amount to increment the angle every iteration
	
	while (angle < util::PI * 2) { // while we don't have a full circle
		AddPoint(glm::vec2(radius * std::cos(angle), radius * std::sin(angle))); // add a point of the circle using current angle
		angle += angleInc; // update the angle
	}
}

std::vector<glm::vec2> Polygon::GetBounds() const {
	return mBounds;
}

void Polygon::Transform(const glm::mat3 & transform) {
	std::vector<glm::vec2> poly = GetAbsolutePoints(); // get the absolute points that make up the polygon
	poly.insert(poly.begin(), mPosition);
	Clear(); // clear this polygon
	
	for (auto iter = poly.begin(); iter != poly.end(); ++iter) { // for all points in the polygon
		glm::vec3 transPoint = transform * glm::vec3(*iter, 1.0f); // transform the current point
		
		if (iter == poly.begin()) { // if this is the first point
			SetPosition(glm::vec2(transPoint)); // set the point as the position
		}
		else { // otherwise for all other points
			// make the point relative to the position and add it
			AddPoint(glm::vec2(transPoint.x - mPosition.x, transPoint.y - mPosition.y));
		}
	}
}
}
