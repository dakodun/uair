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

#include "layoutcontainer.hpp"

#include "layoutdivision.hpp"
#include "layoutelement.hpp"

namespace uair {
LayoutContainer::LayoutContainer(const glm::vec2& position, const float& width, const float& height) :
		mPosition(position), mWidth(width), mHeight(height) {
	
	
}

LayoutContainer::LayoutContainer(const glm::vec2& position, const float& width, const float& height,
		LayoutContainer* parentContainer) : mPosition(position), mWidth(width), mHeight(height),
		mParentContainer(parentContainer) {
	
}

LayoutContainer::LayoutContainer(const glm::vec2& position, const float& width, const float& height,
		LayoutDivision* parentDivision, const unsigned int& row, const unsigned int& column) : mPosition(position),
		mWidth(width), mHeight(height), mParentDivision(parentDivision), mRow(row), mColumn(column) {
	
}

void LayoutContainer::SetPosition(const glm::vec2& position, const CoordinateSpace& coordinateSpace) {
	if (coordinateSpace == CoordinateSpace::Global) { // if we want to set the absolute position...
		if (mParentContainer) { // if we have a parent container...
			mPosition = position - mParentContainer->GetPosition(coordinateSpace); // set the position as absolute
		}
		else if (mParentDivision) { // otherwise if we have a parent division...
			// set the position as absolute
			mPosition = position - mParentDivision->GetCell(mRow, mColumn).GetPosition(coordinateSpace);
		}
	}
	
	mPosition = position; // set the position as relative
}

void LayoutContainer::SetWidth(const float& width) {
	mWidth = width;
	
	auto divisions = mDivisions.Get();
	for (auto iter = divisions.begin(); iter != divisions.end(); ++iter) {
		iter->get().CreateCells();
	}
}

void LayoutContainer::SetHeight(const float& height) {
	mHeight = height;
	
	auto divisions = mDivisions.Get();
	for (auto iter = divisions.begin(); iter != divisions.end(); ++iter) {
		iter->get().CreateCells();
	}
}

glm::vec2 LayoutContainer::GetPosition(const CoordinateSpace& coordinateSpace) const {
	if (coordinateSpace == CoordinateSpace::Global) { // if we want to get the absolute position...
		if (mParentContainer) { // if we have a parent container...
			// recursively call getposition() functions until we reach the top of the parent-child tree and return the absolute position
			return mParentContainer->GetPosition(coordinateSpace) + mPosition;
		}
		else if (mParentDivision) { // otherwise if we have a parent division...
			// recursively call getposition() functions until we reach the top of the parent-child tree and return the absolute position
			return mParentDivision->GetCell(mRow, mColumn).GetPosition(coordinateSpace) + mPosition;
		}
	}
	
	return mPosition; // return the relative position
}

float LayoutContainer::GetWidth() const {
	return mWidth;
}

float LayoutContainer::GetHeight() const {
	return mHeight;
}

LayoutContainer::Handle LayoutContainer::AddContainer(const glm::vec2& position, const float& width,
		const float& height) {
	
	try {
		// add a new container to the store and return the index and counter value as a handle
		std::pair<unsigned int, unsigned int> indexCounterPair = mContainers.Add<glm::vec2, float, float,
				LayoutContainer*>("", position, width, height, this);
		
		// create a tuple (handle) with index, counter and id (denoting container)
		return std::make_tuple(indexCounterPair.first, indexCounterPair.second, 0u);
	} catch (std::exception& e) {
		throw;
	}
}

void LayoutContainer::RemoveContainer(const Handle& handle) {
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

LayoutContainer& LayoutContainer::GetContainer(const Handle& handle) {
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

LayoutContainer::Handle LayoutContainer::AddDivision(const unsigned int& rows, const unsigned int& columns) {
	try {
		// add a new division to the store and return the index and counter value as a handle
		std::pair<unsigned int, unsigned int> indexCounterPair = mDivisions.Add<unsigned int, unsigned int,
				LayoutContainer*>("", rows, columns, this);
		
		// create a tuple (handle) with index, counter and id (denoting division)
		return std::make_tuple(indexCounterPair.first, indexCounterPair.second, 1u);
	} catch (std::exception& e) {
		throw;
	}
}

void LayoutContainer::RemoveDivision(const Handle& handle) {
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

LayoutDivision& LayoutContainer::GetDivision(const Handle& handle) {
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

std::vector<Shape> LayoutContainer::GetShapes() {
	std::vector<Shape> shapes;
	
	{
		Shape shape;
		shape.AddContour(Contour({glm::vec2(0.0f, 0.0f), glm::vec2(mWidth, 0.0f), glm::vec2(mWidth, mHeight), glm::vec2(0.0f, mHeight)}));
		shape.SetPosition(GetPosition(CoordinateSpace::Global));
		shape.mRenderMode = GL_LINE_LOOP;
		
		if (mParentContainer || mParentDivision) {
			shape.SetColour(glm::vec3(1.0f, 0.0f, 1.0f));
		}
		else {
			shape.SetColour(glm::vec3(1.0f, 0.0f, 0.0f));
		}
		
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

void LayoutContainer::AssignElement(LayoutElement* element) {
	element->mLayoutContainer.Unset();
	element->mLayoutDivision.Unset();
	element->mRow = 0u;
	element->mColumn = 0u;
	
	element->mLayoutContainer.Set(this);
}
}
