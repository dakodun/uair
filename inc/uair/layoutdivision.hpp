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

#ifndef UAIRLAYOUTDIVISION_HPP
#define UAIRLAYOUTDIVISION_HPP

#include <tuple>
#include <vector>

#include <glm/glm.hpp>

#include "init.hpp"
#include "layoutbase.hpp"
#include "layoutcontainer.hpp"
#include "manager.hpp"
#include "shape.hpp"

namespace uair {
class LayoutElement;

class EXPORTDLL LayoutDivision : public LayoutBase {
	public :
		friend class LayoutContainer;
		typedef std::tuple<unsigned int, unsigned int, unsigned int> Handle;
		
		struct Cell {
			public :
				Cell(const glm::vec2& position, const float& width, const float& height,
						LayoutDivision* parentDivision = nullptr);
				
				glm::vec2 GetPosition(const CoordinateSpace& coordinateSpace = CoordinateSpace::Local) const;
				float GetWidth() const;
				float GetHeight() const;
			private :
				glm::vec2 mPosition = glm::vec2(0.0f);
				float mWidth = 0.0f;
				float mHeight = 0.0f;
				
				LayoutDivision* mParentDivision = nullptr;
		};
		
		LayoutDivision(const unsigned int& rows, const unsigned int& columns, LayoutContainer* parentContainer);
		LayoutDivision(const unsigned int& rows, const unsigned int& columns, LayoutDivision* parentDivision,
				const unsigned int& row, const unsigned int& column);
		
		unsigned int GetRows() const;
		unsigned int GetColumns() const;
		Cell GetCell(const unsigned int& row, const unsigned int& column) const;
		
		glm::vec2 GetPosition(const CoordinateSpace& coordinateSpace = CoordinateSpace::Local) const;
		float GetWidth() const;
		float GetHeight() const;
		
		Handle AddContainer(const glm::vec2& position, const float& width, const float& height,
				const unsigned int& row, const unsigned int& column);
		void RemoveContainer(const Handle& handle);
		LayoutContainer& GetContainer(const Handle& handle);
		
		Handle AddDivision(const unsigned int& rows, const unsigned int& columns, const unsigned int& row, const unsigned int& column);
		void RemoveDivision(const Handle& handle);
		LayoutDivision& GetDivision(const Handle& handle);
		
		void AssignElement(LayoutElement* element, const unsigned int& row, const unsigned int& column);
		
		std::vector<Shape> GetShapes();
	private :
		void CreateCells();
	
	private :
		unsigned int mRows = 1u;
		unsigned int mColumns = 1u;
		std::vector<Cell> mCells;
		
		Store<LayoutBase, LayoutContainer> mContainers;
		Store<LayoutBase, LayoutDivision> mDivisions;
		
		LayoutContainer* mParentContainer = nullptr;
		LayoutDivision* mParentDivision = nullptr;
		unsigned int mRow = 0u;
		unsigned int mColumn = 0u;
};
}

#endif
