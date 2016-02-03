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

#include "polygon.hpp"

#include <iostream>
#include <algorithm>

#include <glm/gtx/fast_square_root.hpp>

#include "util.hpp"
#include "shape.hpp"
// #include "triangulate.hpp"

namespace uair {
Contour::Contour(const std::vector<glm::vec2>& points) {
	AddPoints(points);
}

/* Contour::Contour(const std::string& svgPath) {
	FromSVGPath(svgPath);
} */

Contour::Contour(const ClipperLib::Path& clipperPath) {
	FromClipperPath(clipperPath);
}

std::vector<glm::vec2> Contour::GetPoints() const {
	return mPoints;
}

void Contour::AddPoint(const glm::vec2& point) {
	mPoints.push_back(point); // add point to contour
	UpdateBounds({point});
}

void Contour::AddPoints(const std::vector<glm::vec2>& points) {
	mPoints.insert(mPoints.end(), points.begin(), points.end()); // add points to contour
	UpdateBounds(points);
}

void Contour::AddBezier(const std::vector<glm::vec2>& controlPoints) {
	if (controlPoints.size() >= 2) { // if there are at least 2 control points...
		std::vector<glm::vec2> adjustedControlPoints; // control points with start point added to front
		
		if (!mPoints.empty()) { // if contour has any points...
			adjustedControlPoints.push_back(mPoints.back()); // add last as first control point
		}
		else {
			AddPoint(glm::vec2(0.0f, 0.0f)); // add initial point to contour
			adjustedControlPoints.emplace_back(0.0f, 0.0f); // add new point as first control point
		}
		
		for (auto iter = controlPoints.begin(); iter != controlPoints.end(); ++iter) { // for all input control points...
			adjustedControlPoints.push_back(*iter); // add to adjusted control points
		}
		
		CreateBezier(adjustedControlPoints); // create the bezier with adjusted control points
	}
}

std::vector<Contour> Contour::GetOffset(float distance, const ClipperLib::JoinType& miterType, const double& miterLimit) {
	if (GetWinding() == Winding::CCW) { // reverse the offset direction if contour is ccw
		distance = -distance;
	}
	
	ClipperLib::ClipperOffset clipperOffset;
	ClipperLib::Path inPath = static_cast<ClipperLib::Path>(*this);
	ClipperLib::Paths outPaths;
	
	clipperOffset.MiterLimit = miterLimit;
    clipperOffset.AddPath(inPath, miterType, ClipperLib::etClosedPolygon);
    clipperOffset.Execute(outPaths, distance);
	
	std::vector<Contour> outContours;
	for (auto path = outPaths.begin(); path != outPaths.end(); ++path) {
		outContours.emplace_back(*path);
	}
	
	return outContours;
}

Winding Contour::GetWinding() const {
	float area = 0.0f;
	for (unsigned int i = 0u; i < mPoints.size(); ++i) {
		area += (mPoints.at((i + 1) % mPoints.size()).x - mPoints.at(i).x) * (mPoints.at((i + 1) % mPoints.size()).x + mPoints.at(i).y);
	}
	
	if (area >= 0.0f) {
		return Winding::CCW;
	}
	
	return Winding::CW;
}

Winding Contour::ReverseWinding() {
	Winding result = Winding::CW;
	if (GetWinding() == result) {
		result = Winding::CCW;
	}
	
	if (!mPoints.empty()) {
		std::reverse(mPoints.begin() + 1, mPoints.end());
	}
	
	return result;
}

Contour::operator ClipperLib::Path() const {
	ClipperLib::Path clipperPath;
	
	for (auto point = mPoints.begin(); point != mPoints.end(); ++point) {
		clipperPath << ClipperLib::IntPoint(static_cast<int>(std::round(point->x)), static_cast<int>(std::round(point->y)));
	}
	
	return clipperPath;
}

void Contour::FromClipperPath(const ClipperLib::Path& clipperPath) {
	mPoints.clear();
	mBounds.clear();
	
	std::vector<glm::vec2> points;
	for (auto point = clipperPath.begin(); point != clipperPath.end(); ++point) {
		points.emplace_back(static_cast<float>(point->X), static_cast<float>(point->Y));
	}
	
	AddPoints(points);
}

void Contour::CreateBezier(const std::vector<glm::vec2>& controlPoints) {
	unsigned int numPoints; // number of points to subdivide curve into
	
	{
		float length; // total length of lines between consecutive control points
		for (auto iter = controlPoints.begin(); iter < controlPoints.end() - 1; ++iter) { // for all control points...
			length += glm::fastDistance(*iter, *(iter + 1)); // get length of line between current point and next
		}
		
		length /= 32.0f; // divide length by 32
		numPoints = static_cast<unsigned int>(std::ceil(glm::fastSqrt((length * length * 0.85f) + 140.0f))); // calculate number of points required
	}
	
	// find point on curve at distance defined by ratio via recursion
	std::function<void(const std::vector<glm::vec2>&, const float&)> SubdivideCurve = [&](const std::vector<glm::vec2>& points, const float& ratio) {
		if (points.size() == 1) { // if one point remains...
			AddPoint(points.back()); // add that point to curve
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
	
	AddPoint(controlPoints.back()); // add final point
}

void Contour::UpdateBounds(const std::vector<glm::vec2>& points) {
	if (!points.empty()) {
		if (mBounds.empty()) {
			mBounds.push_back(points.front());
			mBounds.push_back(points.front());
		}
		
		for (auto iter = points.begin(); iter != points.end(); ++iter) {
			if (iter->x < mBounds.at(0).x) { // left
				mBounds.at(0).x = iter->x;
			}
			else if (iter->x > mBounds.at(1).x) { // right
				mBounds.at(1).x = iter->x;
			}
			
			if (iter->y < mBounds.at(0).y) { // top
				mBounds.at(0).y = iter->y;
			}
			else if (iter->y > mBounds.at(1).y) { // bottom
				mBounds.at(1).y = iter->y;
			}
		}
	}
}


Polygon::Polygon(const std::vector<Contour>& contours, const CoordinateSpace& coordinateSpace) {
	AddContours(contours, coordinateSpace);
}

Polygon::Polygon(const ClipperLib::Paths& clipperPaths) {
	FromClipperPaths(clipperPaths);
}

void Polygon::AddContour(const Contour& contour, const CoordinateSpace& coordinateSpace) {
	if (coordinateSpace == CoordinateSpace::Local) { // if coordinate space is local...
		mContours.push_back(contour);
	}
	else { // otherwise coordinate space is global...
		std::vector<glm::vec2> local = contour.GetPoints(); // points converted to local coordinates
		for (auto iter = local.begin(); iter != local.end(); ++iter) { // for all points to be added...
			iter->x -= mPosition.x; iter->y -= mPosition.y; // adjust point to be local
		}
		
		mContours.emplace_back(local);
	}
	
	UpdateBounds(mContours.back()); // update bounds of polygon
}

void Polygon::AddContours(const std::vector<Contour>& contours, const CoordinateSpace& coordinateSpace) {
	for (auto iter = contours.begin(); iter != contours.end(); ++iter) {
		AddContour(*iter, coordinateSpace);
	}
}

const std::vector<Contour>& Polygon::GetContours() const {
	return mContours;
}

void Polygon::Offset(float distance, const ClipperLib::JoinType& miterType, const double& miterLimit) {
	ClipperLib::ClipperOffset clipperOffset;
	ClipperLib::Paths outPaths;
	clipperOffset.MiterLimit = miterLimit;
	
	for (auto contour = mContours.begin(); contour != mContours.end(); ++contour) {
		clipperOffset.AddPath(static_cast<ClipperLib::Path>(*contour), miterType, ClipperLib::etClosedPolygon);
	}
	
    clipperOffset.Execute(outPaths, distance);
	
	std::vector<Contour> outContours;
	for (auto path = outPaths.begin(); path != outPaths.end(); ++path) {
		outContours.emplace_back(*path);
	}
	
	{ // clear invalid contours and bounds
		std::vector<Contour> contours;
		std::vector<glm::vec2> bounds;
		
		std::swap(mContours, contours);
		std::swap(mBounds, bounds);
	}
	
	AddContours(outContours); // add new offset contours to shape
}

void Polygon::ReverseWinding() {
	for (auto contour = mContours.begin(); contour != mContours.end(); ++contour) {
		contour->ReverseWinding();
	}
}

void Polygon::PositionContoursAtOrigin() {
	if (!mBounds.empty()) {
		glm::vec2 lowest = mBounds.at(0);
		
		if (!util::CompareFloats(lowest.x, util::Equals, 0.0f) || !util::CompareFloats(lowest.y, util::Equals, 0.0f)) { // if x or y value of lowest bounds is not at origin...
			std::vector<Contour> newContours;
			
			for (auto contour = mContours.begin(); contour != mContours.end(); ++contour) {
				std::vector<glm::vec2> newPoints = contour->GetPoints();
				for (auto point = newPoints.begin(); point != newPoints.end(); ++point) {
					*point -= lowest;
				}
				
				newContours.emplace_back(newPoints);
			}
			
			{ // clear invalid contours and bounds
				std::vector<Contour> contours;
				std::vector<glm::vec2> bounds;
				
				std::swap(mContours, contours);
				std::swap(mBounds, bounds);
			}
			
			AddContours(newContours); // add new offset contours to shape
		}
	}
}

Polygon::operator ClipperLib::Paths() const {
	ClipperLib::Paths clipperPaths;
	
	glm::mat3 transform = mTransformation;
	transform *= util::GetTranslationMatrix(mPosition - mOrigin);
	
	transform *= util::GetTranslationMatrix(mOrigin);
	transform *= util::GetRotationMatrix(mRotation);
	transform *= util::GetSkewingMatrix(mSkew);
	transform *= util::GetScalingMatrix(mScale);
	transform *= util::GetTranslationMatrix(-mOrigin);
	
	clipperPaths.emplace_back();
	for (auto contour = mContours.begin(); contour != mContours.end(); ++contour) {
		for (auto point = contour->mPoints.begin(); point != contour->mPoints.end(); ++point) {
			glm::vec3 transPoint = transform * glm::vec3(*point, 1.0f);
			clipperPaths.back() << ClipperLib::IntPoint(static_cast<int>(std::round(transPoint.x)), static_cast<int>(std::round(transPoint.y)));
		}
	}
	
	return clipperPaths;
}

void Polygon::FromClipperPaths(const ClipperLib::Paths& clipperPaths) {
	mContours.clear();
	mBounds.clear();
	
	std::vector<Contour> contours;
	for (auto path = clipperPaths.begin(); path != clipperPaths.end(); ++path) {
		contours.emplace_back(*path);
	}
	
	AddContours(contours);
}

Polygon Polygon::GetTransformed() const {
	Polygon newPolygon; // the transformed polygon
	glm::mat3 trans = mTransformation; // the complete transofrmation matrix (initially the custom transform matrix)
	std::vector<Contour> newContours; // the transformed contours array
	
	{ // apply the individual transforms to the tranformation matrix
		trans *= util::GetTranslationMatrix(mPosition - mOrigin);
		
		trans *= util::GetTranslationMatrix(mOrigin);
		trans *= util::GetRotationMatrix(mRotation);
		trans *= util::GetSkewingMatrix(mSkew);
		trans *= util::GetScalingMatrix(mScale);
		trans *= util::GetTranslationMatrix(-mOrigin);
	}
	
	{ // copy the mask (unchanged)
		newPolygon.mLocalMask = mLocalMask;
		newPolygon.mGlobalMask = mLocalMask;
	}
	
	for (auto contour = mContours.begin(); contour != mContours.end(); ++contour) { // for all contours in the polygon...
		Contour newContour; // the transformed contour
		for (auto point = contour->mPoints.begin(); point != contour->mPoints.end(); ++point) { // for all points in the contour...
			glm::vec3 pos = trans * glm::vec3(*point, 1.0f); // get the transformed point (as a 3d vector)
			
			newContour.AddPoint(glm::vec2(pos.x, pos.y)); // add the point to the transformed contour (only x and y are needed)
		}
		
		newContours.push_back(std::move(newContour)); // add the transformed contours to the transformed contours array
	}
	
	newPolygon.AddContours(newContours); // add the transformed contours to the new polygon
	return newPolygon; // return the new polygon
}

void Polygon::UpdateGlobalBoundingBox() {
	if (!mContours.empty() && !mContours.front().mPoints.empty()) { // transform all points, find new bounds, create bb
		glm::mat3 trans = mTransformation;
		
		trans *= util::GetTranslationMatrix(mPosition - mOrigin);
		
		trans *= util::GetTranslationMatrix(mOrigin);
		trans *= util::GetRotationMatrix(mRotation);
		trans *= util::GetSkewingMatrix(mSkew);
		trans *= util::GetScalingMatrix(mScale);
		trans *= util::GetTranslationMatrix(-mOrigin);
		
		std::vector<glm::vec2> transformedBounds{mContours.front().mPoints.front(), mContours.front().mPoints.front()};
		
		for (auto contour = mContours.begin(); contour != mContours.end(); ++contour) {
			for (auto point = contour->mPoints.begin(); point != contour->mPoints.end(); ++point) {
				glm::vec3 pos = trans * glm::vec3(*point, 1.0f);
				
				if (pos.x < transformedBounds.at(0).x) { // left
					transformedBounds.at(0).x = pos.x;
				}
				else if (pos.x > transformedBounds.at(1).x) { // right
					transformedBounds.at(1).x = pos.x;
				}
				
				if (pos.y < transformedBounds.at(0).y) { // top
					transformedBounds.at(0).y = pos.y;
				}
				else if (pos.y > transformedBounds.at(1).y) { // bottom
					transformedBounds.at(1).y = pos.y;
				}
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
	
	for (auto contour = mContours.begin(); contour != mContours.end(); ++contour) {
		for (auto point = contour->mPoints.begin(); point != contour->mPoints.end(); ++point) {
			mLocalMask.push_back(*point);
		}
	}
}

void Polygon::UpdateBounds(const Contour& contour) {
	if (contour.mBounds.size() > 1) {
		bool adjusted = false;
		
		if (mBounds.size() < 2) {
			mBounds.push_back(contour.mBounds.front());
			mBounds.push_back(contour.mBounds.front());
			adjusted = true;
		}
		
		if (contour.mBounds.at(0).x < mBounds.at(0).x) { // left
			mBounds.at(0).x = contour.mBounds.at(0).x;
			adjusted = true;
		}
		
		if (contour.mBounds.at(1).x > mBounds.at(1).x) { // right
			mBounds.at(1).x = contour.mBounds.at(1).x;
			adjusted = true;
		}
		
		if (contour.mBounds.at(0).y < mBounds.at(0).y) { // top
			mBounds.at(0).y = contour.mBounds.at(0).y;
			adjusted = true;
		}
		
		if (contour.mBounds.at(1).y > mBounds.at(1).y) { // bottom
			mBounds.at(1).y = contour.mBounds.at(1).y;
			adjusted = true;
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
}
