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

/* #ifndef UAIRLAYOUTELEMENT_HPP
#define UAIRLAYOUTELEMENT_HPP

namespace uair {
// forward declare both container and division so we can store
// pointers to each of them
class LayoutContainer;
class LayoutDivision;

class LayoutElement {
	// allow both containers and divisions access to protected
	// member 'mPosition'
	friend class LayoutContainer;
	friend class LayoutDivision;
	
	public :
		LayoutElement() = default;
		LayoutElement(const unsigned int& width,
				const unsigned int& height);
		
		bool GetFloat() const;
		void SetFloat(const bool& flt);
		
		// return the position of the element in the coordinate
		// space specified
		glm::ivec2 GetPosition(const CoordinateSpace& coordinateSpace =
				CoordinateSpace::Local) const;
		
		unsigned int GetWidth() const;
		void SetWidth(const unsigned int width);
		
		unsigned int GetHeight() const;
		void SetHeight(const unsigned int height);
	
	public :
		// indication of this elements interaction with other elements in
		// the same layout object (true: ignore other elements; false:
		// offset based on other elements)
		bool mFloat = false;
	protected :
		glm::ivec2 mPosition;
		
		// the dimensions of this element that are used when positioning
		// it within a layout object
		unsigned int mWidth = 0u;
		unsigned int mHeight = 0u;
};
}

#endif */
