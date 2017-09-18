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

/* #ifndef UAIRLAYOUTDIVISION_HPP
#define UAIRLAYOUTDIVISION_HPP

#include <glm/glm.hpp>

#include "layoutparent.hpp"

namespace uair {
// allow storage of containers
class LayoutContainer;

// allow interaction with element for positioning
class LayoutElement;

class LayoutDivision {
	// allow a container access to parent object
	friend class LayoutContainer;
	
	public :
		struct Cell {
			public :
				Cell() = default;
				Cell(const glm::ivec2& position, const unsigned int& width,
						const unsigned int& height);
				
				Cell(const Cell& other) = delete;
				Cell(Cell&& other);
				
				Cell& operator=(Cell other);
				
				friend void swap(Cell& first, Cell& second) {
					using std::swap;
					
					swap(first.mPosition, second.mPosition);
					swap(first.mWidth, second.mWidth);
					swap(first.mHeight, second.mHeight);
					
					swap(first.mContainers, second.mContainers);
					swap(first.mDivisions, second.mDivisions);
					
					swap(first.mContainerReserve, second.mContainerReserve);
					swap(first.mDivisionReserve, second.mDivisionReserve);
				}
			
			public :
				glm::ivec2 mPosition = glm::ivec2(0, 0);
				unsigned int mWidth = 0u;
				unsigned int mHeight = 0u;
				
				// a handle-based vector to store child containers
				hvector<LayoutContainer> mContainers;
				
				// a handle-based vector to store child divisions
				hvector<LayoutDivision> mDivisions;
				
				// the type of horizontal align (left, centre and right)
				std::string mHorizontalAlign = "left";
				
				// the type of veritcal align (top, middle, bottom)
				std::string mVerticalAlign = "top";
				
				// the default and increase in container and division
				// hvectors capacity (useful to prevent reallocation and
				// pointer invalidation)
				unsigned int mContainerReserve = 1u;
				unsigned int mDivisionReserve = 1u;
		};
	
	public :
		LayoutDivision(const glm::uvec2& cells = glm::uvec2(0u, 0u),
				LayoutContainer* parentContainer = nullptr);
		LayoutDivision(const glm::uvec2& cells,
				LayoutDivision* parentDivision, const glm::uvec2& cell);
		
		LayoutDivision(const LayoutDivision& other) = delete;
		LayoutDivision(LayoutDivision&& other);
		
		LayoutDivision& operator=(LayoutDivision other);
		
		friend void swap(LayoutDivision& first, LayoutDivision& second) {
			using std::swap;
			
			swap(first.mCells, second.mCells);
			swap(first.mCellStore, second.mCellStore);
			
			swap(first.mParent, second.mParent);
			
			// ensure the parents of any nested containers and divisions
			// are properly updated
			first.UpdateParents(&first);
			second.UpdateParents(&second);
		}
		
		glm::uvec2 GetCells() const;
		
		glm::ivec2 GetPosition(const CoordinateSpace& coordinateSpace =
				CoordinateSpace::Local) const;
		unsigned int GetWidth() const;
		unsigned int GetHeight() const;
		
		// functions to interact with cells
			glm::ivec2 GetPosition(const glm::uvec2& cell,
					const CoordinateSpace& coordinateSpace =
					CoordinateSpace::Local) const;
			unsigned int GetWidth(const glm::uvec2& cell) const;
			unsigned int GetHeight(const glm::uvec2& cell) const;
			
			// add, remove or retrieve a child container
			DivisionHandle AddContainer(const glm::ivec2& position,
					const unsigned int& width, const unsigned int& height,
					const glm::uvec2& cell);
			void RemoveContainer(const DivisionHandle& handle,
					const glm::uvec2& cell);
			LayoutContainer& GetContainer(const DivisionHandle& handle,
					const glm::uvec2& cell);
			
			// add, remove or retrieve a child division
			ContainerHandle AddDivision(const glm::uvec2& cells,
					const glm::uvec2& cell);
			void RemoveDivision(const ContainerHandle& handle,
					const glm::uvec2& cell);
			LayoutDivision& GetDivision(const ContainerHandle& handle,
					const glm::uvec2& cell);
			
			std::string GetHorizontalAlign(const glm::uvec2& cell) const;
			void SetHorizontalAlign(const std::string& align,
					const glm::uvec2& cell);
			std::string GetVerticalAlign(const glm::uvec2& cell) const;
			void SetVerticalAlign(const std::string& align,
					const glm::uvec2& cell);
			
			// position a list of element pointers within a cell depending
			// on their size and 'float' flag
			void PositionElements(std::list<LayoutElement*> elements,
					const glm::uvec2& cell);
			
			unsigned int GetContainerReserve(const glm::uvec2& cell) const;
			void SetContainerReserve(const unsigned int& reserve,
					const glm::uvec2& cell);
			unsigned int GetDivisionReserve(const glm::uvec2& cell) const;
			void SetDivisionReserve(const unsigned int& reserve,
					const glm::uvec2& cell);
		//
	private :
		// divide the division into the required number of cells
		void CreateCells();
		
		// update the parents of all child containers and divisions
		void UpdateParents(LayoutDivision* parent);
	
	private :
		// the layout of the division (in terms of cells)
		glm::uvec2 mCells = glm::uvec2(1, 1);
		
		// the cells that compose the division
		std::vector<Cell> mCellStore;
		
		// a struct holding parent information (container or division)
		LayoutParent mParent;
	
	#ifdef _UAIR_DEBUG
	public :
		// if debug mode is active then allow retrieval of outlines
		// representing this division and all its children
		std::vector<Shape> GetShapes();
	#endif
};
}

#endif */
