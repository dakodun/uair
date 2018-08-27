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

#include "solver.hpp"

#include "util.hpp"

namespace uair {
Body& Solver::BodyHandle::GetBody() {
	return mBodyIter->first;
}

void Solver::BodyHandle::AddPolygon(Polygon polygon) {
	mBodyIter->first.AddPolygon(polygon);
}

const std::vector<Polygon>& Solver::BodyHandle::GetPolygons() const {
	return mBodyIter->first.GetPolygons();
}

std::pair<glm::vec2, float> Solver::BodyHandle::GetBoundingCircle() const {
	return mBodyIter->first.GetBoundingCircle();
}

std::string Solver::BodyHandle::GetTag() const {
	return mBodyIter->first.GetTag();
}

void Solver::BodyHandle::SetTag(const std::string& tag) {
	mBodyIter->first.SetTag(tag);
}

void Solver::BodyHandle::SetType(const unsigned int& type) {
	mBodyIter->first.SetType(type);
}

unsigned int Solver::BodyHandle::GetType() const {
	return mBodyIter->first.GetType();
}

bool Solver::BodyHandle::IsStatic() const {
	return mBodyIter->first.IsStatic();
}

bool Solver::BodyHandle::IsDynamic() const {
	return mBodyIter->first.IsDynamic();
}

glm::vec2 Solver::BodyHandle::GetPosition() const {
	return mBodyIter->first.GetPosition();
}

void Solver::BodyHandle::SetPosition(const glm::vec2& pos) {
	mBodyIter->first.SetPosition(pos);
}

glm::vec2 Solver::BodyHandle::GetVelocity() const {
	return mBodyIter->first.GetVelocity();
}

void Solver::BodyHandle::SetVelocity(const glm::vec2& velocity) {
	mBodyIter->first.SetVelocity(velocity);
}

float Solver::BodyHandle::GetInvMass() const {
	return mBodyIter->first.GetInvMass();
}

void Solver::BodyHandle::SetMass(const float& mass) {
	mBodyIter->first.SetMass(mass);
}

float Solver::BodyHandle::GetRestitution() const {
	return mBodyIter->first.GetRestitution();
}

void Solver::BodyHandle::SetRestitution(const float& res) {
	mBodyIter->first.SetRestitution(res);
}


void Solver::Step(float deltaTime) {
	// for all dynamic bodies...
	for (auto iterStart = mDynamicBodies.begin(); iterStart != mDynamicBodies.end(); ++iterStart) {
		// move the body by its velocity (using the time since last frame)
		iterStart->first.SetPosition(iterStart->first.GetPosition() +
				(iterStart->first.GetVelocity() * deltaTime));
	}
	
	// for all dynamic bodies...
	for (auto iterStart = mDynamicBodies.begin(); iterStart != mDynamicBodies.end(); ++iterStart) {
		// for all OTHER dynamic bodies (from current body to list end)...
		for (auto iterCurr = std::next(iterStart); iterCurr != mDynamicBodies.end(); ++iterCurr) {
			// detect and resolve any collisions between both bodies
			// HandleBodyCollision(iterStart->first, iterCurr->first);
			HandleBodyCollision(*iterStart, *iterCurr);
		}
		
		// for all static bodies...
		for (auto iterStatic = mStaticBodies.begin(); iterStatic != mStaticBodies.end(); ++iterStatic) {
			// HandleBodyCollision(iterStart->first, iterStatic->first);
			HandleBodyCollision(*iterStart, *iterStatic);
		}
	}
}

Solver::BodyHandle Solver::CreateStaticBody(const BodyCallback& callback) {
	// add a new body to the list and specifiy its type
	mStaticBodies.emplace_back(0u, callback);
	
	// create the handle and set the matching type and an iterator to the new body
	BodyHandle handle;
	handle.mBodyIter = std::prev(mStaticBodies.end());
	handle.SetMass(0.0f);
	
	return handle;
}

Solver::BodyHandle Solver::CreateDynamicBody(const BodyCallback& callback) {
	mDynamicBodies.emplace_back(1u, callback);
	
	BodyHandle handle;
	handle.mBodyIter = std::prev(mDynamicBodies.end());
	handle.SetType(1u);
	
	return handle;
}

void Solver::DestroyBody(BodyHandle& handle) {
	switch (handle.GetType()) { // depending on the type of body stored in the handle...
		case 0u :
			try {
				// remove the body from the store via the iterator
				mStaticBodies.erase(handle.mBodyIter);
			} catch (std::exception& e) {
				std::cout << e.what() << std::endl;
			}
			
			break;
		case 1u :
			try {
				mDynamicBodies.erase(handle.mBodyIter);
			} catch (std::exception& e) {
				std::cout << e.what() << std::endl;
			}
			
			break;
		default :
			break;
	}
}

bool Solver::AddCallback(const std::string& tagFirst, const std::string& tagSecond,
		const CollisionCallback& callbackNew) {
	
	// indication of exisitng callback for body types being replaced
	bool result = false;
	
	// construct the callback key from body types and ensure it is in ascending alphabetical order (first < second)
	std::pair<std::string, std::string> callbackKey = std::make_pair(tagFirst, tagSecond);
	if (tagFirst > tagSecond) {
		callbackKey = std::make_pair(tagSecond, tagFirst);
	}
	
	// attempt to retrieve the existing callback for the key and remove it if so (indicating as such)
	auto callbackOld = mCallbacks.find(callbackKey);
	if (callbackOld != mCallbacks.end()) {
		mCallbacks.erase(callbackOld);
		result = true;
	}
	
	// if a new callback is to be registered then do so
	if (callbackNew) {
		mCallbacks.insert(std::make_pair(callbackKey, callbackNew));
	}
	
	return result;
}

// bool Solver::HandleBodyCollision(Body& first, Body& second) {
bool Solver::HandleBodyCollision(BodyCallbackEntry& first, BodyCallbackEntry& second) {
	Body& firstBody = first.first;
	Body& secondBody = second.first;
	
	// indicate if a collision occurred
	bool collision = false;
	
	// get the bounding circles for both bodies
	std::pair<glm::vec2, float> firstCircle = firstBody.GetBoundingCircle();
	std::pair<glm::vec2, float> secondCircle = secondBody.GetBoundingCircle();
	
	// calculate the (squared) distance between both circle's centre-points
	glm::vec2 vecCircle = (secondCircle.first + secondBody.GetPosition()) -
			(firstCircle.first + firstBody.GetPosition());
	float distSqCircle = (vecCircle.x * vecCircle.x) + (vecCircle.y * vecCircle.y);
	
	// calculate the sum of both circle's radii (squared since distance is squared)
	float sumSqRadii = secondCircle.second + firstCircle.second;
	sumSqRadii = sumSqRadii * sumSqRadii;
	
	if (distSqCircle < sumSqRadii) { // if the circles intersect...
		// get the collision shapes for both bodies
		std::vector<Polygon> firstPolygons = firstBody.GetPolygons();
		std::vector<Polygon> secondPolygons = secondBody.GetPolygons();
		
		// get and iterator to the first collision shape of both bodies
		auto firstPoly = firstPolygons.begin();
		auto secondPoly = secondPolygons.begin();
		
		// while we don't have a collision and still have polygons to test...
		while (!collision && firstPoly != firstPolygons.end()) {
			// test the current polygons for a collision and store the result
			auto collisionResult = CheckPolygonCollision(*firstPoly, *secondPoly);
			if (std::get<0>(collisionResult)) { // if a collision occurred...
				// construct the callback key from body types and ensure it is in ascending alphabetical order (first < second)
				std::pair<std::string, std::string> callbackKey = std::make_pair(firstBody.GetTag(), secondBody.GetTag());
				if (firstBody.GetTag() > secondBody.GetTag()) {
					callbackKey = std::make_pair(secondBody.GetTag(), firstBody.GetTag());
				}
				
				bool performDefault = true;
				
				// attempt to retrieve the stored callback function and call it passing in current bodies
				auto callback = mCallbacks.find(callbackKey);
				if (callback != mCallbacks.end()) {
					performDefault = callback->second(firstBody, secondBody);
				}
				
				if (performDefault) { // if we're to perform default response...
					// get the relative position between both bodies centres
					glm::vec2 rPos = (secondPoly->GetCentroid() + secondPoly->GetPosition()) -
							(firstPoly->GetCentroid() + firstPoly->GetPosition());
					
					// get the minimum translation normal (collision normal) and distance
					glm::vec2 mtNorm = std::get<1>(collisionResult);
					float mtDist = std::get<2>(collisionResult);
					
					// if the mt normal points in the opposite direction...
					if (util::CompareFloats((mtNorm.x * rPos.x) + (mtNorm.y * rPos.y), util::GreaterThanEquals, 0.0f)) {
						mtNorm = -mtNorm; // reverse the direction of the mt normal
					}
					
					// store the current velocities before they are modified via impulse
					glm::vec2 velFirst = firstBody.GetVelocity();
					glm::vec2 velSecond = secondBody.GetVelocity();
					
					// resolve the collision by applying an impulse
					ApplyImpulse(firstBody, secondBody, mtNorm);
					
					// seperate shapes along collision normal depending on mass, velocity and body type
					PerformSeperation(firstBody, secondBody, mtNorm, mtDist, velFirst, velSecond);
				}
				
				collision = true;
				
				if (first.second) {
					(first.second)(firstBody, secondBody, collisionResult);
				}
				
				if (second.second) {
					(second.second)(secondBody, firstBody, collisionResult);
				}
			}
			else { // otherwise no collision occurred...
				secondPoly = std::next(secondPoly); // retrieve the next collision shape for the SECOND body
				
				if (secondPoly == secondPolygons.end()) { // if there are no more collision shapes for the SECOND body...
					firstPoly = std::next(firstPoly); // retrieve the next collision shape for the FIRST body
					secondPoly = secondPolygons.begin(); // reset back to the first collision shape for the SECOND body
				}
			}
		}
	}
	
	return collision;
}

std::tuple<bool, glm::vec2, float> Solver::CheckPolygonCollision(const Polygon& first, const Polygon& second) {
	// add both polygons to a vector to allow iteration
	std::vector<Polygon> polygons = {first, second};
	
	glm::vec2 direction; // the direction vector (unit) along which to seperate the shapes
	float length = std::numeric_limits<float>::max(); // the distance to seperate the shapes
	
	for (const Polygon& polygon: polygons) { // for both polygons...
		// get the contours that constitute the polygon
		std::vector<Contour> contours = polygon.GetContours();
		glm::vec2 posOffset = polygon.GetPosition(); // get the point offset
		
		for (const Contour& contour: contours) { // for all contours in polygon...
			// get the points that constitute the contour
			std::vector<glm::vec2> points = contour.GetPoints();
			
			for (unsigned int i = 0u; i < points.size(); ++i) { // for all points in contour...
				// clockwise winding
				// get the edge from current point to next point (wrap around to first point)
				glm::vec2 edge = points.at((i + 1u) % (points.size() - 1u)) - points.at(i);
				glm::vec2 axis(-edge.y, edge.x); // get the left-hand normal
				
				// normalise the axis so we can retrieve an accurate minimum translation (mt) distance
				// (this is relatively expensive but can be skipped if mt is not required)
				float mag = std::sqrt((axis.x * axis.x) + (axis.y * axis.y));
				axis.x /= mag;
				axis.y /= mag;
				
				// get the projection of both polygons onto the axis
				glm::vec2 firstProj = first.GetProjection(axis);
				glm::vec2 secondProj = second.GetProjection(axis);
				
				// if there is no overlap between the projections then exit early (no collision)
				if (secondProj.y < firstProj.x || firstProj.y < secondProj.x) {
					return std::make_tuple(false, glm::vec2(0.0f, 0.0f), 0.0f);
				}
				
				// calculate the mt and update the seperation distance (mt) and direction (axis)
				float mt = std::min(firstProj.y, secondProj.y) - std::max(firstProj.x, secondProj.x);
				if (mt < length) {
					direction = axis;
					length = mt;
				}
			}
		}
	}
	
	return std::make_tuple(true, direction, length);
}

void Solver::ApplyImpulse(Body& first, Body& second, const glm::vec2& collNorm) {
	// get the relative velocity between both bodies
	glm::vec2 rVel = second.GetVelocity() - first.GetVelocity();
	
	// get the relative velocity in the direction of the collision normal
	float rVelNorm = (collNorm.x * rVel.x) + (collNorm.y * rVel.y);
	
	if (util::CompareFloats(rVelNorm, util::GreaterThan, 0.0f)) { // if the bodies are moving towards each other...
		// get the restitution value we should use
		float restitution = std::min(first.GetRestitution(), second.GetRestitution());
		
		// calculate the sum of both body's inverse mass
		float sumInvMass = first.GetInvMass() + second.GetInvMass();
		
		if (util::CompareFloats(sumInvMass, util::Equals, 0.0f)) { // if both of the bodies have infinite mass...
			// stop both of them
			first.SetVelocity(glm::vec2(0.0f, 0.0f));
			second.SetVelocity(glm::vec2(0.0f, 0.0f));
		}
		else { // otherwise at least one of the objects has a non-infinite mass...
			// calculate the impulse vector to apply to the bodies
			float impulse = (-(1.0f + restitution) * rVelNorm) / sumInvMass;
			glm::vec2 impulseVec = impulse * collNorm;
			
			// apply the impulse vector to the first body
			first.SetVelocity(first.GetVelocity() - (first.GetInvMass() * impulseVec));
			
			if (second.IsDynamic()) { // if the second body is not static...
				// apply the impulse vector to the second body in the opposite direction
				second.SetVelocity(second.GetVelocity() + (second.GetInvMass() * impulseVec));
			}
		}
	}
}

void Solver::PerformSeperation(Body& first, Body& second, const glm::vec2& collNorm, const float& collDist,
		const glm::vec2& velocityFirst, const glm::vec2& velocitySecond) {
	
	float dist = std::max(collDist - 0.01f, 0.0f);
	
	// calculate the sum of both body's inverse mass
	float sumInvMass = first.GetInvMass() + second.GetInvMass();
	
	if (util::CompareFloats(sumInvMass, util::Equals, 0.0f)) { // if both of the bodies have infinite mass...
		glm::vec2 seperation = collNorm * (dist * 0.6f);
		
		if (second.IsStatic()) { // if the other body is static...
			// seperate by moving the dynamic body the full seperation distance
			first.SetPosition(first.GetPosition() + seperation);
		}
		else { // otherwise both bodies are dynamic...
			// depending on which body (if any) is moving...
			if (util::CompareFloats(velocityFirst.x, util::Equals, 0.0f) &&
					util::CompareFloats(velocityFirst.y, util::Equals, 0.0f)) {
				
				second.SetPosition(second.GetPosition() - seperation); // adjust the body that is in motion
			}
			else if (util::CompareFloats(velocitySecond.x, util::Equals, 0.0f) &&
					util::CompareFloats(velocitySecond.y, util::Equals, 0.0f)) {
				
				first.SetPosition(first.GetPosition() + seperation);
			}
			else {
				// adjust both (in motion) bodies
				first.SetPosition(first.GetPosition() + (seperation * 0.5f));
				second.SetPosition(second.GetPosition() - (seperation * 0.5f));
			}
		}
	}
	else { // otherwise at least one of the bodies has a finite mass...
		// seperate both bodies relative to their masses
		glm::vec2 seperation = collNorm * ((dist / sumInvMass) * 0.6f);
		
		first.SetPosition(first.GetPosition() + (seperation * first.GetInvMass()));
		second.SetPosition(second.GetPosition() - (seperation * second.GetInvMass()));
	}
}
}
