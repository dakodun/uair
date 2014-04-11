/* **************************************************************** **
**	Uair Engine
**	Copyright (c) 2014 Iain M. Crawford
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

#ifndef UAIRSHAPE_HPP
#define UAIRSHAPE_HPP

#include <vector>

#include "renderable.hpp"

namespace uair {
// a renderable shape
class Shape : public Renderable {
	friend class RenderBatch; // allow a render batch to access this class
	
	public :
		std::string GetTag() const; // return the tag (type) of this renderable
		
		std::vector<glm::vec2> GetPoints() const; // return the vector of relative points
		std::vector<glm::vec2> GetAbsolutePoints() const; // return a vector of absolute points
		void AddPoint(const glm::vec2 & point); // add a relative point to the shape
		void AddPoints(const std::vector<glm::vec2> & points); // add a vector of relative points to the shape
		void AddAbsolutePoint(const glm::vec2 & point); // add an absolute point to the shape
		void AddAbsolutePoints(const std::vector<glm::vec2> & points); // add a vector of absolute points to the shape
		void Clear(); // clear all points from the shape and reset bounds and bounding boxes
		
		// set the shape to a pre-defined shape
		void MakeRectangle(const glm::vec2 & position, const glm::vec2 & size);
		void MakeCircle(const glm::vec2 & position, const float & radius, const unsigned int & numPoints);
	protected :
		void UpdateGlobalBoundingBox(); // update the shape's global bounding box (according to it's transform)
		void UpdateGlobalMask(); // update the shape's global mask (according to it's transform)
		void CreateLocalMask(); // create a default local mask for the shape
		RenderBatchData Upload() const; // upload the shape to a render batch
	private :
		std::vector<glm::vec2> mPoints; // the relative points that make up the shape
		
		// the bounds of the shape (top-left and bottom-right)
		std::vector<glm::vec2> mBounds = {glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f)};
};
}

#endif
