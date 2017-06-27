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

#ifndef UAIRBODY_HPP
#define UAIRBODY_HPP

#include <iostream>

#include "polygon.hpp"

namespace uair {
// 
class EXPORTDLL Body {
	private :
		// 
		class BoundingCircle {
			public :
				// update the circle's centre and radius to encompass new points
				void AddPoint(const glm::vec2& point);
				void AddPoints(const std::vector<glm::vec2>& points);
				
				std::pair<glm::vec2, float> GetCircle() const; // return the centre and radius of the circle
			
			private :
				glm::vec2 mCentre; // the centre-point of the circle
				float mRadius = 0.0f;
				
				glm::vec2 mPointTotal; // the sum of all points that the circle encompasses
				glm::vec2 mPointFurthest; // the furthest x and y distance from the centre of the circle
				unsigned int mNumPoints = 0u; // the total number of points that the circle encompasses
		};
	
	public :
		Body(const unsigned int& type = 0u);
		
		void AddPolygon(Polygon polygon); // add a collision shape to the body
		const std::vector<Polygon>& GetPolygons() const;
		
		std::pair<glm::vec2, float> GetBoundingCircle() const; // return the centre and radius of the circle 
		
		std::string GetTag() const;
		void SetTag(const std::string& tag);
		
		void SetType(const unsigned int& type);
		unsigned int GetType() const;
		bool IsStatic() const;
		bool IsDynamic() const;
		
		glm::vec2 GetPosition() const;
		void SetPosition(const glm::vec2& pos); // set the position of the body (and all collision shapes)
		
		glm::vec2 GetVelocity() const;
		void SetVelocity(const glm::vec2& velocity);
		
		float GetInvMass() const;
		void SetMass(const float& mass);
		
		float GetRestitution() const;
		void SetRestitution(const float& res);
	
	private :
		std::vector<Polygon> mPolygons; // collision shapes representing the body for collision detection
		BoundingCircle mBoundingCircle; // the bouding circle that encompasses all collision shapes (used for early out detection)
		
		std::string mTag = ""; // the tag used in identify the body in custom callbacks
		unsigned int mType = 0u; // the type of body (0u - static, 1u - dynamic)
		
		glm::vec2 mPosition;
		glm::vec2 mVelocity;
		float mInvMass = 1.0f;
		float mRestitution = 0.5f;
};
}

#endif
