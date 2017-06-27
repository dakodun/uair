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

#ifndef DYNAMICOBJECT_HPP
#define DYNAMICOBJECT_HPP

#include "init.hpp"

class DynamicObject {
	public :
		DynamicObject(uair::Solver* solver, const std::vector<uair::Polygon>& polygons, const glm::vec2& position,
				const glm::vec2& velocity);
		~DynamicObject();
		
		void UpdateString();
	
	public :
		// a pointer to the solver that create the body belonging to this object
		uair::Solver* mSolverPtr = nullptr;
		
		uair::Solver::BodyHandle mBody;
		std::vector<uair::Shape> mShapes;
		uair::Shape mBCircle;
		
		// string to display velocity, mass and restitution
		uair::RenderString mInfoString;
};

#endif
