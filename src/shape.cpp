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

#include "shape.hpp"

#include <iostream>

#include "renderbatch.hpp"
#include "util.hpp"

namespace uair {
std::string Shape::GetTag() const {
	return "Shape"; // indicate that this renderable is a shape
}

std::vector<glm::vec2> Shape::GetPoints() const {
	return mPoints; // return a copy of the vector of relative points
}

std::vector<glm::vec2> Shape::GetAbsolutePoints() const {
	std::vector<glm::vec2> absolute = GetPoints(); // get the relative points
	for (auto iter = absolute.begin(); iter != absolute.end(); ++iter) { // for all relative points
		*iter += mPosition; // make them absolute by adding the position
	}
	
	return absolute; // return the new absolute vector
}

void Shape::AddPoint(const glm::vec2 & point) {
	mPoints.push_back(point); // add the point to the vector
	
	// check if the current point is outwith any of the bounds and if so adjust the bounds
	bool adjusted = false;
	if (point.x < mBounds.at(0).x) { // left
		mBounds.at(0).x = point.x;
		adjusted = true;
	}
	else if (point.x > mBounds.at(1).x) { // right
		mBounds.at(1).x = point.x;
		adjusted = true;
	}
	
	if (point.y < mBounds.at(0).y) { // top
		mBounds.at(0).y = point.y;
		adjusted = true;
	}
	else if (point.y > mBounds.at(1).y) { // bottom
		mBounds.at(1).y = point.y;
		adjusted = true;
	}
	
	if (adjusted == true) { // if we updated any of the bounds
		// update the local bounding box
		mLocalBoundingBox.Clear();
		mLocalBoundingBox.SetPosition(glm::vec2(mBounds.at(0).x, mBounds.at(0).y));
		mLocalBoundingBox.AddPoint(glm::vec2(mBounds.at(1).x - mBounds.at(0).x,                                 0));
		mLocalBoundingBox.AddPoint(glm::vec2(mBounds.at(1).x - mBounds.at(0).x, mBounds.at(1).y - mBounds.at(0).y));
		mLocalBoundingBox.AddPoint(glm::vec2(                                0, mBounds.at(1).y - mBounds.at(0).y));
		
		UpdateGlobalBoundingBox(); // update the global bounding box
	}
}

void Shape::AddPoints(const std::vector<glm::vec2> & points) {
	mPoints.insert(mPoints.end(), points.begin(), points.end()); // add all points to the vector
	
	// check if the current point is outwith any of the bounds and if so adjust the bounds
	bool adjusted = false;
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
	
	if (adjusted == true) { // if we updated any of the bounds
		// update the local bounding box
		mLocalBoundingBox.Clear();
		mLocalBoundingBox.SetPosition(glm::vec2(mBounds.at(0).x, mBounds.at(0).y));
		mLocalBoundingBox.AddPoint(glm::vec2(mBounds.at(1).x - mBounds.at(0).x,                                 0));
		mLocalBoundingBox.AddPoint(glm::vec2(mBounds.at(1).x - mBounds.at(0).x, mBounds.at(1).y - mBounds.at(0).y));
		mLocalBoundingBox.AddPoint(glm::vec2(                                0, mBounds.at(1).y - mBounds.at(0).y));
		
		UpdateGlobalBoundingBox(); // update the global bounding box
	}
}

void Shape::AddAbsolutePoint(const glm::vec2 & point) {
	glm::vec2 relative = point - mPosition; // convert the point to a relative point
	AddPoint(relative); // call the relative add point function
}

void Shape::AddAbsolutePoints(const std::vector<glm::vec2> & points) {
	std::vector<glm::vec2> relative; // vector to hold converted points
	for (auto iter = points.begin(); iter != points.end(); ++iter) { // for all absolute points
		relative.push_back(*iter - mPosition); // convert them to relative points and add to the vector
	}
	
	AddPoints(relative); // call the relative add points function
}

void Shape::Clear() {
	mPoints.clear(); // clear the vector of points
	
	// reset the bounds and bounding boxes
	mBounds = {glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f)};
	mLocalBoundingBox.Clear();
	mGlobalBoundingBox.Clear();
}

void Shape::MakeRectangle(const glm::vec2 & position, const glm::vec2 & size) {
	Clear(); // clear the current shape
	
	// create a rectangle of the specified dimensions
	SetPosition(position);
	AddPoint(glm::vec2(size.x,      0));
	AddPoint(glm::vec2(size.x, size.y));
	AddPoint(glm::vec2(     0, size.y));
	
	UpdateGlobalBoundingBox(); // update the global bounding box
}

void Shape::MakeCircle(const glm::vec2 & position, const float & radius, const unsigned int & numPoints) {
	Clear(); // clear the current shape
	
	SetPosition(position); // set the position to the specified
	
	float angle = 0; // the current angle
	float angleInc = (360 / numPoints) * (util::PI / 180); // the amount to increment the angle every iteration
	
	while (angle < util::PI * 2) { // while we don't have a full circle
		AddPoint(glm::vec2(radius * std::cos(angle), radius * std::sin(angle))); // add a point of the circle using current angle
		angle += angleInc; // update the angle
	}
	
	UpdateGlobalBoundingBox(); // update the global bounding box
}

void Shape::UpdateGlobalBoundingBox() {
	Polygon poly; // temporary polygon representing the shape (without transforms)
	for (auto iter = mPoints.begin(); iter != mPoints.end(); ++iter) { // for all points in the shape
		poly.AddPoint(*iter); // add the point to the polygon
	}
	
	// get the transformation matrix that represents the shape
	glm::mat3 trans = mTransformation;
	
	trans *= util::GetTranslationMatrix(mPosition - mOrigin);
	
	trans *= util::GetTranslationMatrix(mOrigin);
	trans *= util::GetRotationMatrix(mRotation);
	trans *= util::GetSkewingMatrix(mSkew);
	trans *= util::GetScalingMatrix(mScale);
	trans *= util::GetTranslationMatrix(-mOrigin);
	
	poly.Transform(trans); // transform the temp polygon by the shape's transformation
	std::vector<glm::vec2> bounds = poly.GetBounds(); // get the bounds of the temp polygon
	glm::vec2 pos = poly.GetPosition(); // store it's position
	
	// clear the global bounding box and set it to the bounds of the temp polygon
	mGlobalBoundingBox.Clear();
	mGlobalBoundingBox.SetPosition(pos + bounds.at(0));
	mGlobalBoundingBox.AddPoint(glm::vec2(bounds.at(1).x - bounds.at(0).x,                               0));
	mGlobalBoundingBox.AddPoint(glm::vec2(bounds.at(1).x - bounds.at(0).x, bounds.at(1).y - bounds.at(0).y));
	mGlobalBoundingBox.AddPoint(glm::vec2(                              0, bounds.at(1).y - bounds.at(0).y));
}

void Shape::UpdateGlobalMask() {
	mGlobalMask = mLocalMask; // set the global mask to the local mask
	
	// get the transformation matrix that represents the shape
	glm::mat3 trans = mTransformation;
	
	trans *= util::GetTranslationMatrix(mPosition - mOrigin);
	
	trans *= util::GetTranslationMatrix(mOrigin);
	trans *= util::GetRotationMatrix(mRotation);
	trans *= util::GetSkewingMatrix(mSkew);
	trans *= util::GetScalingMatrix(mScale);
	trans *= util::GetTranslationMatrix(-mOrigin);
	
	mGlobalMask.Transform(trans); // transform the global mask by the shape's transformation
}

void Shape::CreateLocalMask() {
	mLocalMask.Clear(); // clear the current local mask
	
	for (auto iter = mPoints.begin(); iter != mPoints.end(); ++iter) { // for all points in the shape
		mLocalMask.AddPoint(*iter); // add the point to the local mask
	}
}

RenderBatchData Shape::Upload() {
	RenderBatchData rbd; // our batch data representing the shape
	
	// get the transformation matrix that represents the shape
	glm::mat3 trans = mTransformation;
	
	trans *= util::GetTranslationMatrix(mPosition - mOrigin);
	
	trans *= util::GetTranslationMatrix(mOrigin);
	trans *= util::GetRotationMatrix(mRotation);
	trans *= util::GetSkewingMatrix(mSkew);
	trans *= util::GetScalingMatrix(mScale);
	trans *= util::GetTranslationMatrix(-mOrigin);
	
	// get all points representing the shape including the origin (0, 0)
	std::vector<glm::vec2> points;
	points.emplace_back(0.0f, 0.0f); points.insert(points.end(), mPoints.begin(), mPoints.end());
	
	for (unsigned int i = 0u; i < points.size(); ++i) { // for all points
		// convert the point into a vbo vertex
		glm::vec3 pos = trans * glm::vec3(points.at(i), 1.0f);
		
		VBOVertex vert;
		vert.mX = pos.x; vert.mY = pos.y; vert.mZ = mDepth;
		vert.mNX = 0.0f; vert.mNY = 0.0f; vert.mNZ = 1.0f;
		vert.mS = 0.0f; vert.mT = 0.0f; vert.mLayer = 0.0f;
		vert.mR = mColour.x; vert.mG = mColour.y; vert.mB = mColour.z; vert.mA = mAlpha;
		vert.mTex = 0.0f;
		
		rbd.mVertData.push_back(vert); // add the vbo vertex to the batch data
	}
	
	int vertDataSize = static_cast<int>(rbd.mVertData.size());
	for (int i = 0u; i < vertDataSize - 2; ++i) { // for all vbo vertices in the batch data
		// add the appropiate vbo indices
		rbd.mIndData.emplace_back(i);
		rbd.mIndData.emplace_back(i + 1);
		rbd.mIndData.emplace_back(vertDataSize - 1);
	}
	
	rbd.mTexID = 0; // set the texture id
	rbd.mTag = GetTag(); // set the renderables tag
	
	return rbd; // return the batch data
}
}
