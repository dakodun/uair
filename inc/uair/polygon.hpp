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

#ifndef UAIRPOLYGON_HPP
#define UAIRPOLYGON_HPP

#include <vector>

#include <glm/glm.hpp>

namespace uair {
// a polygon is a collection of points that form a polygon
class Polygon {
	public :
		std::vector<glm::vec2> GetPoints() const; // return the vector of relative points
		std::vector<glm::vec2> GetAbsolutePoints() const; // return a vector of absolute points
		void AddPoint(const glm::vec2 & point); // add a relative point to the polygon
		void AddPoints(const std::vector<glm::vec2> & points); // add a vector of relative points to the polygon
		void AddAbsolutePoint(const glm::vec2 & point); // add an absolute point to the polygon
		void AddAbsolutePoints(const std::vector<glm::vec2> & points); // add a vector of absolute points to the polygon
		void Clear(); // clear all points from the polygon and reset bounds
		
		glm::vec2 GetPosition() const; // return a copy of the polygon's position
		void SetPosition(const glm::vec2 & newPos); // set the position of the polygon
		
		// set the polygon to a pre-defined shape
		void MakeRectangle(const glm::vec2 & position, const glm::vec2 & size);
		void MakeCircle(const glm::vec2 & position, const float & radius, const unsigned int & numPoints);
		
		std::vector<glm::vec2> GetBounds() const; // return a copy of the bounds of the polygon
		void Transform(const glm::mat3 & transform); // transform the polygon by a transformation matrix
	private :
		std::vector<glm::vec2> mPoints; // the relative points that make up the polygon
		
		// the bounds of the shape (top-left and bottom-right)
		std::vector<glm::vec2> mBounds = {glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f)};
		
		glm::vec2 mPosition; // the position of the polygon (essentially it's first point)
};
}

#endif
