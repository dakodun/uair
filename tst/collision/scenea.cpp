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

#include "scenea.hpp"

SceneA::SceneA(std::string name) : Scene(name) {
	// create the ever-present boundary walls
	CreateStGeoBoundary(24.0f);
	
	// initialise the list of collision shapes for dynamic objects
	SetUpDynamicShapes(36.0f);
	
	// load the font and initialise the strings
		mFontHandle = GAME.AddResource<uair::Font>("nokiafc22");
		uair::Font& fnt = GAME.GetResource<uair::Font>(mFontHandle);
		fnt.LoadFromFile("../res/sys/nokiafc22.ttf", 8u);
		fnt.LoadGlyphs(uair::charset::BASICLATIN);
		
		mVelocityString.SetFont(&fnt);
		mVelocityString.SetSize(8u);
		mVelocityString.SetDepth(-1.0f);
		
		mInstructionString.SetFont(&fnt);
		mInstructionString.SetSize(8u);
		mInstructionString.SetDepth(-1.0f);
		mInstructionString.SetPosition(glm::vec2(2.0f, 10.0f));
		
		mInstructionLowString.SetFont(&fnt);
		mInstructionLowString.SetSize(8u);
		mInstructionLowString.SetDepth(-1.0f);
		mInstructionLowString.SetPosition(glm::vec2(2.0f, GAME.GetWindow()->GetHeight() - 14.0f));
		mInstructionLowString.SetText(u"left mouse and drag: create dynamic shape\nright mouse: destroy selected shape");
		
		mSimulationString.SetFont(&fnt);
		mSimulationString.SetSize(8u);
		mSimulationString.SetDepth(-1.0f);
		mSimulationString.SetPosition(glm::vec2(GAME.GetWindow()->GetWidth() - 156.0f, 10.0f));
	//
	
	// update the dynamic strings initially
	UpdateInstructionString();
	UpdateSimulationString();
	
	// update the static render batch initially
	UpdateStaticBatch();
}

void SceneA::HandleMessageQueue(const uair::MessageQueue::Entry& e) {
	using namespace uair::WindowMessage;
	
	switch (e.GetTypeID()) {
		case MouseMoveMessage::GetTypeID() : // if the mouse moved since last frame...
			if (GAME.GetMouseDown(uair::Mouse::Left)) { // if the left mouse button is down...
				mUpdateLine = true; // indicate we need to redraw the "object creation" line
			}
			
			break;
		default :
			break;
	}
}

void SceneA::Input() {
	using namespace uair::util;
	
	if (GAME.GetKeyboardPressed(uair::Keyboard::Num1)) {
		mShowHelp = !mShowHelp;
		UpdateInstructionString();
	}
	
	if (GAME.GetKeyboardPressed(uair::Keyboard::Q)) {
		
	}
	
	if (GAME.GetKeyboardPressed(uair::Keyboard::W)) {
		CreateRoom(); // change the room layout and reset the simulation
	}
	
	if (GAME.GetKeyboardPressed(uair::Keyboard::E)) {
		// advance the current dynamic object collision shape iter
		mDynamicShapeIter = std::next(mDynamicShapeIter);
		if (mDynamicShapeIter == mDynamicShapes.end()) {
			mDynamicShapeIter = mDynamicShapes.begin();
		}
		
		// if we're currently in the process of creating a dynamic object (left mouse down)
		if (mPlacingDynamic) {
			mDynamicOutlines.clear();
			for (const uair::Polygon& polygon: mDynamicShapeIter->mPolygons) {
				// set up the outline of the new shape we are about to create
				uair::Shape dynamicOutline;
				dynamicOutline.AddContours(polygon.GetContours());
				dynamicOutline.SetDepth(-1.0f);
				dynamicOutline.SetColour(glm::vec3(1.0f, 1.0f, 1.0f));
				dynamicOutline.SetPosition(mDynamicStart);
				dynamicOutline.SetOrigin(mDynamicShapeIter->mOffset);
				dynamicOutline.mRenderMode = GL_LINE_LOOP;
				
				mDynamicOutlines.push_back(std::move(dynamicOutline));
			}
			
			mUpdateLine = true;
		}
		
		UpdateInstructionString();
	}
	
	if (GAME.GetKeyboardPressed(uair::Keyboard::A)) {
		// change the restitution of all static geometry and update them
		if (mStRestitution > 0) {
			--mStRestitution;
			mStRestitutionF = static_cast<float>(mStRestitution) / 10u;
			
			for (StaticGeometry& staticGeometry: mStGeoBoundary) {
				staticGeometry.mBody.SetRestitution(mStRestitutionF);
			}
			
			for (StaticGeometry& staticGeometry: mStGeoExtra) {
				staticGeometry.mBody.SetRestitution(mStRestitutionF);
			}
			
			UpdateInstructionString();
		}
	}
	
	if (GAME.GetKeyboardPressed(uair::Keyboard::S)) {
		if (mStRestitution < 10u) {
			++mStRestitution;
			mStRestitutionF = static_cast<float>(mStRestitution) / 10u;
			
			for (StaticGeometry& staticGeometry: mStGeoBoundary) {
				staticGeometry.mBody.SetRestitution(mStRestitutionF);
			}
			
			for (StaticGeometry& staticGeometry: mStGeoExtra) {
				staticGeometry.mBody.SetRestitution(mStRestitutionF);
			}
			
			UpdateInstructionString();
		}
	}
	
	if (GAME.GetKeyboardPressed(uair::Keyboard::Z)) {
		// change the restitution of all future dynamic objects
		if (mDynRestitution > 0u) {
			--mDynRestitution;
			mDynRestitutionF = static_cast<float>(mDynRestitution) / 10u;
			
			UpdateInstructionString();
		}
	}
	
	if (GAME.GetKeyboardPressed(uair::Keyboard::X)) {
		if (mDynRestitution < 10u) {
			++mDynRestitution;
			mDynRestitutionF = static_cast<float>(mDynRestitution) / 10u;
			
			UpdateInstructionString();
		}
	}
	
	if (GAME.GetKeyboardPressed(uair::Keyboard::C)) {
		// change the mass of all future dynamic objects
		if (mDynMass >= 5u) {
			mDynMass -= 5u;
			mDynMassF = static_cast<float>(mDynMass);
			
			UpdateInstructionString();
		}
	}
	
	if (GAME.GetKeyboardPressed(uair::Keyboard::V)) {
		if (mDynMass <= 95u) {
			mDynMass += 5u;
			mDynMassF = static_cast<float>(mDynMass);
			
			UpdateInstructionString();
		}
	}
	
	if (GAME.GetMousePressed(uair::Mouse::Left)) {
		mDynamicStart = GAME.GetMouseCoords();
		
		mDynamicOutlines.clear();
		for (const uair::Polygon& polygon: mDynamicShapeIter->mPolygons) {
			// set up the outline of the new shape we are about to create
			uair::Shape dynamicOutline;
			dynamicOutline.AddContours(polygon.GetContours());
			dynamicOutline.SetDepth(-1.0f);
			dynamicOutline.SetColour(glm::vec3(1.0f, 1.0f, 1.0f));
			dynamicOutline.SetPosition(mDynamicStart);
			dynamicOutline.SetOrigin(mDynamicShapeIter->mOffset);
			dynamicOutline.mRenderMode = GL_LINE_LOOP;
			
			mDynamicOutlines.push_back(std::move(dynamicOutline));
		}
		
		mPlacingDynamic = true;
		mUpdateLine = true;
	}
	
	if (GAME.GetMouseReleased(uair::Mouse::Left)) {
		if (mPlacingDynamic) {
			glm::vec2 dynamicEnd = GAME.GetMouseCoords();
			glm::vec2 velocity = dynamicEnd - mDynamicStart;
			
			// caluclate the magnitude (squared) of the velocity
			float magSq = (velocity.x * velocity.x) + (velocity.y * velocity.y);
			if (CompareFloats(magSq, GreaterThan, mTerminalVelocitySq)) { // if the velocity is too much...
				float mag = std::sqrt(magSq); // find the magnitude
				velocity.x /= mag; velocity.y /= mag; // normalise velocity
				mag = mTerminalVelocity; // lower the magnitude to the max velocity
				velocity.x *= mag; velocity.y *= mag; // apply the magnitude to the normal (distance)
			}
			
			// create the new dynamic object and update restitution and mass
			mDynamicObjects.emplace_back(&mSolver, mDynamicShapeIter->mPolygons,
					mDynamicStart - mDynamicShapeIter->mOffset, velocity);
			mDynamicObjects.back().mBody.SetRestitution(mDynRestitutionF);
			mDynamicObjects.back().mBody.SetMass(mDynMassF);
			
			// intialise the dynamic object's property string
			uair::Font& fnt = GAME.GetResource<uair::Font>(mFontHandle);
			mDynamicObjects.back().mInfoString.SetFont(&fnt);
			mDynamicObjects.back().mInfoString.SetSize(8u);
			mDynamicObjects.back().mInfoString.SetDepth(-1.0f);
			mDynamicObjects.back().mInfoString.SetOrigin((-mDynamicShapeIter->mOffset) - glm::vec2(6.0f, 4.0f));
			
			UpdateSimulationString();
			mPlacingDynamic = false;
		}
	}
	
	if (GAME.GetMousePressed(uair::Mouse::Right)) {
		glm::vec2 mouseCoords = GAME.GetMouseCoords();
		
		auto dynObjIter = mDynamicObjects.begin();
		while (dynObjIter != mDynamicObjects.end()) { // for all exisiting dynamic objects...
			// get the bounding circle of the current dynamic object
			std::pair<glm::vec2, float> circle = dynObjIter->mBody.GetBoundingCircle();
			circle.first += dynObjIter->mBody.GetPosition();
			
			// if the cursor is inside the bounding circle...
			if ((mouseCoords.x > circle.first.x - circle.second && mouseCoords.x < circle.first.x + circle.second) &&
					(mouseCoords.y > circle.first.y - circle.second && mouseCoords.y < circle.first.y + circle.second)) {
				
				// remove the object and exit
				mDynamicObjects.erase(dynObjIter);
				dynObjIter = mDynamicObjects.end();
			}
			else {
				++dynObjIter;
			}
		}
		
		UpdateSimulationString();
	}
}

void SceneA::Process(float deltaTime) {
	using namespace uair::util;
	
	mTimer.Reset();
	
	mSolver.Step(deltaTime);
	
	mFrameTimes.push(mTimer.GetElapsedTime()); // add the time take to process bodies to the total
	if (mFrameTimes.size() > 10) { // if we've stored enough times
		// reset the current average process time and remove the 2 longest times
		mAvgFrame = 0.0f;
		mFrameTimes.pop();
		mFrameTimes.pop();
		
		for (unsigned i = 0u; i < 6u; ++i) { // for all other times except the 2 shortest...
			// add to the current sum time and update the longest
			float currTime = mFrameTimes.top();
			mAvgFrame += currTime;
			mHighFrame = std::max(mHighFrame, currTime);
			mFrameTimes.pop();
		}
		
		// find the average time from the sum and clear the reamaining queue
		mAvgFrame /= 6u;
		mFrameTimes = std::priority_queue<float>();
		UpdateSimulationString();
	}
	
	if (mUpdateLine) { // if the "object creation" has changed...
		// update the dynamic object's velocity line
		uair::Shape line;
		line.AddContour(uair::Contour({mDynamicStart, GAME.GetMouseCoords()}));
		line.SetDepth(-1.0f);
		line.SetColour(glm::vec3(1.0f, 1.0f, 1.0f));
		line.mRenderMode = GL_LINES;
		std::swap(line, mLine);
		
		// calculate the magnitude of the velocity vector
		glm::vec2 dynamicEnd = GAME.GetMouseCoords();
		glm::vec2 velocity = dynamicEnd - mDynamicStart;
		float mag = std::sqrt((velocity.x * velocity.x) + (velocity.y * velocity.y));
		mag = std::min(mag, mTerminalVelocity);
		
		// update the velocity string
		std::u16string string;
		string += UTF8toUTF16(ToString(mag, 2));
		mVelocityString.SetPosition(glm::vec2(dynamicEnd.x - 16.0f, dynamicEnd.y - 2.0f));
		mVelocityString.SetText(string);
		
		mUpdateLine = false;
	}
	
	UpdateDynamicBatch();
}

void SceneA::PostProcess(const unsigned int& processed, float deltaTime) {
	
}

void SceneA::Render(const unsigned int& pass) {
	uair::OpenGLStates::mViewMatrix = glm::mat4(1.0f);
	uair::OpenGLStates::mModelMatrix = glm::mat4(1.0f);
	
	uair::OpenGLStates::BindFBO(0);
	glViewport(0, 0, GAME.GetWindow()->GetWidth(), GAME.GetWindow()->GetHeight());
	uair::OpenGLStates::mProjectionMatrix = glm::ortho(0.0f, static_cast<float>(GAME.GetWindow()->GetWidth()),
			static_cast<float>(GAME.GetWindow()->GetHeight()), 0.0f, 0.01f, 1000.0f);
	
	if (pass == 0u) {
		glClearColor(0.38f, 0.42f, 0.48f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	
	mStaticBatch.Draw();
	mDynamicBatch.Draw();
}

void SceneA::OnEnter() {
	
}

void SceneA::OnLeave() {
	
}

bool SceneA::OnQuit() {
	return true;
}

std::string SceneA::GetTag() {
	return "SceneA";
}

void SceneA::CreateStGeoBoundary(const float& width) {
	// get the dimensions of the window
	glm::vec2 window(GAME.GetWindow()->GetWidth(), GAME.GetWindow()->GetHeight());
	
	// create the four sides of the boundary box
	mStGeoBoundary.emplace_back(&mSolver, std::vector<glm::vec2>{
					glm::vec2(0.0f, 0.0f), glm::vec2(window.x, 0.0f),
					glm::vec2(window.x, width), glm::vec2(0.0f, width)
			}, glm::vec2(0.0f, 0.0f));
	mStGeoBoundary.back().mBody.SetRestitution(mStRestitutionF);
	
	mStGeoBoundary.emplace_back(&mSolver, std::vector<glm::vec2>{
				glm::vec2(0.0f, 0.0f), glm::vec2(width, 0.0f),
				glm::vec2(width, window.y - (width * 2u)), glm::vec2(0.0f, window.y - (width * 2u))
			}, glm::vec2(window.x - width, width));
	mStGeoBoundary.back().mBody.SetRestitution(mStRestitutionF);
	
	mStGeoBoundary.emplace_back(&mSolver, std::vector<glm::vec2>{
					glm::vec2(0.0f, 0.0f), glm::vec2(window.x, 0.0f),
					glm::vec2(window.x, width), glm::vec2(0.0f, width)
			}, glm::vec2(0.0f, window.y - width));
	mStGeoBoundary.back().mBody.SetRestitution(mStRestitutionF);
	
	mStGeoBoundary.emplace_back(&mSolver, std::vector<glm::vec2>{
					glm::vec2(0.0f, 0.0f), glm::vec2(width, 0.0f),
					glm::vec2(width, window.y - (width * 2u)), glm::vec2(0.0f, window.y - (width * 2u))
			}, glm::vec2(0.0f, width));
	mStGeoBoundary.back().mBody.SetRestitution(mStRestitutionF);
}

void SceneA::UpdateStaticBatch() {
	// add the boundary walls to the batch
	for (StaticGeometry& staticGeometry : mStGeoBoundary) {
		mStaticBatch.Add(staticGeometry.mShape);
	}
	
	// add the extra geometry to the batch
	for (StaticGeometry& staticGeometry : mStGeoExtra) {
		mStaticBatch.Add(staticGeometry.mShape);
	}
	
	// add the (static) instructions string to the batch
	mStaticBatch.Add(mInstructionLowString);
	
	mStaticBatch.Upload();
}

void SceneA::SetUpDynamicShapes(const float& size) {
	float halfSize = size / 2u;
	
	{ // create a square and add it the the possible dynamic object collision shapes
		uair::Contour contour({glm::vec2(0.0f, 0.0f), glm::vec2(size, 0.0f),
				glm::vec2(size, size), glm::vec2(0.0f, size)});
		
		uair::Polygon polygon;
		polygon.AddContour(contour);
		
		mDynamicShapes.emplace_back(u"square", std::vector<uair::Polygon>{std::move(polygon)}, glm::vec2(halfSize, halfSize));
	}
	
	{ // small circle
		uair::Contour contour;
		for (unsigned int i = 0u; i < 360u; i += 20u) {
			float angle = i * uair::util::PIOVER180;
			contour.AddPoint(glm::vec2(cos(angle) * halfSize, sin(angle) * halfSize));
		}
		
		uair::Polygon polygon;
		polygon.AddContour(contour);
		
		mDynamicShapes.emplace_back(u"circle (s)", std::vector<uair::Polygon>{std::move(polygon)}, glm::vec2(0.0f, 0.0f));
	}
	
	{ // larger circle
		uair::Contour contour;
		for (unsigned int i = 0u; i < 360u; i += 20u) {
			float angle = i * uair::util::PIOVER180;
			contour.AddPoint(glm::vec2(cos(angle) * size, sin(angle) * size));
		}
		
		uair::Polygon polygon;
		polygon.AddContour(contour);
		
		mDynamicShapes.emplace_back(u"circle (l)", std::vector<uair::Polygon>{std::move(polygon)}, glm::vec2(0.0f, 0.0f));
	}
	
	{ // create a composite of 2 small circles and a square (pill shape)
		uair::Contour contourTop, contourBottom;
		for (unsigned int i = 0u; i < 360u; i += 20u) {
			float angle = i * uair::util::PIOVER180;
			contourTop.AddPoint(glm::vec2((cos(angle) * halfSize) + halfSize, sin(angle) * halfSize));
			contourBottom.AddPoint(glm::vec2((cos(angle) * halfSize) + halfSize, (sin(angle) * halfSize) + size));
		}
		
		uair::Contour contourMid({glm::vec2(0.0f, 0.0f), glm::vec2(size, 0.0f),
				glm::vec2(size, size), glm::vec2(0.0f, size)});
		
		uair::Polygon polygonTop, polygonMid, polygonBottom;
		polygonTop.AddContour(contourTop);
		polygonMid.AddContour(contourMid);
		polygonBottom.AddContour(contourBottom);
		
		mDynamicShapes.emplace_back(u"composite", std::vector<uair::Polygon>{std::move(polygonTop), std::move(polygonMid),
				std::move(polygonBottom)}, glm::vec2(halfSize, halfSize));
	}
	
	mDynamicShapeIter = mDynamicShapes.begin();
}

void SceneA::UpdateDynamicBatch() {
	mDynamicBatch.Clear();
	
	// add the (dynamic) strings to the batch
	mDynamicBatch.Add(mInstructionString);
	
	if (mShowHelp) {
		mDynamicBatch.Add(mSimulationString);
	}
	
	if (mPlacingDynamic) { // if we're creating a dynamic object...
		// add the "object creation" feedback elements
		mDynamicBatch.Add(mLine);
		mDynamicBatch.Add(mVelocityString);
		
		for (uair::Shape& dynamicOutline: mDynamicOutlines) {
			mDynamicBatch.Add(dynamicOutline);
		}
	}
	
	for (DynamicObject& dynamicObject : mDynamicObjects) { // for all dynamic objects...
		for (uair::Shape& dynamicObjectShape: dynamicObject.mShapes) { // for all shapes in current dynamic object...
			dynamicObjectShape.SetPosition(dynamicObject.mBody.GetPosition()); // update the position to match the body's position
			mDynamicBatch.Add(dynamicObjectShape); // add the shape to the dynamic render batch
		}
		
		if (mShowHelp) {
			// add the dynamic object's bounding circle
			dynamicObject.mBCircle.SetPosition(dynamicObject.mBody.GetPosition());
			mDynamicBatch.Add(dynamicObject.mBCircle);
			
			// add the dynamic object's "properties" string
			dynamicObject.mInfoString.SetPosition(dynamicObject.mBody.GetPosition());
			dynamicObject.UpdateString();
			mDynamicBatch.Add(dynamicObject.mInfoString);
		}
	}
	
	mDynamicBatch.Upload();
}

void SceneA::UpdateInstructionString() {
	using namespace uair::util;
	
	std::u16string string;
	string += u"(1) - toggle debug";
	
	if (mShowHelp) {
		string += u"\n";
		string += u"(Q) - current scene: ";
		string += u"detection & response";
		string += u'\n';
		
		string += u"(W) - current room: ";
		string += UTF8toUTF16(ToString(mRoom));
		string += u"\n";
		
		string += u"(E) - current shape: ";
		string += mDynamicShapeIter->mName;
		string += u'\n';
		
		
		string += u"(A / S) - static restitution: ";
		string += UTF8toUTF16(ToString(mStRestitutionF, 1));
		string += u"\n";
		
		string += u"(Z / X) - dynamic restitution: ";
		string += UTF8toUTF16(ToString(mDynRestitutionF, 1));
		string += u"\n";
		
		string += u"(C / V) - dynamic mass: ";
		string += UTF8toUTF16(ToString(mDynMass, 0));
		string += u"\n";
	}
	
	mInstructionString.SetText(string);
}

void SceneA::UpdateSimulationString() {
	using namespace uair::util;
	
	std::u16string string;
	string += u"current process time: ";
	string += UTF8toUTF16(ToString(mAvgFrame, 6));
	string += u'\n';
	
	string += u"longest process time: ";
	string += UTF8toUTF16(ToString(mHighFrame, 6));
	string += u'\n';
	
	string += u"dynamic object count: ";
	string += UTF8toUTF16(ToString(mDynamicObjects.size()));
	string += u'\n';
	
	mSimulationString.SetText(string);
}

void SceneA::CreateStGeoExtra(const unsigned int& type, const float& size, const glm::vec2& pos) {
	float halfSize = size / 2u;
	
	switch (type) {
		default :
		case 0u : // central square
			mStGeoExtra.emplace_back(&mSolver, std::vector<glm::vec2>{
					glm::vec2(0.0f, 0.0f), glm::vec2(size, 0.0f),
					glm::vec2(size, size), glm::vec2(0.0f, size)
			}, pos);
			mStGeoExtra.back().mBody.SetRestitution(mStRestitutionF);
			
			break;
		case 1u : { // central circle
			uair::Contour contour;
			for (unsigned int i = 0u; i < 360u; i += 20u) {
				float angle = i * uair::util::PIOVER180;
				contour.AddPoint(glm::vec2(cos(angle) * halfSize, sin(angle) * halfSize));
			}
			
			mStGeoExtra.emplace_back(&mSolver, contour.GetPoints(), pos);
			mStGeoExtra.back().mBody.SetRestitution(mStRestitutionF);
			
			break;
		}
		case 2u : { // central star
			float fifthSize = size / 5u;
			
			// central box
			mStGeoExtra.emplace_back(&mSolver, std::vector<glm::vec2>{
					glm::vec2(-fifthSize, -fifthSize),
					glm::vec2(fifthSize, -fifthSize),
					glm::vec2(fifthSize, fifthSize),
					glm::vec2(-fifthSize, fifthSize)
			}, pos);
			mStGeoExtra.back().mBody.SetRestitution(mStRestitutionF);
			
			// top arrow
			mStGeoExtra.emplace_back(&mSolver, std::vector<glm::vec2>{
					glm::vec2(0.0f, -halfSize),
					glm::vec2(fifthSize, -fifthSize),
					glm::vec2(-fifthSize, -fifthSize)
			}, pos);
			mStGeoExtra.back().mBody.SetRestitution(mStRestitutionF);
			
			// right arrow
			mStGeoExtra.emplace_back(&mSolver, std::vector<glm::vec2>{
					glm::vec2(fifthSize, -fifthSize),
					glm::vec2(halfSize, 0.0f),
					glm::vec2(fifthSize, fifthSize)
			}, pos);
			mStGeoExtra.back().mBody.SetRestitution(mStRestitutionF);
			
			// bottom arrow
			mStGeoExtra.emplace_back(&mSolver, std::vector<glm::vec2>{
					glm::vec2(fifthSize, fifthSize),
					glm::vec2(0.0f, halfSize),
					glm::vec2(-fifthSize, fifthSize)
			}, pos);
			mStGeoExtra.back().mBody.SetRestitution(mStRestitutionF);
			
			// left arrow
			mStGeoExtra.emplace_back(&mSolver, std::vector<glm::vec2>{
					glm::vec2(-fifthSize, -fifthSize),
					glm::vec2(-fifthSize, fifthSize),
					glm::vec2(-halfSize, 0.0f)
			}, pos);
			mStGeoExtra.back().mBody.SetRestitution(mStRestitutionF);
			
			break;
		}
	}
}

void SceneA::CreateRoom() {
	// clear the all current dynamic objects and exit "object creation" mode (if necessary)
	mDynamicObjects.clear();
	mUpdateLine = false;
	mPlacingDynamic = false;
	
	// advance the room layout counter and update strings to indicate new clean simulation state
	mRoom = (mRoom + 1u) % 8u;
	UpdateInstructionString();
	UpdateSimulationString();
	UpdateDynamicBatch();
	
	// clear any existing extra static geometry
	mStGeoExtra.clear();
	
	{
		glm::vec2 window(GAME.GetWindow()->GetWidth(), GAME.GetWindow()->GetHeight());
		float size = 120.0f;
		
		switch (mRoom) {
			default :
			case 0u : // empty room
				break;
			case 1u : // central square
				// create the extra static geometry needed for this layout
				CreateStGeoExtra(0u, 120.0f, glm::vec2((window.x / 2u) - (size / 2u), (window.y / 2u) - (size / 2u)));
				
				break;
			case 2u : // central circle
				CreateStGeoExtra(1u, 120.0f, glm::vec2(window.x / 2u, window.y / 2u));
				
				break;
			case 3u : // central star
				CreateStGeoExtra(2u, 120.0f, glm::vec2(window.x / 2u, window.y / 2u));
				
				break;
			case 4u : // squares
				CreateStGeoExtra(0u, 80.0f, glm::vec2(window.x - 160.0f, 90.0f));
				CreateStGeoExtra(0u, 135.0f, glm::vec2(90.0f, 185.0f));
				CreateStGeoExtra(0u, 200.0f, glm::vec2((window.x / 2u) + 40.0f, window.y - 224.0f));
				
				break;
			case 5u : // circles
				CreateStGeoExtra(1u, 60.0f, glm::vec2(124.0f, 124.0f));
				CreateStGeoExtra(1u, 60.0f, glm::vec2((window.x / 2u), 124.0f));
				CreateStGeoExtra(1u, 60.0f, glm::vec2(window.x - 124.0f, 124.0f));
				
				CreateStGeoExtra(1u, 60.0f, glm::vec2(124.0f, (window.y / 2u)));
				CreateStGeoExtra(1u, 60.0f, glm::vec2((window.x / 2u), (window.y / 2u)));
				CreateStGeoExtra(1u, 60.0f, glm::vec2(window.x - 124.0f, (window.y / 2u)));
				
				CreateStGeoExtra(1u, 60.0f, glm::vec2(124.0f, window.y - 124.0f));
				CreateStGeoExtra(1u, 60.0f, glm::vec2((window.x / 2u), window.y - 124.0f));
				CreateStGeoExtra(1u, 60.0f, glm::vec2(window.x - 124.0f, window.y - 124.0f));
				
				break;
			case 6u : // stars
				CreateStGeoExtra(2u, 145.0f, glm::vec2(180.0f, 180.0f));
				CreateStGeoExtra(2u, 145.0f, glm::vec2(window.x - 180.0f, 180.0f));
				CreateStGeoExtra(2u, 145.0f, glm::vec2(window.x - 180.0f, window.y - 180.0f));
				CreateStGeoExtra(2u, 145.0f, glm::vec2(180.0f, window.y - 180.0f));
				
				break;
			case 7u : // mixture of shapes
				CreateStGeoExtra(0u, 60.0f, glm::vec2(85.0f, 85.0f));
				CreateStGeoExtra(0u, 100.0f, glm::vec2(180.0f, 240.0f));
				
				CreateStGeoExtra(1u, 45.0f, glm::vec2(window.x - 130.0f, window.y - 200.0f));
				CreateStGeoExtra(1u, 55.0f, glm::vec2(window.x - 100.0f, window.y - 100.0f));
				CreateStGeoExtra(1u, 65.0f, glm::vec2(window.x - 210.0f, window.y - 140.0f));
				
				CreateStGeoExtra(2u, 165.0f, glm::vec2(window.x - 200.0f, 200.0f));
				
				break;
		}
	}
	
	UpdateStaticBatch();
}
