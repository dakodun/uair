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

#include "staticgeometry.hpp"

StaticGeometry::StaticGeometry(uair::Solver* solver, const std::vector<glm::vec2>& points, const glm::vec2& position) {
	mSolverPtr = solver;
	
	if (mSolverPtr) { // if we have a valid parent solver...
		// create a contour from the specified points
		uair::Contour contour(points);
		
		// create a collision shape from the contour
		uair::Polygon polygon;
		polygon.AddContour(contour);
		
		// create a static body, position it and add the collision shape
		mBody = mSolverPtr->CreateStaticBody();
		mBody.SetPosition(position);
		mBody.AddPolygon(polygon);
		mBody.SetMass(0.0f);
		
		// create a shape to match the body's collision shape
		mShape.AddContour(contour);
		mShape.SetDepth(-100.0f);
		mShape.SetPosition(position);
		mShape.SetColour(glm::vec3(0.6f, 0.1f, 0.2f));
	}
}

StaticGeometry::~StaticGeometry() {
	if (mSolverPtr) { // if we have a valid parent solver...
		mSolverPtr->DestroyBody(mBody);
	}
}
