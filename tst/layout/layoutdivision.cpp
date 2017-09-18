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

/* #include "layoutdivision.hpp"

#include "layoutcontainer.hpp"
#include "layoutelement.hpp"

namespace uair {
LayoutDivision::Cell::Cell(const glm::ivec2& position,
		const unsigned int& width, const unsigned int& height) :
		mPosition(position), mWidth(width), mHeight(height) {
	
	
}

LayoutDivision::Cell::Cell(Cell&& other) : Cell() {
	swap(*this, other);
}

LayoutDivision::Cell& LayoutDivision::Cell::operator=(Cell other) {
	swap(*this, other);
	
	return *this;
}


LayoutDivision::LayoutDivision(const glm::uvec2& cells,
		LayoutContainer* parentContainer) : mCells(cells),
		mParent(parentContainer) {
	
	CreateCells();
}

LayoutDivision::LayoutDivision(const glm::uvec2& cells,
		LayoutDivision* parentDivision,
		const glm::uvec2& cell) : mCells(cells),
		mParent(parentDivision, cell) {
	
	CreateCells();
}

LayoutDivision::LayoutDivision(LayoutDivision&& other) : LayoutDivision() {
	swap(*this, other);
}

LayoutDivision& LayoutDivision::operator=(LayoutDivision other) {
	swap(*this, other);
	
	return *this;
}

glm::uvec2 LayoutDivision::GetCells() const {
	return mCells;
}

glm::ivec2 LayoutDivision::GetPosition(const CoordinateSpace&
		coordinateSpace) const {
	
	return mParent.GetPosition(coordinateSpace);
}

unsigned int LayoutDivision::GetWidth() const {
	return mParent.GetWidth();
}

unsigned int LayoutDivision::GetHeight() const {
	return mParent.GetHeight();
}

DivisionHandle LayoutDivision::AddContainer(const glm::ivec2& position,
		const unsigned int& width, const unsigned int& height,
		const glm::uvec2& cell) {
	
	// ensure the requested cell exists in the division's cell store
	if (cell.x >= mCells.x || cell.y >= mCells.y) {
		throw std::runtime_error("invalid cell requested");
	}
	
	// store a reference to the cell object
	Cell& cellRef = mCellStore.at(cell.x + (cell.y * mCells.x));
	
	// before adding a new container ensure enough capacity remains
	// otherwise increase the capacity by the minimum reserve amount
	if (cellRef.mContainers.count() == cellRef.mContainers.capacity()) {
		cellRef.mContainers.reserve(cellRef.mContainers.capacity() +
				cellRef.mContainerReserve);
	}
	
	return cellRef.mContainers.emplace(position, width, height, this, cell);
}

void LayoutDivision::RemoveContainer(const DivisionHandle& handle,
		const glm::uvec2& cell) {
	
	if (cell.x >= mCells.x || cell.y >= mCells.y) {
		throw std::runtime_error("invalid cell requested");
	}
	
	Cell& cellRef = mCellStore.at(cell.x + (cell.y * mCells.x));
	
	cellRef.mContainers.pop(handle);
}

LayoutContainer& LayoutDivision::GetContainer(const DivisionHandle& handle,
		const glm::uvec2& cell) {
	
	if (cell.x >= mCells.x || cell.y >= mCells.y) {
		throw std::runtime_error("invalid cell requested");
	}
	
	Cell& cellRef = mCellStore.at(cell.x + (cell.y * mCells.x));
	
	return cellRef.mContainers.at(handle);
}

ContainerHandle LayoutDivision::AddDivision(const glm::uvec2& cells,
		const glm::uvec2& cell) {
	
	if (cell.x >= mCells.x || cell.y >= mCells.y) {
		throw std::runtime_error("invalid cell requested");
	}
	
	Cell& cellRef = mCellStore.at(cell.x + (cell.y * mCells.x));
	
	if (cellRef.mDivisions.count() == cellRef.mDivisions.capacity()) {
		cellRef.mDivisions.reserve(cellRef.mDivisions.capacity() +
				cellRef.mDivisionReserve);
	}
	
	return cellRef.mDivisions.emplace(cells, this, cell);
}

void LayoutDivision::RemoveDivision(const ContainerHandle& handle,
		const glm::uvec2& cell) {
	
	if (cell.x >= mCells.x || cell.y >= mCells.y) {
		throw std::runtime_error("invalid cell requested");
	}
	
	Cell& cellRef = mCellStore.at(cell.x + (cell.y * mCells.x));
	
	cellRef.mDivisions.pop(handle);
}

LayoutDivision& LayoutDivision::GetDivision(const ContainerHandle& handle,
		const glm::uvec2& cell) {
	
	if (cell.x >= mCells.x || cell.y >= mCells.y) {
		throw std::runtime_error("invalid cell requested");
	}
	
	Cell& cellRef = mCellStore.at(cell.x + (cell.y * mCells.x));
	
	return cellRef.mDivisions.at(handle);
}

// 
glm::ivec2 LayoutDivision::GetPosition(const glm::uvec2& cell,
		const CoordinateSpace& coordinateSpace) const {
	
	if (cell.x >= mCells.x || cell.y >= mCells.y) {
		throw std::runtime_error("invalid cell requested");
	}
	
	const Cell& cellRef = mCellStore.at(cell.x + (cell.y * mCells.x));
	if (coordinateSpace == CoordinateSpace::Global) { // if we seek the
		// global position...
		
		// add the position of the division to the cell's position
		return GetPosition(coordinateSpace) + cellRef.mPosition;
	}
	else {
		// otherwise return only the cell's position
		return cellRef.mPosition;
	}
}

unsigned int LayoutDivision::GetWidth(const glm::uvec2& cell) const {
	if (cell.x >= mCells.x || cell.y >= mCells.y) {
		throw std::runtime_error("invalid cell requested");
	}
	
	const Cell& cellRef = mCellStore.at(cell.x + (cell.y * mCells.x));
	return cellRef.mWidth;
}

unsigned int LayoutDivision::GetHeight(const glm::uvec2& cell) const {
	if (cell.x >= mCells.x || cell.y >= mCells.y) {
		throw std::runtime_error("invalid cell requested");
	}
	
	const Cell& cellRef = mCellStore.at(cell.x + (cell.y * mCells.x));
	return cellRef.mHeight;
}

std::string LayoutDivision::GetHorizontalAlign(const glm::uvec2& cell) const {
	if (cell.x >= mCells.x || cell.y >= mCells.y) {
		throw std::runtime_error("invalid cell requested");
	}
	
	const Cell& cellRef = mCellStore.at(cell.x + (cell.y * mCells.x));
	return cellRef.mHorizontalAlign;
}

void LayoutDivision::SetHorizontalAlign(const std::string& align,
		const glm::uvec2& cell) {
	
	if (cell.x >= mCells.x || cell.y >= mCells.y) {
		throw std::runtime_error("invalid cell requested");
	}
	
	Cell& cellRef = mCellStore.at(cell.x + (cell.y * mCells.x));
	cellRef.mHorizontalAlign = align;
}

std::string LayoutDivision::GetVerticalAlign(const glm::uvec2& cell) const {
	if (cell.x >= mCells.x || cell.y >= mCells.y) {
		throw std::runtime_error("invalid cell requested");
	}
	
	const Cell& cellRef = mCellStore.at(cell.x + (cell.y * mCells.x));
	return cellRef.mVerticalAlign;
}

void LayoutDivision::SetVerticalAlign(const std::string& align,
		const glm::uvec2& cell) {
	
	if (cell.x >= mCells.x || cell.y >= mCells.y) {
		throw std::runtime_error("invalid cell requested");
	}
	
	Cell& cellRef = mCellStore.at(cell.x + (cell.y * mCells.x));
	cellRef.mVerticalAlign = align;
}

void LayoutDivision::PositionElements(std::list<LayoutElement*> elements,
		const glm::uvec2& cell) {
	
	// store the position of the cell
	glm::ivec2 position = GetPosition(cell, CoordinateSpace::Global);
	
	// the width and height of the entire 'block' of elements
	glm::uvec2 total(0u, 0u);
	
	// the width and height of the current 'line' of elements
	glm::uvec2 line(0u, 0u);
	
	// a list of elements that constitute a single 'line'
	std::list< std::pair<unsigned int, std::list<LayoutElement*> > >
			heightElementPairs;
	
	// store some values associated with cell dimensions
	unsigned int width = GetWidth(cell);
	unsigned int height = GetHeight(cell);
	unsigned int halfWidth = width / 2u;
	unsigned int halfHeight = height / 2u;
	
	// store the alignment values of the cell
	std::string halign = GetHorizontalAlign(cell);
	std::string valign = GetVerticalAlign(cell);
	
	heightElementPairs.emplace_back();
	for (auto& ele: elements) { // for all elements to be positioned...
		if (ele->mFloat) { // if the element is to float...
			// set the position directly (ignore other elements)
			ele->mPosition = position;
			
			// adjust x-position of the element based on the cell's
			// horizontal align value
			if (halign == "centre") {
				ele->mPosition.x += halfWidth -
						(ele->GetWidth() / 2u);
			}
			else if (halign == "right") {
				ele->mPosition.x += width -
						ele->GetWidth();
			}
			
			// adjust y-position of the element based on the cell's
			// vertical align value
			if (valign == "middle") {
				ele->mPosition.y += halfHeight -
						(ele->GetHeight() / 2u);
			}
			else if (valign == "bottom") {
				ele->mPosition.y += height -
						ele->GetHeight();
			}
		}
		else {
			if (line.x + ele->GetWidth() <= width &&
					total.y + ele->GetHeight() <= height) { 
				// if the element fits into the current line...
				
				// position the current element using the current offset
				ele->mPosition = glm::ivec2(position.x + line.x,
						position.y + total.y);
				heightElementPairs.back().second.push_back(ele);
				
				// update the current line width and height
				line.x += ele->GetWidth();
				line.y = std::max(ele->GetHeight(), line.y);
			}
			else if (ele->GetWidth() <= width &&
					(line.y + total.y) + ele->GetHeight() <= height) {
				// otherwise if the element fits into the next line...
				
				// update the line height and add a new entry
				heightElementPairs.back().first = line.y;
				heightElementPairs.emplace_back();
				
				// update the block size and then move onto the next line
				total.x = std::max(line.x, total.x);
				total.y += line.y;
				line.x = 0u;
				line.y = 0u;
				
				ele->mPosition = glm::ivec2(position.x + line.x,
						position.y + total.y);
				heightElementPairs.back().second.push_back(ele);
				
				line.x += ele->GetWidth();
				line.y = std::max(ele->GetHeight(), line.y);
			}
		}
	}
	
	// update the last line height and block size
	heightElementPairs.back().first = line.y;
	total.x = std::max(line.x, total.x);
	total.y += line.y;
	
	// store some values base on remaining space in the cell
	unsigned int widthGap = width - total.x;
	unsigned int heightGap = height - total.y;
	unsigned int halfWidthGap = widthGap / 2u;
	unsigned int halfHeightGap = heightGap / 2u;
	
	for (auto& pair: heightElementPairs) { // for all element 'lines'...
		for (auto& ele: pair.second) { // for all element's in
			// current 'line'...
			
			if (halign == "centre") {
				ele->mPosition.x += halfWidthGap;
			}
			else if (halign == "right") {
				ele->mPosition.x += widthGap;
			}
			
			if (valign == "middle") {
				ele->mPosition.y += halfHeightGap;
			}
			else if (valign == "bottom") {
				ele->mPosition.y += heightGap;
			}
		}
	}
}
//

unsigned int LayoutDivision::GetContainerReserve(const glm::uvec2& cell)
		const {
	
	if (cell.x >= mCells.x || cell.y >= mCells.y) {
		throw std::runtime_error("invalid cell requested");
	}
	
	const Cell& cellRef = mCellStore.at(cell.x + (cell.y * mCells.x));
	
	return cellRef.mContainerReserve;
}

void LayoutDivision::SetContainerReserve(const unsigned int& reserve,
		const glm::uvec2& cell) {
	
	if (cell.x >= mCells.x || cell.y >= mCells.y) {
		throw std::runtime_error("invalid cell requested");
	}
	
	Cell& cellRef = mCellStore.at(cell.x + (cell.y * mCells.x));
	
	// update the container minimum reserve amount and then reserve more
	// space if the container hvector currently has less capacity
	// than the new value
	cellRef.mContainerReserve = reserve;
	if (cellRef.mContainers.capacity() < cellRef.mContainerReserve) {
		cellRef.mContainers.reserve(cellRef.mContainerReserve);
	}
}

unsigned int LayoutDivision::GetDivisionReserve(const glm::uvec2& cell)
		const {
	
	if (cell.x >= mCells.x || cell.y >= mCells.y) {
		throw std::runtime_error("invalid cell requested");
	}
	
	const Cell& cellRef = mCellStore.at(cell.x + (cell.y * mCells.x));
	
	return cellRef.mDivisionReserve;
}

void LayoutDivision::SetDivisionReserve(const unsigned int& reserve,
		const glm::uvec2& cell) {
	
	if (cell.x >= mCells.x || cell.y >= mCells.y) {
		throw std::runtime_error("invalid cell requested");
	}
	
	Cell& cellRef = mCellStore.at(cell.x + (cell.y * mCells.x));
	
	cellRef.mDivisionReserve = reserve;
	if (cellRef.mDivisions.capacity() < cellRef.mDivisionReserve) {
		cellRef.mDivisions.reserve(cellRef.mDivisionReserve);
	}
}

void LayoutDivision::CreateCells() {
	std::vector<Cell> cells;
	
	// get the dimensions of an individual cell
	float widthInc = GetWidth() / mCells.x;
	float heightInc = GetHeight() / mCells.y;
	
	for (unsigned int y = 0u; y < mCells.y; ++y) { // for each row...
		for (unsigned int x = 0u; x < mCells.x; ++x) { // for each column...
			// create a new cell and add it to the store
			cells.emplace_back(glm::vec2(widthInc * x, heightInc * y),
					widthInc, heightInc);
		}
	}
	
	// replace the existing cell store (if any) with the updated one
	mCellStore = std::move(cells);
	
	for (auto& cell: mCellStore) { // for all cells in the new store...
		for (auto& division: cell.mDivisions) { // for all child divisions
			// in current cell...
			
			// ensure child division also updates cells
			division.CreateCells();
		}
	}
}

void LayoutDivision::UpdateParents(LayoutDivision* parent) {
	for (auto& cell: mCellStore) { // for all cells...
		for (auto& container: cell.mContainers) { // for all child
			// containers...
			
			// update child's parent to point to this
			container.mParent.SetParent(parent);
			
			// recursively call UpdateParents function to update child's
			// children, passing in child as the parent
			container.UpdateParents(&container);
		}
		
		for (auto& division: cell.mDivisions) { // for all child
			// divisions...
			
			division.mParent.SetParent(parent);
			division.UpdateParents(&division);
		}
	}
}

#ifdef _UAIR_DEBUG
std::vector<Shape> LayoutDivision::GetShapes() {
	std::vector<Shape> shapes;
	
	{ // create the outline for this container
		Shape shape;
		
		float width = GetWidth();
		float height = GetHeight();
		
		float widthInc = width / mCells.x;
		float heightInc = height / mCells.y;
		
		for (unsigned int x = 1u; x < mCells.x; ++x) {
			shape.AddContour(Contour({glm::vec2(widthInc * x, 0.0f),
					glm::vec2(widthInc * x, height)}));
		}
		
		for (unsigned int y = 1u; y < mCells.y; ++y) {
			shape.AddContour(Contour({glm::vec2(0.0f, heightInc * y),
					glm::vec2(width, heightInc * y)}));
		}
		
		shape.SetPosition(GetPosition(CoordinateSpace::Global));
		shape.mRenderMode = GL_LINE_LOOP;
		shape.SetColour(glm::vec3(0.0f, 1.0f, 0.0f));
		shape.SetDepth(-1.0f);
		
		shapes.push_back(std::move(shape));
	}
	
	for (auto& cell: mCellStore) { // for all cells...
		for (auto& container: cell.mContainers) { // for all child
			// containers...
			
			// recursively call GetShapes function
			std::vector<Shape> newShapes = container.GetShapes();
			shapes.insert(shapes.end(),
					newShapes.begin(), newShapes.end());
		}
		
		for (auto& division: cell.mDivisions) { // for all child
			// divisions...
			
			std::vector<Shape> newShapes = division.GetShapes();
			shapes.insert(shapes.end(),
					newShapes.begin(), newShapes.end());
		}
	}
	
	return shapes;
}
#endif
} */
