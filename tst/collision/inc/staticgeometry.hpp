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

#ifndef STATICGEOMETRY_HPP
#define STATICGEOMETRY_HPP

#include "init.hpp"

class StaticGeometry {
	public :
		StaticGeometry(uair::Solver* solver, const std::vector<glm::vec2>& points, const glm::vec2& position);
		~StaticGeometry();
	
	public :
		// a pointer to the solver that create the body belonging to this object
		uair::Solver* mSolverPtr = nullptr;
		
		uair::Solver::BodyHandle mBody;
		uair::Shape mShape;
};

#endif
