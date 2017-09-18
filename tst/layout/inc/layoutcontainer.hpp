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

/* #ifndef UAIRLAYOUTCONTAINER_HPP
#define UAIRLAYOUTCONTAINER_HPP

#include <glm/glm.hpp>

#include "layoutparent.hpp"

namespace uair {
// allow storage of divisions
class LayoutDivision;

// allow interaction with element for positioning
class LayoutElement;

class LayoutContainer {
	// allow a division access to parent object
	friend class LayoutDivision;
	
	public :
		// an aggregate class to help initialise a container after
		// default construction
		struct Properties {
			glm::ivec2 mPosition;
			unsigned int mWidth;
			unsigned int mHeight;
		};
	
	public :
		LayoutContainer() = default;
		LayoutContainer(const glm::ivec2& position,
				const unsigned int& width, const unsigned int& height);
		
		// create a container as a child of another container
		LayoutContainer(const glm::ivec2& position,
				const unsigned int& width, const unsigned int& height,
				LayoutContainer* parentContainer);
		
		// create a container as a child of a division
		LayoutContainer(const glm::ivec2& position,
				const unsigned int& width, const unsigned int& height,
				LayoutDivision* parentDivision, const glm::uvec2& cell);
		
		LayoutContainer(const LayoutContainer& other) = delete;
		LayoutContainer(LayoutContainer&& other);
		
		LayoutContainer& operator=(LayoutContainer other);
		
		friend void swap(LayoutContainer& first, LayoutContainer& second) {
			using std::swap;
			
			swap(first.mPosition, second.mPosition);
			swap(first.mWidth, second.mWidth);
			swap(first.mHeight, second.mHeight);
			
			swap(first.mContainers, second.mContainers);
			swap(first.mDivisions, second.mDivisions);
			swap(first.mParent, second.mParent);
			
			swap(first.mHorizontalAlign, second.mHorizontalAlign);
			swap(first.mVerticalAlign, second.mVerticalAlign);
			
			swap(first.mContainerReserve, second.mContainerReserve);
			swap(first.mDivisionReserve, second.mDivisionReserve);
			
			// ensure the parents of any nested containers and divisions
			// are properly updated
			first.UpdateParents(&first);
			second.UpdateParents(&second);
		}
		
		void FromProperties(const Properties& properties);
		
		// get or set the container's position either relative to its
		// parent (if any) or the window (local or global respectively)
		glm::ivec2 GetPosition(const CoordinateSpace& coordinateSpace =
				CoordinateSpace::Local) const;
		void SetPosition(const glm::ivec2& position);
		
		unsigned int GetWidth() const;
		void SetWidth(const unsigned int& width);
		
		unsigned int GetHeight() const;
		void SetHeight(const unsigned int& height);
		
		// add, remove or retrieve a child container
		ContainerHandle AddContainer(const glm::ivec2& position,
				const unsigned int& width, const unsigned int& height);
		void RemoveContainer(const ContainerHandle& handle);
		LayoutContainer& GetContainer(const ContainerHandle& handle);
		
		// add, remove or retrieve a child division
		DivisionHandle AddDivision(const glm::uvec2& cells);
		void RemoveDivision(const DivisionHandle& handle);
		LayoutDivision& GetDivision(const DivisionHandle& handle);
		
		std::string GetHorizontalAlign() const;
		void SetHorizontalAlign(const std::string& align);
		std::string GetVerticalAlign() const;
		void SetVerticalAlign(const std::string& align);
		
		// position a list of element pointers within the container
		// depending on their size and 'float' flag
		void PositionElements(std::list<LayoutElement*> elements);
		
		unsigned int GetContainerReserve() const;
		void SetContainerReserve(const unsigned int& reserve);
		unsigned int GetDivisionReserve() const;
		void SetDivisionReserve(const unsigned int& reserve);
	private :
		// update the parents of all child containers and divisions
		void UpdateParents(LayoutContainer* parent);
	
	private :
		glm::ivec2 mPosition = glm::ivec2(0, 0);
		unsigned int mWidth = 0u;
		unsigned int mHeight = 0u;
		
		// a handle-based vector to store child containers
		hvector<LayoutContainer> mContainers;
		
		// a handle-based vector to store child divisions
		hvector<LayoutDivision> mDivisions;
		
		// a struct holding parent information (container or division)
		LayoutParent mParent;
		
		// the type of horizontal align (left, centre and right)
		std::string mHorizontalAlign = "left";
		
		// the type of veritcal align (top, middle, bottom)
		std::string mVerticalAlign = "top";
		
		// the default and increase in container and division hvectors
		// capacity (useful to prevent reallocation and pointer invalidation)
		unsigned int mContainerReserve = 1u;
		unsigned int mDivisionReserve = 1u;
	
	#ifdef _UAIR_DEBUG
	public :
		// if debug mode is active then allow retrieval of outlines
		// representing this container and all its children
		std::vector<Shape> GetShapes();
	#endif
};
}

#endif */
