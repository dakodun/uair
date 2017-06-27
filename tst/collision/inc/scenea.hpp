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

#ifndef SCENEA_HPP
#define SCENEA_HPP

#include "init.hpp"
#include "staticgeometry.hpp"
#include "dynamicobject.hpp"

class SceneA : public uair::Scene {
	public :
		struct DynamicShape {
			public :
				DynamicShape(const std::u16string& name, const std::vector<uair::Polygon>& polygons,
						const glm::vec2& offset) : mName(name), mPolygons(polygons), mOffset(offset) {
					
				}
			
			public :
				std::u16string mName = u"";
				std::vector<uair::Polygon> mPolygons;
				glm::vec2 mOffset;
		};
	
	public :
		SceneA(std::string name);
		
		void HandleMessageQueue(const uair::MessageQueue::Entry& e);
		void Input();
		void Process(float deltaTime);
		void PostProcess(const unsigned int& processed, float deltaTime);
		void Render(const unsigned int& pass);
		
		void OnEnter();
		void OnLeave();
		bool OnQuit();
		
		std::string GetTag();
	private :
		// create the four boundary walls
		void CreateStGeoBoundary(const float& width);
		void UpdateStaticBatch();
		
		// create store of possible dynamic collision shapes
		void SetUpDynamicShapes(const float& size);
		void UpdateDynamicBatch();
		
		// update dynamic strings
		void UpdateInstructionString();
		void UpdateSimulationString();
		
		// create a piece of static geometry (square, circle, star)
		void CreateStGeoExtra(const unsigned int& type, const float& size, const glm::vec2& pos);
		
		void CreateRoom(); // cycle through the room layouts
	
	public :
		bool mPersist = false;
	protected :
		std::string mName = "";
	private :
		// the collision detection and response
		uair::Solver mSolver;
		
		// static geometry (4 boundary walls and additional if present)
		uair::RenderBatch mStaticBatch;
		std::list<StaticGeometry> mStGeoBoundary;
		std::list<StaticGeometry> mStGeoExtra;
		unsigned int mStRestitution = 5u;
		float mStRestitutionF = 0.5f;
		
		// dynamic objects that have been spawned
		uair::RenderBatch mDynamicBatch;
		std::list<DynamicObject> mDynamicObjects;
		unsigned int mDynRestitution = 5u;
		float mDynRestitutionF = 0.5f;
		unsigned int mDynMass = 5u;
		unsigned int mDynMassF = 5.0f;
		
		// list of possible collision shapes for dynamic objects
		typedef std::list<DynamicShape> DynamicList;
		DynamicList mDynamicShapes;
		DynamicList::iterator mDynamicShapeIter;
		
		// control creation of dynamic objects
		bool mPlacingDynamic = false;
		glm::vec2 mDynamicStart;
		uair::Shape mLine;
		std::vector<uair::Shape> mDynamicOutlines;
		bool mUpdateLine = false;
		float mTerminalVelocity = 480.0f;
		float mTerminalVelocitySq = mTerminalVelocity * mTerminalVelocity;
		
		// strings to display feedback and usage instructions
		uair::ResourceHandle mFontHandle;
		uair::RenderString mVelocityString;
		uair::RenderString mInstructionString;
		uair::RenderString mInstructionLowString;
		uair::RenderString mSimulationString;
		bool mShowHelp = false;
		
		// room layout counter
		unsigned int mRoom = 0u;
		
		// measure the performance of the collision detection and resolution process
		uair::Timer mTimer;
		std::priority_queue<float> mFrameTimes;
		float mAvgFrame = 0.0f;
		float mHighFrame = std::numeric_limits<float>::lowest();
};

#endif
