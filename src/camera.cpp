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

#include "camera.hpp"

#include <iostream>
#include "util.hpp"

namespace uair {
void Camera::Rotate(const Axis& axis, const float& angle) {
	switch (axis) { // depending on axis to rotate around...
		case Axis::Z : // roll
			UpdateAngle(mAngles.z, angle); // update the accumulated roll angle
			break;
		case Axis::Y : // yaw
			UpdateAngle(mAngles.y, angle); // update the accumulated yaw angle
			break;
		case Axis::X : // pitch
			UpdateAngle(mAngles.x, angle); // update the accumulated pitch angle
			break;
		default : // invalid axis
			break;
	};
}

void Camera::Move(const Axis& axis, const float& distance) {
	switch (axis) { // depending on axis to move along...
		case Axis::Z : // moving along z axis
			UpdatePosition(distance, 2); // update the x, y and z positions
			break;
		case Axis::Y : // moving along y axis
			UpdatePosition(distance, 1);
			break;
		case Axis::X : // moving along x axis
			UpdatePosition(distance, 0);
			break;
		default : // invalid axis
			break;
	};
}

glm::mat4 Camera::GetMatrix() {
	UpdateView(); // update the view matrix
	
	return mView; // return the view matrix
}

void Camera::LockToPlane(const Plane& plane) {
	mLockedPlane = plane; // set the plane to lock the camera too
}

void Camera::UpdateAngle(float& oldAngle, const float& angleChange) {
	if (!util::CompareFloats(angleChange, util::Equals, 0.0f)) { // if the change in angle is not zero (avoid unnecessary calculations)...
		oldAngle = std::fmod(oldAngle + angleChange, 360.0f); // add the change in angle to the existing angle and clip it between 0 and 360
		mUpdate = true; // indicate a view matrix update is required
	}
}

void Camera::UpdatePosition(const float& distanceChange, const int& axisIndex) {
	if (!util::CompareFloats(distanceChange, util::Equals, 0.0f)) { // if the amount to move is not zero (avoid unnecessary calculations)...
		UpdateView(); // ensure view matrix is up to date
		
		glm::vec3 positionChange; // the values from the view matrix relating to the required axis
		
		if (mLockedPlane != Plane::XY) { // if the camera isn't locked to the XY plane (i.e., allow z movement)
			positionChange.z = mView[2][axisIndex]; // store the required value from the view matrix
		}
		
		if (mLockedPlane != Plane::XZ) {
			positionChange.y = mView[1][axisIndex];
		}
		
		if (mLockedPlane != Plane::YZ) {
			positionChange.x = mView[0][axisIndex];
		}
		
		positionChange = glm::normalize(positionChange); // normalize the view matrix values (required if one has been ignored due to a locked plane)
		mPosition += (positionChange * distanceChange); // update the position (move along requested axis)
		
		mUpdate = true; // indicate an update to the view matrix is required
	}
}

void Camera::UpdateView() {
	if (mUpdate) { // if an update to the view matrix is required...
		// convert angles to radians
		float pi180 = static_cast<float>(util::PIOVER180);
		float angleZ = mAngles.z * pi180;
		float angleY = mAngles.y * pi180;
		float angleX = mAngles.x * pi180;
		
		// find the cosine and sine of each of the angles
		float cosZ = std::cos(angleZ); float sinZ = std::sin(angleZ);
		float cosY = std::cos(angleY); float sinY = std::sin(angleY);
		float cosX = std::cos(angleX); float sinX = std::sin(angleX);
		
		// create a ZYX rotation matrix
		glm::mat4 rotation = glm::mat4(cosZ * cosY,  (cosZ * sinY * sinX) - (cosX * sinZ),  (sinZ * sinX) + (cosZ * cosX * sinY),  0,
									   cosY * sinZ,  (cosZ * cosX) + (sinZ * sinY * sinX),  (cosX * sinZ * sinY) - (cosZ * sinX),  0,
											 -sinY,                           cosY * sinX,                           cosY * cosX,  0,
												 0,                                     0,                                     0,  1);
		
		// create an identity view matrix translated by position
		mView = glm::mat4(1.0f,         0.0f,         0.0f,  0.0f,
						  0.0f,         1.0f,         0.0f,  0.0f,
						  0.0f,         0.0f,         1.0f,  0.0f,
				   mPosition.x,  mPosition.y,  mPosition.z,  1.0f);
		
		mView = rotation * mView; // calculate the rotated view matrix
		mUpdate = false; // indicate that update has been performed
	}
}
}
