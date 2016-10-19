/* **************************************************************** **
**	Uair Engine
**	Copyright (c) 2010 - 2016, Iain M. Crawford
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

#include "layoutelement.hpp"

namespace uair {
glm::vec2 LayoutElement::GetPosition(const CoordinateSpace& coordinateSpace) const {
	if (coordinateSpace == CoordinateSpace::Global) { // if we want to get the absolute position...
		if (mLayoutContainer) { // if we have an associated container...
			// recursively call getposition() functions until we reach the top of the parent-child tree and return the absolute position
			return mLayoutContainer.GetConst()->GetPosition(coordinateSpace) + mPosition;
		}
		else if (mLayoutDivision) { // otherwise if we have an associated division...
			// recursively call getposition() functions until we reach the top of the parent-child tree and return the absolute position
			return mLayoutDivision.GetConst()->GetCell(mRow, mColumn).GetPosition(coordinateSpace) + mPosition;
		}
	}
	
	return mPosition;
}

void LayoutElement::AssignToContainer(LayoutContainer* container) {
	mLayoutContainer.Unset();
	mLayoutDivision.Unset();
	mRow = 0u;
	mColumn = 0u;
	
	mLayoutContainer.Set(container);
}

void LayoutElement::AssignToDivision(LayoutDivision* division, const unsigned int& row, const unsigned int & column) {
	mLayoutContainer.Unset();
	mLayoutDivision.Unset();
	mRow = 0u;
	mColumn = 0u;
	
	mLayoutDivision.Set(division);
	mRow = row;
	mColumn = column;
}
}
