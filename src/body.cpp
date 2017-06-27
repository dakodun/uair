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

#include "body.hpp"

#include "util.hpp"

namespace uair {
void Body::BoundingCircle::AddPoint(const glm::vec2& point) {
	AddPoints({point});
}

void Body::BoundingCircle::AddPoints(const std::vector<glm::vec2>& points) {
	for (const glm::vec2& point: points) { // for all points being added...
		mPointTotal += point; // add the point to the current sum total
		++mNumPoints; // increment the point count
	}
	
	// calculate the centre-point of the circle by finding the average
	mCentre = glm::vec2(mPointTotal.x / mNumPoints, mPointTotal.y / mNumPoints);
	
	float radius = 0.0f; // the (squared) distance from the centre point to the furthest point point
	for (const glm::vec2& point: points) { // for all points being added...
		glm::vec2 vecCP = point - mCentre; // get the vector from the centre to the current points
		float distSq = (vecCP.x * vecCP.x) + (vecCP.y * vecCP.y); // get the squared distance of the vector
		
		// if the new squared distance is further than the previous highest update it
		radius = std::max(radius, distSq);
	}
	
	// to find the true distance we need to find the square root
	mRadius = std::sqrt(radius);
}

std::pair<glm::vec2, float> Body::BoundingCircle::GetCircle() const {
	return std::make_pair(mCentre, mRadius);
}


Body::Body(const unsigned int& type) : mType(type) {
	
}

void Body::AddPolygon(Polygon polygon) {
	// [todo] sanitise polygons
		// split complex polygons (concave, holes, distinct countours) into simple polygons
	
	polygon.SetPosition(mPosition); // set the position of the new collision shape to match the body's position
	
	std::vector<Contour> contours = polygon.GetContours(); // get the contours that constitute the polygon
	for (const Contour& contour: contours) { // for all contours in polygon...
		mBoundingCircle.AddPoints(contour.GetPoints()); // update the bounding circle's centre-point and radius
	}
	
	mPolygons.push_back(std::move(polygon)); // add the new collision shape
}

const std::vector<Polygon>& Body::GetPolygons() const {
	return mPolygons;
}

std::pair<glm::vec2, float> Body::GetBoundingCircle() const {
	return mBoundingCircle.GetCircle();
}

std::string Body::GetTag() const {
	return mTag;
}

void Body::SetTag(const std::string& tag) {
	mTag = tag;
}

void Body::SetType(const unsigned int& type) {
	mType = type % 2u;
}

unsigned int Body::GetType() const {
	return mType;
}

bool Body::IsStatic() const {
	return (mType == 0u);
}

bool Body::IsDynamic() const {
	return (mType == 1u);
}

glm::vec2 Body::GetPosition() const {
	return mPosition;
}

void Body::SetPosition(const glm::vec2& pos) {
	for (Polygon& polygon: mPolygons) { // for all collision shapes...
		polygon.SetPosition(pos); // update the position to match the body's new position
	}
	
	mPosition = pos; // update the body's position
}

glm::vec2 Body::GetVelocity() const {
	return mVelocity;
}

void Body::SetVelocity(const glm::vec2& velocity) {
	mVelocity = velocity;
}

float Body::GetInvMass() const {
	return mInvMass;
}

void Body::SetMass(const float& mass) {
	if (util::CompareFloats(mass, util::Equals, 0.0f)) {
		mInvMass = 0.0f;
	}
	else {
		mInvMass = 1.0f / mass;
	}
}

float Body::GetRestitution() const {
	return mRestitution;
}

void Body::SetRestitution(const float& res) {
	mRestitution = std::min(1.0f, std::max(0.0f, res));
}
}
