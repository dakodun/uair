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

#include "dynamicobject.hpp"

DynamicObject::DynamicObject(uair::Solver* solver, const std::vector<uair::Polygon>& polygons, const glm::vec2& position,
		const glm::vec2& velocity) {
	
	mSolverPtr = solver;
	
	if (mSolverPtr) { // if we have a valid parent solver...
		// create a static body, position it and set the velocity
		mBody = mSolverPtr->CreateDynamicBody();
		mBody.SetPosition(position);
		mBody.SetVelocity(velocity);
		
		for (const uair::Polygon& polygon: polygons) { // for all collision shapes...
			mBody.AddPolygon(polygon);
			
			// create a shape to match the collision shape
			uair::Shape shape;
			shape.AddContours(polygon.GetContours());
			shape.SetDepth(-100.0f);
			shape.SetPosition(position);
			shape.SetColour(glm::vec3(0.1f, 0.45f, 0.8f));
			mShapes.push_back(std::move(shape));
		}
		
		// create a shape to mimic the body's bounding circle
			std::pair<glm::vec2, float> circle = mBody.GetBoundingCircle();
			uair::Contour contour;
			for (unsigned int i = 0u; i < 360u; i += 20u) {
				float angle = i * uair::util::PIOVER180;
				contour.AddPoint(glm::vec2(cos(angle) * circle.second, sin(angle) * circle.second));
			}
			
			mBCircle.AddContour(contour);
			mBCircle.SetDepth(-100.0f);
			mBCircle.SetPosition(position);
			mBCircle.SetColour(glm::vec3(0.5f, 0.5f, 0.5f));
			mBCircle.SetOrigin(glm::vec2(-circle.first.x, -circle.first.y));
			mBCircle.mRenderMode = GL_LINE_LOOP;
		//
	}
}

DynamicObject::~DynamicObject() {
	if (mSolverPtr) { // if we have a valid parent solver...
		mSolverPtr->DestroyBody(mBody);
	}
}

void DynamicObject::UpdateString() {
	using namespace uair::util;
	
	// calculate the velocity vector's magnitude
	glm::vec2 velocity = mBody.GetVelocity();
	float mag = std::sqrt((velocity.x * velocity.x) + (velocity.y * velocity.y));
	
	// construct a string to display the properties of the dynamic body
		std::u16string string;
		string += u"v: ";
		string += UTF8toUTF16(ToString(mag, 1));
		string += u'\n';
		
		// calculate the mass from the inverse mass
		string += u"m: ";
		float mass = 0.0f;
		if (!CompareFloats(mBody.GetInvMass(), Equals, 0.0f)) {
			mass = 1u / mBody.GetInvMass();
		}
		
		string += UTF8toUTF16(ToString((mass), 0));
		string += u'\n';
		
		string += u"r: ";
		string += UTF8toUTF16(ToString(mBody.GetRestitution(), 1));
		string += u'\n';
		
		mInfoString.SetText(string);
	//
}
