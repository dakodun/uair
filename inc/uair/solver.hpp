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

#ifndef UAIRSOLVER_HPP
#define UAIRSOLVER_HPP

#include <list>
#include <map>

#include "body.hpp"

namespace uair {
class EXPORTDLL Solver {
	public :
		// a handle to a body that holds a pointer to the body and an iterator into a list
		class EXPORTDLL BodyHandle {
			friend class Solver; // allow solver to modify pointer and iterator values
			
			public :
				Body& GetBody();
				
				// body helper functions
					void AddPolygon(Polygon polygon);
					const std::vector<Polygon>& GetPolygons() const;
					
					std::pair<glm::vec2, float> GetBoundingCircle() const;
					
					std::string GetTag() const;
					void SetTag(const std::string& tag);
					
					void SetType(const unsigned int& type);
					unsigned int GetType() const;
					bool IsStatic() const;
					bool IsDynamic() const;
					
					glm::vec2 GetPosition() const;
					void SetPosition(const glm::vec2& pos);
					
					glm::vec2 GetVelocity() const;
					void SetVelocity(const glm::vec2& velocity);
					
					float GetInvMass() const;
					void SetMass(const float& mass);
					
					float GetRestitution() const;
					void SetRestitution(const float& res);
				//
			
			private :
				std::list<Body>::iterator mBodyIter;
		};
	
	public :
		void Step(float deltaTime);
		
		// create the requested type of body, add it to the list and return a handle (pointer and iterator)
		BodyHandle CreateStaticBody();
		BodyHandle CreateDynamicBody();
		
		void DestroyBody(BodyHandle& handle); // remove a body using its handle
		
		bool AddCallback(const std::string& tagFirst, const std::string& tagSecond,
				const std::function<void(Body& first, Body& second)>& callback = nullptr);
	private :
		bool HandleBodyCollision(Body& first, Body& second);
		std::tuple<bool, glm::vec2, float> CheckPolygonCollision(const Polygon& first, const Polygon& second);
		void ApplyImpulse(Body& first, Body& second, const glm::vec2& collNorm);
	
	private :
		std::list<Body> mStaticBodies;
		std::list<Body> mDynamicBodies;
		
		std::map< std::pair<std::string, std::string>,
				std::function<void(Body& first, Body& second)> > mCallbacks;
};
}

#endif
