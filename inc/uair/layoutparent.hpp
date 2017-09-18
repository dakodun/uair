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

#ifndef UAIRLAYOUTPARENT_HPP
#define UAIRLAYOUTPARENT_HPP

#include <glm/glm.hpp>

#include "hvector.hpp"
#include "shape.hpp"

namespace uair {
// forward declare both container and division so we can store
// pointers to each of them
class LayoutContainer;
class LayoutDivision;

// add a convenient typedef to store handles
typedef hvector<LayoutContainer>::handle ContainerHandle;
typedef hvector<LayoutContainer>::handle DivisionHandle;

// a class to make it easier to store and interact with a parent
// container or division without needing to know which one
class EXPORTDLL LayoutParent {
	private :
		// for a parent container we only need to store a pointer
		struct Container {
			LayoutContainer* mPtr;
		};
		
		// for a parent division we need to store a pointer and
		// the cell (row and column)
		struct Division {
			LayoutDivision* mPtr;
			unsigned int mX;
			unsigned int mY;
		};
	
	public :
		LayoutParent();
		LayoutParent(LayoutContainer* parentContainer);
		LayoutParent(LayoutDivision* parentDivision,
				const glm::uvec2& cell);
		
		// various functions to set the parent either to a null value,
		// a container or a division
		void SetParent();
		void SetParent(LayoutContainer* parentContainer);
		void SetParent(LayoutDivision* parentDivision,
				const glm::uvec2& cell);
		void SetParent(LayoutDivision* parentDivision);
		
		// check if a valid parent (container or division) exists
		explicit operator bool() const;
		
		// helper functions to retrieve the appropiate value from the parent
		// (note that GetWidth() and GetHeight() only apply to divisions
		// since they depend on their parent for their dimensions)
		glm::ivec2 GetPosition(const CoordinateSpace& coordinateSpace =
				CoordinateSpace::Local) const;
		unsigned int GetWidth() const;
		unsigned int GetHeight() const;
	
	private :
		// the type of current parent (0u for container, 1u for division)
		unsigned int mType = 0u;
		
		// store the parent struct in a union since we only
		// need one or the other, never both
		union Parent {
			Container mContainer;
			Division mDivision;
		} mParent;
};
}

#endif
