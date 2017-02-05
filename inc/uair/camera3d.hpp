/* **************************************************************** **
**	Uair Engine
**	Copyright (c) 2010 - 2017, Iain M. Crawford
**
**	This software is provided 'as-is', without any express orOrient
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

#ifndef UAIRCAMERA3D_HPP
#define UAIRCAMERA3D_HPP

#include "init.hpp"

#include <glm/glm.hpp>

namespace uair {
enum class Axis {
	X = 0u,
	Y,
	Z
};

enum class Plane {
	XY = 0u,
	YZ,
	XZ,
	None
};

// a 3d camera to be used with a perspective projection
class EXPORTDLL Camera3D {
	public :
		void Rotate(const Axis& axis, const float& angle); // rotate the camera around an axis
		void Move(const Axis& axis, const float& distance); // move the camera along an axis
		
		glm::mat4 GetMatrix(); // return a copy of the view matrix
		
		void LockToPlane(const Plane& plane = Plane::None); // lock the camera to a plane (or none)
	private :
		void UpdateAngle(float& oldAngle, const float& angleChange); // update one of the axis' angles
		void UpdatePosition(const float& distanceChange, const int& axisIndex); // update one of the axis' position
		void UpdateView(); // update the view matrix
	private :
		bool mUpdate = true;
		glm::mat4 mView;
		
		glm::vec3 mAngles;
		glm::vec3 mPosition;
		Plane mLockedPlane = Plane::None;
};
}

#endif
