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

/* #include "layoutcontainer.hpp"

#include "layoutdivision.hpp"
#include "layoutelement.hpp"

namespace uair {
LayoutContainer::LayoutContainer(const glm::ivec2& position,
		const unsigned int& width, const unsigned int& height) :
		mPosition(position), mWidth(width), mHeight(height) {
	
	
}

LayoutContainer::LayoutContainer(const glm::ivec2& position,
		const unsigned int& width, const unsigned int& height,
		LayoutContainer* parentContainer) : mPosition(position),
		mWidth(width), mHeight(height), mParent(parentContainer) {
	
	
}

LayoutContainer::LayoutContainer(const glm::ivec2& position,
		const unsigned int& width, const unsigned int& height,
		LayoutDivision* parentDivision, const glm::uvec2& cell) :
		mPosition(position), mWidth(width), mHeight(height),
		mParent(parentDivision, cell) {
	
	
}

LayoutContainer::LayoutContainer(LayoutContainer&& other) : LayoutContainer() {
	swap(*this, other);
}

LayoutContainer& LayoutContainer::operator=(LayoutContainer other) {
	swap(*this, other);
	
	return *this;
}

void LayoutContainer::FromProperties(const Properties& properties) {
	mPosition = properties.mPosition;
	mWidth = properties.mWidth;
	mHeight = properties.mHeight;
}

glm::ivec2 LayoutContainer::GetPosition(const CoordinateSpace&
		coordinateSpace) const {
	
	// if we want to get the absolute position...
	if (coordinateSpace == CoordinateSpace::Global) {
		// add the position of the parent to this
		return mParent.GetPosition(coordinateSpace) + mPosition;
	}
	else { // otherwise return the relative position
		return mPosition;
	}
}

void LayoutContainer::SetPosition(const glm::ivec2& position) {
	mPosition = position;
}

unsigned int LayoutContainer::GetWidth() const {
	return mWidth;
}

void LayoutContainer::SetWidth(const unsigned int& width) {
	mWidth = width;
	
	// make sure to update all divisions (cells will have new
	// positions and dimensions)
	for (auto iter = mDivisions.begin(); iter != mDivisions.end(); ++iter) {
		iter->CreateCells();
	}
}

unsigned int LayoutContainer::GetHeight() const {
	return mHeight;
}

void LayoutContainer::SetHeight(const unsigned int& height) {
	mHeight = height;
	
	for (auto iter = mDivisions.begin(); iter != mDivisions.end(); ++iter) {
		iter->CreateCells();
	}
}

ContainerHandle LayoutContainer::AddContainer(const glm::ivec2& position,
		const unsigned int& width, const unsigned int& height) {
	
	// before adding a new container ensure enough capacity remains
	// otherwise increase the capacity by the minimum reserve amount
	if (mContainers.count() == mContainers.capacity()) {
		mContainers.reserve(mContainers.capacity() + mContainerReserve);
	}
	
	return mContainers.emplace(position, width, height, this);
}

void LayoutContainer::RemoveContainer(const ContainerHandle& handle) {
	mContainers.pop(handle);
}

LayoutContainer& LayoutContainer::GetContainer(const ContainerHandle&
		handle) {
	
	return mContainers.at(handle);
}

DivisionHandle LayoutContainer::AddDivision(const glm::uvec2& cells) {
	if (mDivisions.count() == mDivisions.capacity()) {
		mDivisions.reserve(mDivisions.capacity() + mDivisionReserve);
	}
	
	return mDivisions.emplace(cells, this);
}

void LayoutContainer::RemoveDivision(const DivisionHandle& handle) {
	mDivisions.pop(handle);
}

LayoutDivision& LayoutContainer::GetDivision(const DivisionHandle& handle) {
	return mDivisions.at(handle);
}

std::string LayoutContainer::GetHorizontalAlign() const {
	return mHorizontalAlign;
}

void LayoutContainer::SetHorizontalAlign(const std::string& align) {
	mHorizontalAlign = align;
}

std::string LayoutContainer::GetVerticalAlign() const {
	return mVerticalAlign;
}

void LayoutContainer::SetVerticalAlign(const std::string& align) {
	mVerticalAlign = align;
}

void LayoutContainer::PositionElements(std::list<LayoutElement*> elements) {
	// store the position of the container
	glm::ivec2 position = GetPosition(CoordinateSpace::Global);
	
	// the width and height of the entire 'block' of elements
	glm::uvec2 total(0u, 0u);
	
	// the width and height of the current 'line' of elements
	glm::uvec2 line(0u, 0u);
	
	// a list of elements that constitute a single 'line'
	std::list< std::pair<unsigned int, std::list<LayoutElement*> > >
			heightElementPairs;
	
	// store some values associated with container dimensions
	unsigned int width = GetWidth();
	unsigned int height = GetHeight();
	unsigned int halfWidth = width / 2u;
	unsigned int halfHeight = height / 2u;
	
	heightElementPairs.emplace_back();
	for (auto& ele: elements) { // for all elements to be positioned...
		if (ele->mFloat) { // if the element is to float...
			// set the position directly (ignore other elements)
			ele->mPosition = position;
			
			// adjust x-position of the element based on the container's
			// horizontal align value
			if (mHorizontalAlign == "centre") {
				ele->mPosition.x += halfWidth -
						(ele->GetWidth() / 2u);
			}
			else if (mHorizontalAlign == "right") {
				ele->mPosition.x += width -
						ele->GetWidth();
			}
			
			// adjust y-position of the element based on the container's
			// vertical align value
			if (mVerticalAlign == "middle") {
				ele->mPosition.y += halfHeight -
						(ele->GetHeight() / 2u);
			}
			else if (mVerticalAlign == "bottom") {
				ele->mPosition.y += height -
						ele->GetHeight();
			}
		}
		else {
			if (line.x + ele->GetWidth() <= width &&
					total.y + ele->GetHeight() <= height) { // if the
				// element fits into the current line...
				
				// position the current element using the current offset
				ele->mPosition = glm::ivec2(position.x + line.x,
						position.y + total.y);
				heightElementPairs.back().second.push_back(ele);
				
				// update the current line width and height
				line.x += ele->GetWidth();
				line.y = std::max(ele->GetHeight(), line.y);
			}
			else if (ele->GetWidth() <= width &&
					(line.y + total.y) + ele->GetHeight()<= height) {
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
	
	// store some values base on remaining space in the container
	unsigned int widthGap = width - total.x;
	unsigned int heightGap = height - total.y;
	unsigned int halfWidthGap = widthGap / 2u;
	unsigned int halfHeightGap = heightGap / 2u;
	
	for (auto& pair: heightElementPairs) { // for all element 'lines'...
		for (auto& ele: pair.second) { // for all element's in
			// current 'line'...
			
			if (mHorizontalAlign == "centre") {
				ele->mPosition.x += halfWidthGap;
			}
			else if (mHorizontalAlign == "right") {
				ele->mPosition.x += widthGap;
			}
			
			if (mVerticalAlign == "middle") {
				ele->mPosition.y += halfHeightGap;
			}
			else if (mVerticalAlign == "bottom") {
				ele->mPosition.y += heightGap;
			}
		}
	}
}

unsigned int LayoutContainer::GetContainerReserve() const {
	return mContainerReserve;
}

void LayoutContainer::SetContainerReserve(const unsigned int& reserve) {
	// update the container minimum reserve amount and then reserve more
	// space if the container hvector currently has less capacity
	// than the new value
	mContainerReserve = reserve;
	if (mContainers.capacity() < mContainerReserve) {
		mContainers.reserve(mContainerReserve);
	}
}

unsigned int LayoutContainer::GetDivisionReserve() const {
	return mDivisionReserve;
}

void LayoutContainer::SetDivisionReserve(const unsigned int& reserve) {
	mDivisionReserve = reserve;
	if (mDivisions.capacity() < mDivisionReserve) {
		mDivisions.reserve(mDivisionReserve);
	}
}

void LayoutContainer::UpdateParents(LayoutContainer* parent) {
	for (auto& container: mContainers) { // for all child containers...
		// update child's parent to point to this
		container.mParent.SetParent(parent);
		
		// recursively call UpdateParents function to update child's
		// children, passing in child as the parent
		container.UpdateParents(&container);
	}
	
	for (auto& division: mDivisions) { // for all child divisions...
		division.mParent.SetParent(parent);
		division.UpdateParents(&division);
	}
}

#ifdef _UAIR_DEBUG
std::vector<Shape> LayoutContainer::GetShapes() {
	std::vector<Shape> shapes;
	
	{ // create the outline for this container
		Shape shape;
		shape.AddContour(Contour({glm::vec2(0.0f, 0.0f),
				glm::vec2(mWidth, 0.0f),
				glm::vec2(mWidth, mHeight),
				glm::vec2(0.0f, mHeight)}));
		shape.SetPosition(GetPosition(CoordinateSpace::Global));
		shape.SetDepth(-1.0f);
		shape.mRenderMode = GL_LINE_LOOP;
		
		// set colour depending on whether this is the root container or not
		if (mParent) {
			shape.SetColour(glm::vec3(1.0f, 0.0f, 1.0f));
		}
		else {
			shape.SetColour(glm::vec3(1.0f, 0.0f, 0.0f));
		}
		
		shapes.push_back(std::move(shape));
	}
	
	for (auto& container: mContainers) { // for all child containers...
		// recursively call GetShapes function
		std::vector<Shape> newShapes = container.GetShapes();
		shapes.insert(shapes.end(),
				newShapes.begin(), newShapes.end());
	}
	
	for (auto& division: mDivisions) { // for all child divisions...
		std::vector<Shape> newShapes = division.GetShapes();
		shapes.insert(shapes.end(),
				newShapes.begin(), newShapes.end());
	}
	
	return shapes;
}
#endif
} */
