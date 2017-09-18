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

/* #include "layoutparent.hpp"

#include "layoutcontainer.hpp"
#include "layoutdivision.hpp"

namespace uair {
LayoutParent::LayoutParent() {
	SetParent(); // initialise the parent to a nullptr
}

LayoutParent::LayoutParent(LayoutContainer* parentContainer) {
	SetParent(parentContainer);
}

LayoutParent::LayoutParent(LayoutDivision* parentDivision,
		const glm::uvec2& cell) {
	
	SetParent(parentDivision, cell);
}

void LayoutParent::SetParent() {
	mType = 0u;
	mParent.mDivision.mPtr = nullptr;
}

void LayoutParent::SetParent(LayoutContainer* parentContainer) {
	mType = 0u;
	mParent.mContainer.mPtr = parentContainer;
}

void LayoutParent::SetParent(LayoutDivision* parentDivision,
		const glm::uvec2& cell) {
	
	mType = 1u;
	mParent.mDivision.mPtr = parentDivision;
	mParent.mDivision.mX = cell.x;
	mParent.mDivision.mY = cell.y;
}

void LayoutParent::SetParent(LayoutDivision* parentDivision) {
	// set the division pointer but leave the cell untouched (essentially
	// we are updating the division address)
	mType = 1u;
	mParent.mDivision.mPtr = parentDivision;
}

LayoutParent::operator bool() const {
	// check for a valid parent pointer depending on parent type
	if ((mType == 0u && mParent.mContainer.mPtr) ||
			(mType == 1u && mParent.mDivision.mPtr)) {
		
		return true;
	}
	
	return false;
}

glm::ivec2 LayoutParent::GetPosition(const CoordinateSpace&
		coordinateSpace) const {
	
	// depending on the current (if any) parent type...
	if (mType == 0u && mParent.mContainer.mPtr) {
		// recursively call GetPosition() functions until we reach the
		// top of the parent-child tree and return the absolute position
		return mParent.mContainer.mPtr->GetPosition(coordinateSpace);
	}
	else if (mType == 1u && mParent.mDivision.mPtr) {
		glm::uvec2 cell(mParent.mDivision.mX, mParent.mDivision.mY);
		
		return mParent.mDivision.mPtr->GetPosition(cell, coordinateSpace);
	}
	
	// no valid parent is set so return a default position
	return glm::ivec2(0, 0);
}

unsigned int LayoutParent::GetWidth() const {
	if (mType == 0u && mParent.mContainer.mPtr) {
		// return the width of the parent container
		return mParent.mContainer.mPtr->GetWidth();
	}
	else if (mType == 1u && mParent.mDivision.mPtr) {
		glm::uvec2 cell(mParent.mDivision.mX, mParent.mDivision.mY);
		
		// return the width of the parent cell
		return mParent.mDivision.mPtr->GetWidth(cell);
	}
	
	// no valid parent is set so return a default width
	return 0u;
}

unsigned int LayoutParent::GetHeight() const {
	if (mType == 0u && mParent.mContainer.mPtr) {
		return mParent.mContainer.mPtr->GetHeight();
	}
	else if (mType == 1u && mParent.mDivision.mPtr) {
		glm::uvec2 cell(mParent.mDivision.mX, mParent.mDivision.mY);
		
		return mParent.mDivision.mPtr->GetHeight(cell);
	}
	
	return 0u;
}
} */
