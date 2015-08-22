/* **************************************************************** **
**	Uair Engine
**	Copyright (c) 20XX Iain M. Crawford
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

enum class CoordinateSpace {
	Local = 0u,
	Global
};

enum class Winding {
	CW = 0u,
	CCW
};

// bezier related code based on the excellent "A Primer on Bézier Curves" (http://pomax.github.io/bezierinfo/#decasteljau) by Mike "Pomax" Kamermans
// additional code from Bartosz Ciechanowski (http://ciechanowski.me/blog/page/2/)
class Contour {
	friend class Polygon;
	friend class Shape;
	
	public :
		Contour() = default;
		explicit Contour(const std::vector<glm::vec2>& points);
		// explicit Contour(const std::string& svgPath);
		explicit Contour(const ClipperLib::Path& clipperPath);
		
		std::vector<glm::vec2> GetPoints() const;
		void AddPoint(const glm::vec2& point);
		void AddPoints(const std::vector<glm::vec2>& points);
		void AddBezier(const std::vector<glm::vec2>& controlPoints);
		
		std::vector<Contour> GetOffset(float distance, const ClipperLib::JoinType& miterType = ClipperLib::jtRound, const double& miterLimit = 2.0d);
		Winding GetWinding() const;
		Winding ReverseWinding();
		
		// operator std::string() const;
		operator ClipperLib::Path() const;
		// void FromSVGPath(const std::string& svgPath);
		void FromClipperPath(const ClipperLib::Path& clipperPath);
	protected :
		void CreateBezier(const std::vector<glm::vec2>& controlPoints);
		void UpdateBounds(const std::vector<glm::vec2>& points);
	protected :
		std::vector<glm::vec2> mPoints;
		std::vector<glm::vec2> mBounds;
};

class Polygon : public Transformable {
	public :
		Polygon() = default;
		explicit Polygon(const std::vector<Contour>& contours, const CoordinateSpace& coordinateSpace = CoordinateSpace::Local);
		explicit Polygon(const ClipperLib::Paths& clipperPaths);
		
		void AddContour(const Contour& contour, const CoordinateSpace& coordinateSpace = CoordinateSpace::Local);
		void AddContours(const std::vector<Contour>& contours, const CoordinateSpace& coordinateSpace = CoordinateSpace::Local);
		const std::vector<Contour>& GetContours() const;
		
		void Offset(float distance, const ClipperLib::JoinType& miterType = ClipperLib::jtRound, const double& miterLimit = 2.0d);
		void ReverseWinding();
		void PositionContoursAtOrigin();
		
		operator ClipperLib::Paths() const;
		void FromClipperPaths(const ClipperLib::Paths& clipperPaths);
		
		// return a copy of this polygon with its transformations applied directly to its contours (transformations of new polygon are reset back to default)
		Polygon GetTransformed() const;
	protected :
		void UpdateGlobalBoundingBox();
		void UpdateGlobalMask();
		void CreateLocalMask();
		
		void UpdateBounds(const Contour& contour);
	protected :
		std::vector<Contour> mContours;
		std::vector<glm::vec2> mBounds;
};
}

#endif
