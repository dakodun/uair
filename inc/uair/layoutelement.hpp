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

#ifndef UAIRLAYOUTELEMENT_HPP
#define UAIRLAYOUTELEMENT_HPP

#include "init.hpp"
#include "layoutcontainer.hpp"
#include "layoutdivision.hpp"
#include "polygon.hpp"

namespace uair {
class EXPORTDLL LayoutElement {
	friend class LayoutContainer;
	friend class LayoutDivision;
	
	public :
		glm::vec2 GetPosition(const CoordinateSpace& coordinateSpace = CoordinateSpace::Local) const;
		
		void AssignToContainer(LayoutContainer* container);
		void AssignToDivision(LayoutDivision* division, const unsigned int& row, const unsigned int & column);
	
	protected :
		glm::vec2 mPosition = glm::vec2(0.0f);
		
		LayoutPtr<LayoutContainer> mLayoutContainer;
		LayoutPtr<LayoutDivision> mLayoutDivision;
		unsigned int mRow = 0u;
		unsigned int mColumn = 0u;
};
}

#endif
