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

#include "polygon.hpp"

#include <iostream>
#include <algorithm>

#include <glm/gtx/fast_square_root.hpp>

#include "util.hpp"
#include "shape.hpp"
#include "triangulate.hpp"

namespace uair {
Polygon::Polygon(const ClipperLib::Paths& paths) {
	FromClipperPaths(paths);
}

std::vector<glm::vec2> Polygon::GetPoints(const unsigned int& coordinateSpace) const {
	if (coordinateSpace == CoordinateSpace::Local) { // if coordinate space is local...
		return mPoints; // return points as is
	}
	
	std::vector<glm::vec2> global; // otherwise coordinate space is global
	for (auto iter = mPoints.begin(); iter != mPoints.end(); ++iter) { // for all points...
		global.emplace_back(iter->x + mPosition.x, iter->y + mPosition.y); // adjust point to be global
	}
	
	return global; // return global points
}

std::vector< std::vector<glm::vec2> > Polygon::GetInnerBoundaries(const unsigned int& coordinateSpace) const {
	if (coordinateSpace == CoordinateSpace::Local) { // if coordinate space is local...
		return mInnerBoundaries; // return inner boundaries as is
	}
	
	std::vector< std::vector<glm::vec2> > global; // otherwise coordinate space is global
	for (auto iter = mInnerBoundaries.begin(); iter != mInnerBoundaries.end(); ++iter) { // for all outer boundaries...
		global.emplace_back(); // add empty boundary
		
		for (auto jter = iter->begin(); jter != iter->end(); ++jter) { // for all points in the boundary...
			global.back().emplace_back(jter->x + mPosition.x, jter->y + mPosition.y); // adjust point to be global
		}
	}
	
	return global; // return global points
}

void Polygon::AddPoint(const glm::vec2& point, const unsigned int& coordinateSpace) {
	if (coordinateSpace == CoordinateSpace::Local) { // if coordinate space is local...
		mPoints.push_back(point); // add point to outer boundary
	}
	else { // otherwise coordinate space is global...
		mPoints.push_back(point - mPosition); // adjust point to be local
	}
	
	UpdateBounds({point}); // update bounds of polygon
}

void Polygon::AddPoints(const std::vector<glm::vec2>& points, const unsigned int& coordinateSpace) {
	if (coordinateSpace == CoordinateSpace::Local) { // if coordinate space is local...
		mPoints.insert(mPoints.end(), points.begin(), points.end()); // add points to outer boundary
		UpdateBounds(points); // update bounds of polygon
	}
	else { // otherwise coordinate space is global...
		std::vector<glm::vec2> local; // points converted to local coordinates
		for (auto iter = points.begin(); iter != points.end(); ++iter) { // for all points to be added...
			local.emplace_back(iter->x - mPosition.x, iter->y - mPosition.y); // adjust point to be local
		}
		
		mPoints.insert(mPoints.end(), local.begin(), local.end()); // add points to outer boundary
		UpdateBounds(local); // update bounds of polygon
	}
}

void Polygon::AddBezier(const std::vector<glm::vec2>& controlPoints, const unsigned int& coordinateSpace) {
	if (controlPoints.size() >= 2) { // if there are at least 2 control points...
		std::vector<glm::vec2> adjustedControlPoints; // control points with start point added to front
		
		if (!mPoints.empty()) { // if shape has any points...
			adjustedControlPoints.push_back(mPoints.back()); // add last as first control point
		}
		else {
			AddPoint(glm::vec2(0.0f, 0.0f)); // add initial point
			adjustedControlPoints.emplace_back(0.0f, 0.0f); // otherwise add local origin vector
		}
		
		if (coordinateSpace == CoordinateSpace::Local) { // if coordinate space is local...
			for (auto iter = controlPoints.begin(); iter != controlPoints.end(); ++iter) { // for all other control points...
				adjustedControlPoints.push_back(*iter); // add control point to adjusted list
			}
		}
		else { // otherwise coordinate space is global...
			for (auto iter = controlPoints.begin(); iter != controlPoints.end(); ++iter) { // for all other control points...
				adjustedControlPoints.push_back(*iter - mPosition); // add control point to adjusted list (convert to local)
			}
		}
		
		CreateBezier(adjustedControlPoints); // create the bezier
	}
}

void Polygon::AddInnerBoundary(const std::vector<glm::vec2>& innerBoundary, const unsigned int& coordinateSpace) {
	if (coordinateSpace == CoordinateSpace::Local) { // if coordinate space is local...
		mInnerBoundaries.push_back(innerBoundary); // add boundary to list
	}
	else { // otherwise coordinate space is global...
		std::vector<glm::vec2> local; // points converted to local coordinates
		for (auto iter = innerBoundary.begin(); iter != innerBoundary.end(); ++iter) { // for all points in the boundary...
			local.emplace_back(iter->x - mPosition.x, iter->y - mPosition.y); // adjust point to be local
		}
		
		mInnerBoundaries.push_back(local); // add boundary to list
	}
}

void Polygon::Clear() {
	mPoints.clear();
	mInnerBoundaries.clear();
	mBounds.clear();
	mLocalBoundingBox.clear();
	mGlobalBoundingBox.clear();
}

void Polygon::MakeRectangle(const float& width, const float& height, const glm::vec2& offset) {
	Clear();
	
	AddPoints({glm::vec2(offset.x, offset.y), glm::vec2(offset.x + width, offset.y), glm::vec2(offset.x + width, offset.y + height), glm::vec2(offset.x, offset.y + height)});
}

void Polygon::MakeCircle(const float& radius, const unsigned int& numPoints, const glm::vec2& offset) {
	Clear();
	
	std::vector<glm::vec2> points;
	glm::vec2 pos = offset + radius;
	
	float angle = 0.0f;
	float angleInc = (360.0f / static_cast<float>(numPoints)) * (util::PI / 180.0f);
	
	for (unsigned int i = 0u; i < numPoints; ++i) {
		points.emplace_back(pos.x + (radius * std::cos(angle)), pos.y + (radius * std::sin(angle)));
		angle += angleInc;
	}
	
	AddPoints(points);
}

void Polygon::FromSVGPath(const std::string& svgPath, const unsigned int& smoothness) {
	Clear();
	
	auto iterPath = svgPath.begin();
	
	{
		size_t start = svgPath.find(R"(d=")"); // "
		if (start != svgPath.npos) {
			iterPath += (start + 3);
		}
		else {
			return;
		}
	}
	
	std::vector<Polygon> boundaries;
	std::map<char, std::function<bool()> > commands;
	std::vector<float> parameters;
	glm::vec2 lastPoint = glm::vec2(0.0f, 0.0f);
	
	{ // moveto commands
		// relative
		commands.insert(std::make_pair('m',
			[&]() {
				if (parameters.size() >= 2u) {
					boundaries.emplace_back();
					
					lastPoint.x += parameters.at(0u); lastPoint.y += parameters.at(1u);
					boundaries.back().AddPoint(lastPoint);
					
					for (size_t i = 2u; i < parameters.size(); i += 2u) {
						if (i != (parameters.size() - 1u)) {
							lastPoint.x += parameters.at(i); lastPoint.y += parameters.at(i + 1);
							boundaries.back().AddPoint(lastPoint);
						}
						else {
							return false;
						}
					}
				}
				else {
					return false;
				}
				
				return true;
			}
		));
		
		// absolute
		commands.insert(std::make_pair('M',
			[&]() {
				if (parameters.size() >= 2u) {
					boundaries.emplace_back();
					
					lastPoint.x = parameters.at(0u); lastPoint.y = parameters.at(1u);
					boundaries.back().AddPoint(lastPoint);
					
					for (size_t i = 2u; i < parameters.size(); i += 2u) {
						if (i != (parameters.size() - 1u)) {
							lastPoint.x = parameters.at(i); lastPoint.y = parameters.at(i + 1);
							boundaries.back().AddPoint(lastPoint);
						}
						else {
							return false;
						}
					}
				}
				else {
					return false;
				}
				
				return true;
			}
		));
	}
	
	{ // closepath command
		// relative and absolute
		commands.insert(std::make_pair('z',
			[&]() {
				if (!boundaries.empty()) {
					const std::vector<glm::vec2>& points = boundaries.back().GetPoints();
					
					if (!points.empty()) {
						lastPoint = points.front();
						boundaries.back().AddPoint(lastPoint);
					}
				}
				else {
					return false;
				}
				
				return true;
			}
		));
	}
	
	{ // lineto commands
		// relative
		commands.insert(std::make_pair('l',
			[&]() {
				if (parameters.size() >= 2u) {
					for (size_t i = 0u; i < parameters.size(); i += 2u) {
						if (i != (parameters.size() - 1u)) {
							lastPoint.x += parameters.at(i); lastPoint.y += parameters.at(i + 1);
							boundaries.back().AddPoint(lastPoint);
						}
						else {
							return false;
						}
					}
				}
				else {
					return false;
				}
				
				return true;
			}
		));
		
		// absolute
		commands.insert(std::make_pair('L',
			[&]() {
				if (parameters.size() >= 2u) {
					for (size_t i = 0u; i < parameters.size(); i += 2u) {
						if (i != (parameters.size() - 1u)) {
							lastPoint.x = parameters.at(i); lastPoint.y = parameters.at(i + 1);
							boundaries.back().AddPoint(lastPoint);
						}
						else {
							return false;
						}
					}
				}
				else {
					return false;
				}
				
				return true;
			}
		));
		
		// relative horizontal
		commands.insert(std::make_pair('h',
			[&]() {
				if (!parameters.empty()) {
					for (size_t i = 0u; i < parameters.size(); ++i) {
						lastPoint.x += parameters.at(i);
						boundaries.back().AddPoint(lastPoint);
					}
				}
				else {
					return false;
				}
				
				return true;
			}
		));
		
		// absolute horizontal
		commands.insert(std::make_pair('H',
			[&]() {
				if (!parameters.empty()) {
					for (size_t i = 0u; i < parameters.size(); ++i) {
						lastPoint.x = parameters.at(i);
						boundaries.back().AddPoint(lastPoint);
					}
				}
				else {
					return false;
				}
				
				return true;
			}
		));
		
		// relative vertical
		commands.insert(std::make_pair('v',
			[&]() {
				if (!parameters.empty()) {
					for (size_t i = 0u; i < parameters.size(); ++i) {
						lastPoint.y += parameters.at(i);
						boundaries.back().AddPoint(lastPoint);
					}
				}
				else {
					return false;
				}
				
				return true;
			}
		));
		
		// absolute vertical
		commands.insert(std::make_pair('V',
			[&]() {
				if (!parameters.empty()) {
					for (size_t i = 0u; i < parameters.size(); ++i) {
						lastPoint.y = parameters.at(i);
						boundaries.back().AddPoint(lastPoint);
					}
				}
				else {
					return false;
				}
				
				return true;
			}
		));
	}
	
	{ // bezier commands
		// relative cubic
		commands.insert(std::make_pair('c',
			[&]() {
				if (parameters.size() >= 6u) {
					for (size_t i = 0u; i < parameters.size(); i += 6u) {
						if (parameters.size() - i >= 6) {
							std::vector<glm::vec2> bezier;
							
							bezier.push_back(glm::vec2(lastPoint.x + parameters.at(i), lastPoint.y + parameters.at(i + 1)));
							bezier.push_back(glm::vec2(lastPoint.x + parameters.at(i + 2), lastPoint.y + parameters.at(i + 3)));
							lastPoint.x += parameters.at(i + 4); lastPoint.y += parameters.at(i + 5); bezier.push_back(lastPoint);
							
							boundaries.back().AddBezier(bezier);
						}
						else {
							return false;
						}
					}
				}
				else {
					return false;
				}
				
				return true;
			}
		));
		
		// relative cubic
		commands.insert(std::make_pair('C',
			[&]() {
				if (parameters.size() >= 6u) {
					for (size_t i = 0u; i < parameters.size(); i += 6u) {
						if (parameters.size() - i >= 6) {
							std::vector<glm::vec2> bezier;
							
							bezier.push_back(glm::vec2(parameters.at(i), parameters.at(i + 1)));
							bezier.push_back(glm::vec2(parameters.at(i + 2), parameters.at(i + 3)));
							lastPoint.x = parameters.at(i + 4); lastPoint.y = parameters.at(i + 5); bezier.push_back(lastPoint);
							
							boundaries.back().AddBezier(bezier);
						}
						else {
							return false;
						}
					}
				}
				else {
					return false;
				}
				
				return true;
			}
		));
	}
	
	char currentCommand = ' ';
	std::string currentParam = "";
	
	while (iterPath != svgPath.end() && *iterPath != '"') {
		if (*iterPath == ' ' || *iterPath == ',' || *iterPath == ';') {
			if (!currentParam.empty()) {
				parameters.push_back(std::move(util::FromString<float>(currentParam)));
			}
			
			currentParam = "";
		}
		else if (*iterPath == 'M' || *iterPath == 'm' || *iterPath == 'Z' || *iterPath == 'z' ||
				*iterPath == 'L' || *iterPath == 'l' || *iterPath == 'H' || *iterPath == 'h' || *iterPath == 'V' || *iterPath == 'v' ||
				*iterPath == 'C' || *iterPath == 'c') {
			
			if (!currentParam.empty()) {
				parameters.push_back(std::move(util::FromString<float>(currentParam)));
			}
			
			auto iter = commands.find(currentCommand);
			if (iter != commands.end()) {
				iter->second();
			}
			
			parameters.clear();
			currentCommand = *iterPath;
			currentParam = "";
			
			if (currentCommand == 'Z') {
				currentCommand = 'z';
			}
		}
		else {
			currentParam += *iterPath;
		}
		
		++iterPath;
	}
	
	if (!currentParam.empty()) {
		parameters.push_back(std::move(util::FromString<float>(currentParam)));
		
		auto iter = commands.find(currentCommand);
		if (iter != commands.end()) {
			iter->second();
		}
	}
	
	if (!boundaries.empty()) {
		{
			std::vector<glm::vec2> points = boundaries.front().GetPoints();
			
			while (!points.empty() && points.front() == points.back()) {
				points.pop_back();
			}
			
			AddPoints(points);
		}
		
		for (auto iter = boundaries.begin() + 1; iter != boundaries.end(); ++iter) {
			std::vector<glm::vec2> points = iter->GetPoints();
			
			while (!points.empty() && points.front() == points.back()) {
				points.pop_back();
			}
			
			AddInnerBoundary(points);
		}
		
		FixWinding();
	}
}

Polygon::operator Shape() const {
	Shape s;
	s.mPoints = mPoints;
	s.mInnerBoundaries = mInnerBoundaries;
	s.mBounds = mBounds;
	
	return s;
}

Polygon::operator ClipperLib::Paths() const {
	ClipperLib::Paths p;
	
	glm::mat3 transform = mTransformation;
	transform *= util::GetTranslationMatrix(mPosition - mOrigin);
	
	transform *= util::GetTranslationMatrix(mOrigin);
	transform *= util::GetRotationMatrix(mRotation);
	transform *= util::GetSkewingMatrix(mSkew);
	transform *= util::GetScalingMatrix(mScale);
	transform *= util::GetTranslationMatrix(-mOrigin);
	
	p.emplace_back();
	for (auto point = mPoints.begin(); point != mPoints.end(); ++point) {
		glm::vec3 transPoint = transform * glm::vec3(*point, 1.0f);
		p.back() << ClipperLib::IntPoint(static_cast<int>(std::round(transPoint.x)), static_cast<int>(std::round(transPoint.y)));
	}
	
	for (auto boundary = mInnerBoundaries.begin(); boundary != mInnerBoundaries.end(); ++boundary) {
		p.emplace_back();
		for (auto point = boundary->begin(); point != boundary->end(); ++point) {
			glm::vec3 transPoint = transform * glm::vec3(*point, 1.0f);
			p.back() << ClipperLib::IntPoint(static_cast<int>(std::round(transPoint.x)), static_cast<int>(std::round(transPoint.y)));
		}
	}
	
	return p;
}

void Polygon::FromClipperPaths(const ClipperLib::Paths& paths) {
	Clear();
	
	std::vector< std::vector<glm::vec2> > points;
	for (auto path = paths.begin(); path != paths.end(); ++path) {
		points.emplace_back();
		for (auto point = path->begin(); point != path->end(); ++point) {
			points.back().emplace_back(static_cast<float>(point->X), static_cast<float>(point->Y));
		}
	}
	
	AddPoints(*points.begin());
	/* for (auto boundary = points.begin() + 1; boundary != points.end(); ++boundary) {
		AddInnerBoundary(*boundary);
	} */
}

bool Polygon::FixWinding() {
	bool reversed = false;
	float sum = 0.0f;
	for (unsigned int i = 0u; i < mPoints.size(); ++i) {
		const glm::vec2& curr = mPoints.at(i);
		const glm::vec2& next = mPoints.at((i + 1) % mPoints.size());
		
		sum += (next.x - curr.x) * (next.y + curr.y);
	}
	
	if (!mPoints.empty()) {
		if (util::CompareFloats(sum, util::GreaterThan, 0.0f)) {
			std::reverse(mPoints.begin() + 1, mPoints.end());
			reversed = true;
		}
	}
	
	for (auto iter = mInnerBoundaries.begin(); iter != mInnerBoundaries.end(); ++iter) {
		sum = 0.0f;
		for (unsigned int i = 0u; i < iter->size(); ++i) {
			const glm::vec2& curr = iter->at(i);
			const glm::vec2& next = iter->at((i + 1) % iter->size());
			
			sum += (next.x - curr.x) * (next.y + curr.y);
		}
		
		if (!iter->empty()) {
			if (util::CompareFloats(sum, util::LessThan, 0.0f)) {
				std::reverse(iter->begin() + 1, iter->end());
				reversed = true;
			}
		}
	}
	
	return reversed;
}

Polygon Polygon::Offset(const float& distance) {
	ClipperLib::ClipperOffset co;
	ClipperLib::Paths poly = *this;
	
    co.AddPaths(poly, ClipperLib::jtRound, ClipperLib::etClosedPolygon);
    co.Execute(poly, distance);
	
	return Polygon(poly);
}

void Polygon::UpdateGlobalBoundingBox() {
	glm::mat3 trans = mTransformation;
	
	trans *= util::GetTranslationMatrix(mPosition - mOrigin);
	
	trans *= util::GetTranslationMatrix(mOrigin);
	trans *= util::GetRotationMatrix(mRotation);
	trans *= util::GetSkewingMatrix(mSkew);
	trans *= util::GetScalingMatrix(mScale);
	trans *= util::GetTranslationMatrix(-mOrigin);
	
	std::vector<glm::vec2> transformedPoints(mPoints.begin(), mPoints.end());
	
	if (!transformedPoints.empty()) {
		std::vector<glm::vec2> transformedBounds;
		
		{
			glm::vec3 pos = trans * glm::vec3(transformedPoints.front(), 1.0f);
			transformedBounds.emplace_back(pos.x, pos.y);
			transformedBounds.emplace_back(pos.x, pos.y);
		}
		
		for (auto iter = transformedPoints.begin() + 1; iter != transformedPoints.end(); ++iter) {
			glm::vec3 pos = trans * glm::vec3(*iter, 1.0f);
			iter->x = pos.x; iter->y = pos.y;
			
			if (iter->x < transformedBounds.at(0).x) { // left
				transformedBounds.at(0).x = iter->x;
			}
			else if (iter->x > transformedBounds.at(1).x) { // right
				transformedBounds.at(1).x = iter->x;
			}
			
			if (iter->y < transformedBounds.at(0).y) { // top
				transformedBounds.at(0).y = iter->y;
			}
			else if (iter->y > transformedBounds.at(1).y) { // bottom
				transformedBounds.at(1).y = iter->y;
			}
		}
		
		mGlobalBoundingBox.clear();
		mGlobalBoundingBox.emplace_back(transformedBounds.at(0).x, transformedBounds.at(0).y);
		mGlobalBoundingBox.emplace_back(transformedBounds.at(1).x, transformedBounds.at(0).y);
		mGlobalBoundingBox.emplace_back(transformedBounds.at(1).x, transformedBounds.at(1).y);
		mGlobalBoundingBox.emplace_back(transformedBounds.at(0).x, transformedBounds.at(1).y);
	}
}

void Polygon::UpdateGlobalMask() {
	glm::mat3 trans = mTransformation;
	
	trans *= util::GetTranslationMatrix(mPosition - mOrigin);
	
	trans *= util::GetTranslationMatrix(mOrigin);
	trans *= util::GetRotationMatrix(mRotation);
	trans *= util::GetSkewingMatrix(mSkew);
	trans *= util::GetScalingMatrix(mScale);
	trans *= util::GetTranslationMatrix(-mOrigin);
	
	mGlobalMask.clear();
	for (auto iter = mLocalMask.begin(); iter != mLocalMask.end(); ++iter) {
		glm::vec3 pos = trans * glm::vec3(*iter, 1.0f);
		mGlobalMask.emplace_back(pos.x, pos.y);
	}
}

void Polygon::CreateLocalMask() {
	mLocalMask.clear();
	
	for (auto iter = mPoints.begin(); iter != mPoints.end(); ++iter) {
		mLocalMask.push_back(*iter);
	}
}

void Polygon::UpdateBounds(const std::vector<glm::vec2>& points) {
	if (!points.empty()) {
		bool adjusted = false;
		
		if (mBounds.empty()) {
			mBounds.push_back(points.front());
			mBounds.push_back(points.front());
			adjusted = true;
		}
		
		for (auto iter = points.begin(); iter != points.end(); ++iter) {
			if (iter->x < mBounds.at(0).x) { // left
				mBounds.at(0).x = iter->x;
				adjusted = true;
			}
			else if (iter->x > mBounds.at(1).x) { // right
				mBounds.at(1).x = iter->x;
				adjusted = true;
			}
			
			if (iter->y < mBounds.at(0).y) { // top
				mBounds.at(0).y = iter->y;
				adjusted = true;
			}
			else if (iter->y > mBounds.at(1).y) { // bottom
				mBounds.at(1).y = iter->y;
				adjusted = true;
			}
		}
		
		if (adjusted == true) {
			mLocalBoundingBox.clear();
			mLocalBoundingBox.emplace_back(mBounds.at(0).x, mBounds.at(0).y);
			mLocalBoundingBox.emplace_back(mBounds.at(1).x, mBounds.at(0).y);
			mLocalBoundingBox.emplace_back(mBounds.at(1).x, mBounds.at(1).y);
			mLocalBoundingBox.emplace_back(mBounds.at(0).x, mBounds.at(1).y);
			
			UpdateGlobalBoundingBox();
		}
	}
}

void Polygon::CreateBezier(const std::vector<glm::vec2>& controlPoints) {
	unsigned int numPoints; // number of points to subdivide curve into
	
	{
		float length; // total length of lines between consecutive control points
		for (auto iter = controlPoints.begin(); iter < controlPoints.end() - 1; ++iter) { // for all control points...
			length += glm::fastDistance(*iter, *(iter + 1)); // get length of line between current point and next
		}
		
		length /= 32.0f; // divide length by 32
		numPoints = static_cast<unsigned int>(std::ceil(std::sqrt((length * length * 0.85f) + 140.0f))); // calculate number of points required
	}
	
	// find point on curve at distance defined by ratio via recursion
	std::function<void(const std::vector<glm::vec2>&, const float&)> SubdivideCurve = [&](const std::vector<glm::vec2>& points, const float& ratio) {
		if (points.size() == 1) { // if one point remains...
			AddPoint(points.back(), CoordinateSpace::Local); // add that point to curve
		}
		else if (!points.empty()) { // otherwise as long as points remain...
			std::vector<glm::vec2> newPoints; // array of new points (will be one less point than in current points array)
			for (auto iter = points.begin(); iter != points.end() - 1; ++iter) { // for all points in current...
				glm::vec2 newPoint = ((1 - ratio) * *iter) + (ratio * *(iter + 1)); // calculate point at distance define by ratio
				newPoints.push_back(std::move(newPoint)); // move point into new points array
			}
			
			SubdivideCurve(newPoints, ratio); // recall function with list of new points
		}
	};
	
	float numPointsFloat = static_cast<float>(numPoints - 1); // divisor for ratio as a float to ensure float result
	for (unsigned int i = 1u; i < numPoints - 1; ++i) { // for all ratios between 0 and 1 exclusive...
		SubdivideCurve(controlPoints, i / numPointsFloat); // subdivide the curve with current ratio
	}
	
	AddPoint(controlPoints.back(), CoordinateSpace::Local); // add final point
}
}
