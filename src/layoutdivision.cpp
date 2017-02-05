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

#include "layoutdivision.hpp"

#include "layoutelement.hpp"

namespace uair {
LayoutDivision::Cell::Cell(const glm::vec2& position, const float& width, const float& height,
		LayoutDivision* parentDivision) : mPosition(position), mWidth(width), mHeight(height),
		mParentDivision(parentDivision) {
	
}

glm::vec2 LayoutDivision::Cell::GetPosition(const CoordinateSpace& coordinateSpace) const {
	if (coordinateSpace == CoordinateSpace::Global) { // if we want to get the absolute position...
		if (mParentDivision) { // if we have a valid parent division...
			// recursively call getposition() functions until we reach the top of the parent-child tree and return the absolute position
			return mParentDivision->GetPosition(coordinateSpace) + mPosition;
		}
		
		throw std::runtime_error("");
	}
	
	return mPosition; // return the relative position
}

float LayoutDivision::Cell::GetWidth() const {
	return mWidth;
}

float LayoutDivision::Cell::GetHeight() const {
	return mHeight;
}

LayoutDivision::LayoutDivision(const unsigned int& rows, const unsigned int& columns, LayoutContainer* parentContainer) :
		mRows(rows), mColumns(columns), mParentContainer(parentContainer) {
	
	CreateCells();
}

LayoutDivision::LayoutDivision(const unsigned int& rows, const unsigned int& columns, LayoutDivision* parentDivision,
		const unsigned int& row, const unsigned int& column) : mRows(rows), mColumns(columns),
		mParentDivision(parentDivision), mRow(row), mColumn(column) {
	
	CreateCells();
}

unsigned int LayoutDivision::GetRows() const {
	return mRows;
}

unsigned int LayoutDivision::GetColumns() const {
	return mColumns;
}

LayoutDivision::Cell LayoutDivision::GetCell(const unsigned int& row, const unsigned int& column) const {
	if (row < mRows && column < mColumns) {
		return mCells.at(column + (row * mColumns));
	}
	
	throw std::runtime_error("");
}

glm::vec2 LayoutDivision::GetPosition(const CoordinateSpace& coordinateSpace) const {
	if (mParentContainer) { // if we have a parent container...
		// recursively call getposition() functions until we reach the top of the parent-child tree and return the absolute position
		return mParentContainer->GetPosition(coordinateSpace);
	}
	else if (mParentDivision) { // otherwise if we have a parent division...
		// recursively call getposition() functions until we reach the top of the parent-child tree and return the absolute position
		return mParentDivision->GetCell(mRow, mColumn).GetPosition(coordinateSpace);
	}
	
	throw std::runtime_error("");
}

float LayoutDivision::GetWidth() const {
	if (mParentContainer) { // if we have a parent container...
		return mParentContainer->GetWidth(); // return the width of the parent container
	}
	else if (mParentDivision) { // otherwise if we have a parent division...
		return mParentDivision->GetCell(mRow, mColumn).GetWidth(); // return the width of the parent cell
	}
	
	throw std::runtime_error("");
}

float LayoutDivision::GetHeight() const {
	if (mParentContainer) { // if we have a parent container...
		return mParentContainer->GetHeight(); // return the height of the parent container
	}
	else if (mParentDivision) { // otherwise if we have a parent division...
		return mParentDivision->GetCell(mRow, mColumn).GetHeight(); // return the height of the parent cell
	}
	
	throw std::runtime_error("");
}

LayoutDivision::Handle LayoutDivision::AddContainer(const glm::vec2& position, const float& width,
		const float& height, const unsigned int& row, const unsigned int& column) {
	
	try {
		// add a new container to the store and return the index and counter value as a handle
		std::pair<unsigned int, unsigned int> indexCounterPair = mContainers.Add<glm::vec2, float, float,
				LayoutDivision*, unsigned int, unsigned int>("", position, width, height, this, row, column);
		
		// create a tuple (handle) with index, counter and id (denoting container)
		return std::make_tuple(indexCounterPair.first, indexCounterPair.second, 0u);
	} catch (std::exception& e) {
		throw;
	}
}

void LayoutDivision::RemoveContainer(const Handle& handle) {
	if (std::get<2>(handle) != 0u) { // if the handle is not a valid container handle...
		throw std::runtime_error("");
	}
	
	try {
		// remove the container from the store using its index and counter (validity) value
		mContainers.Remove(std::get<0>(handle), std::get<1>(handle));
	} catch (std::exception& e) {
		throw;
	}
}

LayoutContainer& LayoutDivision::GetContainer(const Handle& handle) {
	if (std::get<2>(handle) != 0u) { // if the handle is not a valid container handle...
		throw std::runtime_error("");
	}
	
	try {
		// return a reference to the stored container using its index and counter (validity) value
		return mContainers.Get(std::get<0>(handle), std::get<1>(handle));
	} catch (std::exception& e) {
		throw;
	}
}

LayoutDivision::Handle LayoutDivision::AddDivision(const unsigned int& rows, const unsigned int& columns,
		const unsigned int& row, const unsigned int& column) {
		
	try {
		// add a new division to the store and return the index and counter value as a handle
		std::pair<unsigned int, unsigned int> indexCounterPair = mDivisions.Add<unsigned int, unsigned int,
				LayoutDivision*, unsigned int, unsigned int>("", rows, columns, this, row, column);
		
		// create a tuple (handle) with index, counter and id (denoting division)
		return std::make_tuple(indexCounterPair.first, indexCounterPair.second, 1u);
	} catch (std::exception& e) {
		throw;
	}
}

void LayoutDivision::RemoveDivision(const Handle& handle) {
	if (std::get<2>(handle) != 1u) { // if the handle is not a valid division handle...
		throw std::runtime_error("");
	}
	
	try {
		// remove the division from the store using its index and counter (validity) value
		mDivisions.Remove(std::get<0>(handle), std::get<1>(handle));
	} catch (std::exception& e) {
		throw;
	}
}

LayoutDivision& LayoutDivision::GetDivision(const Handle& handle) {
	if (std::get<2>(handle) != 1u) { // if the handle is not a valid division handle...
		throw std::runtime_error("");
	}
	
	try {
		// return a reference to the stored division using its index and counter (validity) value
		return mDivisions.Get(std::get<0>(handle), std::get<1>(handle));
	} catch (std::exception& e) {
		throw;
	}
}

std::vector<Shape> LayoutDivision::GetShapes() {
	std::vector<Shape> shapes;
	
	{
		Shape shape;
		
		float width = GetWidth();
		float height = GetHeight();
		
		float widthInc = width / mRows;
		float heightInc = height / mColumns;
		
		for (unsigned int x = 1u; x < mColumns; ++x) {
			shape.AddContour(Contour({glm::vec2(widthInc * x, 0.0f), glm::vec2(widthInc * x, height)}));
		}
		
		for (unsigned int y = 1u; y < mRows; ++y) {
			shape.AddContour(Contour({glm::vec2(0.0f, heightInc * y), glm::vec2(width, heightInc * y)}));
		}
		
		shape.SetPosition(GetPosition(CoordinateSpace::Global));
		shape.mRenderMode = GL_LINE_LOOP;
		shape.SetColour(glm::vec3(0.0f, 1.0f, 0.0f));
		
		shapes.push_back(std::move(shape));
	}
	
	auto containers = mContainers.Get();
	for (auto iter = containers.begin(); iter != containers.end(); ++iter) {
		std::vector<Shape> containerShapes = iter->get().GetShapes();
		shapes.insert(shapes.end(), containerShapes.begin(), containerShapes.end());
	}
	
	auto divisions = mDivisions.Get();
	for (auto iter = divisions.begin(); iter != divisions.end(); ++iter) {
		std::vector<Shape> divisionShapes = iter->get().GetShapes();
		shapes.insert(shapes.end(), divisionShapes.begin(), divisionShapes.end());
	}
	
	return shapes;
}

void LayoutDivision::AssignElement(LayoutElement* element, const unsigned int& row, const unsigned int& column) {
	element->mLayoutContainer.Unset();
	element->mLayoutDivision.Unset();
	element->mRow = 0u;
	element->mColumn = 0u;
	
	element->mLayoutDivision.Set(this);
	element->mRow = row;
	element->mColumn = column;
}

void LayoutDivision::CreateCells() {
	std::vector<Cell> cells;
	
	float widthInc = GetWidth() / mRows;
	float heightInc = GetHeight() / mColumns;
	
	for (unsigned int y = 0u; y < mRows; ++y) {
		for (unsigned int x = 0u; x < mColumns; ++x) {
			cells.emplace_back(glm::vec2(widthInc * x, heightInc * y), widthInc, heightInc, this);
		}
	}
	
	mCells = std::move(cells);
	
	auto divisions = mDivisions.Get();
	for (auto iter = divisions.begin(); iter != divisions.end(); ++iter) {
		iter->get().CreateCells();
	}
}
}
