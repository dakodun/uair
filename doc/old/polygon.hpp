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
#include <string>

#include <glm/glm.hpp>
#include <clipper.hpp>

#include "transformable.hpp"

namespace uair {
class Shape;

namespace CoordinateSpace {
	enum {
		Local = 0u,
		Global
	};
}

// bezier related code based on the excellent "A Primer on Bézier Curves" (http://pomax.github.io/bezierinfo/#decasteljau) by Mike "Pomax" Kamermans
// additional code from Bartosz Ciechanowski (http://ciechanowski.me/blog/page/2/)
class Polygon : public Transformable {
	public :
		Polygon() = default;
		explicit Polygon(const ClipperLib::Paths& paths);
		
		std::vector<glm::vec2> GetPoints(const unsigned int& coordinateSpace = CoordinateSpace::Local) const;
		std::vector< std::vector<glm::vec2> > GetInnerBoundaries(const unsigned int& coordinateSpace = CoordinateSpace::Local) const;
		void AddPoint(const glm::vec2& point, const unsigned int& coordinateSpace = CoordinateSpace::Local);
		void AddPoints(const std::vector<glm::vec2>& points, const unsigned int& coordinateSpace = CoordinateSpace::Local);
		void AddBezier(const std::vector<glm::vec2>& controlPoints, const unsigned int& coordinateSpace = CoordinateSpace::Local);
		void AddInnerBoundary(const std::vector<glm::vec2>& innerBoundary, const unsigned int& coordinateSpace = CoordinateSpace::Local);
		
		void Clear();
		
		void MakeRectangle(const float& width, const float& height, const glm::vec2& offset = glm::vec2(0.0f, 0.0f));
		void MakeCircle(const float& radius, const unsigned int& numPoints, const glm::vec2& offset = glm::vec2(0.0f, 0.0f));
		
		void FromSVGPath(const std::string& svgPath, const unsigned int& smoothness = 18u);
		
		operator Shape() const;
		operator ClipperLib::Paths() const;
		void FromClipperPaths(const ClipperLib::Paths& paths);
		
		bool FixWinding();
		Polygon Offset(const float& distance);
	protected :
		void UpdateGlobalBoundingBox();
		void UpdateGlobalMask();
		void CreateLocalMask();
		
		void UpdateBounds(const std::vector<glm::vec2>& points);
		void CreateBezier(const std::vector<glm::vec2>& controlPoints);
	protected :
		std::vector<glm::vec2> mPoints;
		std::vector< std::vector<glm::vec2> > mInnerBoundaries;
		std::vector<glm::vec2> mBounds;
};
}

#endif
