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
	// create the layout
	mContainerBase.FromProperties({glm::ivec2(20, 20),
			GAME.GetWindow()->GetWidth() - 40u,
			GAME.GetWindow()->GetHeight() - 40u});
	
	// setting the reserve amount means no reallocation takes place which
	// means the references returned will remain valid
	mContainerBase.SetContainerReserve(5u);
		
		// 
		auto& topLeftBase = mContainerBase.GetContainer(
			mContainerBase.AddContainer(glm::ivec2(10, 10), 288u, 48u)
		);
			
			// 
			auto& topLeftDiv = topLeftBase.GetDivision(
				topLeftBase.AddDivision(glm::uvec2(6u, 1u))
			);
				
				// 
				auto& topLeftBarsBase = topLeftDiv.GetContainer(
					topLeftDiv.AddContainer(glm::ivec2(0, 9), 240u,
							30u, glm::uvec2(1u, 0u)), glm::uvec2(1u, 0u)
				);
					
					// 
					auto& topLeftBars = topLeftBarsBase.GetDivision(
						topLeftBarsBase.AddDivision(glm::uvec2(1u, 3u))
					);
		
		// 
		auto& middleBottomBase = mContainerBase.GetContainer(
			mContainerBase.AddContainer(glm::ivec2(140, 444), 480u, 48u)
		);
		
		// 
		auto& middleBottomDivBase = mContainerBase.GetContainer(
			mContainerBase.AddContainer(glm::ivec2(140, 502), 480u, 48u)
		);
			
			// 
			auto& middleBottomDivSquares = middleBottomDivBase.GetDivision(
				middleBottomDivBase.AddDivision(glm::uvec2(10u, 1u))
			);
		
		// 
		auto& topRight = mContainerBase.GetContainer(
			mContainerHandle = mContainerBase.AddContainer(
					glm::ivec2(GAME.GetWindow()->GetWidth() - 164, -20),
					144u, 144u)
		);
			
			// 
			auto& topRightDiv = topRight.GetDivision(
				mDivisionHandle = topRight.AddDivision(glm::uvec2(2u, 2u))
			);
		
		// 
		auto& splitBar = mContainerBase.GetContainer(
			mContainerBase.AddContainer(
					glm::ivec2(10, GAME.GetWindow()->GetHeight() - 402u),
					64u, 352u)
		);
			
			// 
			auto& splitBarHalf = splitBar.GetDivision(
				splitBar.AddDivision(glm::uvec2(2u, 1u))
			);
				
				// 
				auto& splitBarRight = splitBarHalf.GetDivision(
					splitBarHalf.AddDivision(glm::uvec2(1u, 11u),
							glm::uvec2(1u, 0u)), glm::uvec2(1u, 0u)
				);
	//
	
	auto shapes = mContainerBase.GetShapes();
	for (auto shape: shapes) {
		shape.SetDepth(-10.0f);
		mLinesBatch.Add(shape);
	}
	
	mLinesBatch.Upload();
	
	// initalise ui elements
		InitIcons(topLeftDiv, topRight, topRightDiv);
		
		InitResourceBars(topLeftBars);
		
		InitHotbarTop(middleBottomBase);
		InitHotbarBottom(middleBottomDivSquares);
		
		InitSplit(splitBarHalf, splitBarRight);
	//
	
	mUIBatch.Upload();
	
	// load the font and initialise the strings
		mFontHandle = GAME.AddResource<uair::Font>("nokiafc22");
		uair::Font& fnt = GAME.GetResource<uair::Font>(mFontHandle);
		fnt.LoadFromFile("../res/sys/nokiafc22.ttf", 8u);
		fnt.LoadGlyphs(uair::charset::BASICLATIN);
		
		mInstructionString.SetFont(&fnt);
		mInstructionString.SetSize(8u);
		mInstructionString.SetDepth(-1.0f);
		mInstructionString.SetPosition(glm::vec2(2.0f, 10.0f));
		
		mInstructionBack.AddContour(uair::Contour({glm::vec2(0.0f, 0.0f),
				glm::vec2(226.0f, 0.0f), glm::vec2(226.0f, 80.0f),
				glm::vec2(0.0f, 80.0f)}));
		mInstructionBack.SetColour(glm::vec3(0.0f, 0.0f, 0.0f));
		mInstructionBack.SetAlpha(0.75f);
		mInstructionBack.SetDepth(-2.0f);
		
		UpdateInstructionText();
	//
}

void SceneA::HandleMessageQueue(const uair::MessageQueue::Entry& e) {
	
}

void SceneA::Input() {
	// toggle layout lines
	if (GAME.GetKeyboardPressed(uair::Keyboard::L)) {
		mShowLines = !mShowLines;
	}
	
	// toggle ui elements
	if (GAME.GetKeyboardPressed(uair::Keyboard::U)) {
		mShowUI = !mShowUI;
	}
	
	// toggle instruction text
	if (GAME.GetKeyboardPressed(uair::Keyboard::F1)) {
		mShowInstruction = !mShowInstruction;
		UpdateInstructionText();
	}
	
	// toggle small top-left (floating) icon
	if (GAME.GetKeyboardPressed(uair::Keyboard::Q)) {
		mShowIconFloat = !mShowIconFloat;
	}
	
	// change alignment of top-right (small and large) icons
	if (GAME.GetKeyboardPressed(uair::Keyboard::W)) {
		auto& topRight = mContainerBase.GetContainer(mContainerHandle);
		auto& topRightDiv = topRight.GetDivision(mDivisionHandle);
		
		// cycle through horizontal and vertical alignments
		if (topRight.GetHorizontalAlign() == "left") {
			topRight.SetHorizontalAlign("centre");
			topRightDiv.SetHorizontalAlign("centre", glm::uvec2(0u, 0u));
		}
		else if (topRight.GetHorizontalAlign() == "centre") {
			topRight.SetHorizontalAlign("right");
			topRightDiv.SetHorizontalAlign("right", glm::uvec2(0u, 0u));
		}
		else {
			topRight.SetHorizontalAlign("left");
			topRightDiv.SetHorizontalAlign("left", glm::uvec2(0u, 0u));
			
			if (topRight.GetVerticalAlign() == "top") {
				topRight.SetVerticalAlign("middle");
				topRightDiv.SetVerticalAlign("middle", glm::uvec2(0u, 0u));
			}
			else if (topRight.GetVerticalAlign() == "middle") {
				topRight.SetVerticalAlign("bottom");
				topRightDiv.SetVerticalAlign("bottom", glm::uvec2(0u, 0u));
			}
			else {
				topRight.SetVerticalAlign("top");
				topRightDiv.SetVerticalAlign("top", glm::uvec2(0u, 0u));
			}
		}
		
		// update the large icon's position
		topRight.PositionElements({&(mIconAlignLarge.mElement)});
		mIconAlignLarge.mShape.SetPosition(mIconAlignLarge.mElement.GetPosition());
		
		// update the small icon's position
		topRightDiv.PositionElements({&(mIconAlignSmall.mElement)},
				glm::uvec2(0u, 0u));
		mIconAlignSmall.mShape.SetPosition(mIconAlignSmall.mElement.GetPosition());
	}
	
	// process input for ui elements that need it
		for (auto& bar: mResourceBars) {
			bar.Input();
		}
		
		for (auto& icon: mHotbarIconsTop) {
			icon.Input();
		}
		
		for (auto& icon: mHotbarIconsBottom) {
			icon.Input();
		}
	//
}

void SceneA::Process(float deltaTime) {
	// process ui elements and update the ui batch
		mUIBatch.Add(mIcon.mShape);
		
		if (mShowIconFloat) {
			mUIBatch.Add(mIconFloat.mShape);
		}
		
		mUIBatch.Add(mIconAlignLarge.mShape);
		mUIBatch.Add(mIconAlignSmall.mShape);
		
		for (auto& bar: mResourceBars) {
			bar.Process(deltaTime);
			mUIBatch.Add(bar.mShape);
		}
		
		for (auto& icon: mHotbarIconsTop) {
			icon.Process(deltaTime);
			mUIBatch.Add(icon.mShape);
		}
		
		for (auto& icon: mHotbarIconsBottom) {
			icon.Process(deltaTime);
			mUIBatch.Add(icon.mShape);
		}
		
		for (auto& icon: mSplitIcons) {
			mUIBatch.Add(icon.mShape);
		}
		
		mUIBatch.Upload();
	//
}

void SceneA::PostProcess(const unsigned int& processed, float deltaTime) {
	
}

void SceneA::Render(const unsigned int& pass) {
	uair::OpenGLStates::mViewMatrix = glm::mat4(1.0f);
	uair::OpenGLStates::mModelMatrix = glm::mat4(1.0f);
	
	uair::OpenGLStates::BindFBO(0);
	glViewport(0, 0, GAME.GetWindow()->GetWidth(),
			GAME.GetWindow()->GetHeight());
	uair::OpenGLStates::mProjectionMatrix = glm::ortho(0.0f,
			static_cast<float>(GAME.GetWindow()->GetWidth()),
			static_cast<float>(GAME.GetWindow()->GetHeight()),
			0.0f, 0.01f, 1000.0f);
	
	if (pass == 0u) {
		glClearColor(0.38f, 0.42f, 0.48f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	
	if (mShowUI) {
		mUIBatch.Draw(pass);
	}
	
	if (mShowLines) {
		mLinesBatch.Draw(pass);
	}
	
	mTextBatch.Draw(pass);
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

void SceneA::InitIcons(uair::LayoutDivision& division,
		uair::LayoutContainer& large, uair::LayoutDivision& small) {
	
	// create and position the large top-left icon
	Icon icon(48u, 48u, glm::vec3(0.15f, 0.15f, 0.15f));
	division.PositionElements({&(icon.mElement)}, glm::uvec2(0u, 0u));
	icon.mShape.SetPosition(icon.mElement.GetPosition());
	mUIBatch.Add(icon.mShape);
	mIcon = std::move(icon);
	
	// create and position the smaller (floating) top-left icon
	Icon iconFloat(14u, 14u, glm::vec3(0.95f, 0.95f, 0.95f));
	iconFloat.mElement.SetFloat(true);
	division.PositionElements({&(iconFloat.mElement)}, glm::uvec2(0u, 0u));
	iconFloat.mShape.SetPosition(iconFloat.mElement.GetPosition() +
			glm::ivec2(6, 6));
	mUIBatch.Add(iconFloat.mShape);
	mIconFloat = std::move(iconFloat);
	
	// create and position the large top-right icon
	Icon iconAlignLarge(64u, 64u, glm::vec3(0.15f, 0.85f, 0.45f));
	large.PositionElements({&(iconAlignLarge.mElement)});
	iconAlignLarge.mShape.SetPosition(iconAlignLarge.mElement.GetPosition());
	mUIBatch.Add(iconAlignLarge.mShape);
	mIconAlignLarge = std::move(iconAlignLarge);
	
	// create and position the small top-right icon
	Icon iconAlignSmall(24u, 24u, glm::vec3(0.35f, 0.15f, 0.85f));
	small.PositionElements({&(iconAlignSmall.mElement)}, glm::uvec2(0u, 0u));
	iconAlignSmall.mShape.SetPosition(iconAlignSmall.mElement.GetPosition());
	mUIBatch.Add(iconAlignSmall.mShape);
	mIconAlignSmall = std::move(iconAlignSmall);
}

void SceneA::InitResourceBars(uair::LayoutDivision& division) {
	// create and position the top resource bar
	ResourceBar barTop(glm::vec3(0.95f, 0.0f, 0.0f), uair::Keyboard::Z);
	division.PositionElements({&(barTop.mElement)}, glm::uvec2(0u, 0u));
	barTop.mShape.SetPosition(barTop.mElement.GetPosition());
	mUIBatch.Add(barTop.mShape);
	mResourceBars.push_back(std::move(barTop));
	
	// create and position the middle resource bar
	ResourceBar barMid(glm::vec3(0.0f, 0.74f, 0.0f), uair::Keyboard::X);
	division.PositionElements({&(barMid.mElement)}, glm::uvec2(0u, 1u));
	barMid.mShape.SetPosition(barMid.mElement.GetPosition());
	mUIBatch.Add(barMid.mShape);
	mResourceBars.push_back(std::move(barMid));
	
	// create and position the bottom resource bar
	ResourceBar barBot(glm::vec3(0.0f, 0.0f, 0.92f), uair::Keyboard::C);
	division.PositionElements({&(barBot.mElement)}, glm::uvec2(0u, 2u));
	barBot.mShape.SetPosition(barBot.mElement.GetPosition());
	mUIBatch.Add(barBot.mShape);
	mResourceBars.push_back(std::move(barBot));
}

void SceneA::InitHotbarTop(uair::LayoutContainer& container) {
	// create a vector of hotbar icon colours
	std::vector<glm::vec3> coloursTop = {
		glm::vec3(0.92f, 0.14f, 0.14f), glm::vec3(0.92f, 0.20f, 0.14f),
		glm::vec3(0.92f, 0.26f, 0.14f), glm::vec3(0.92f, 0.32f, 0.14f),
		glm::vec3(0.92f, 0.38f, 0.14f), glm::vec3(0.92f, 0.44f, 0.14f),
		glm::vec3(0.92f, 0.50f, 0.14f), glm::vec3(0.92f, 0.56f, 0.14f),
		glm::vec3(0.92f, 0.62f, 0.14f), glm::vec3(0.92f, 0.68f, 0.14f)
	};
	
	// create a vector of hotbar icon hotkeys
	std::vector<uair::Keyboard> keys = {
		uair::Keyboard::Num1, uair::Keyboard::Num2,
		uair::Keyboard::Num3, uair::Keyboard::Num4,
		uair::Keyboard::Num5, uair::Keyboard::Num6,
		uair::Keyboard::Num7, uair::Keyboard::Num8,
		uair::Keyboard::Num9, uair::Keyboard::Num0
	};
	
	// create the hotbar icons
	std::list<uair::LayoutElement*> elements;
	for (unsigned int i = 0u; i < keys.size(); ++i) {
		HotbarIcon icon(coloursTop.at(i), keys.at(i), true);
		
		mHotbarIconsTop.push_back(std::move(icon));
		elements.push_back(&(mHotbarIconsTop.back().mElement));
	}
	
	// retrieve the hotbar icons' position in the container
	container.PositionElements(elements);
	
	for (auto& icon: mHotbarIconsTop) {
		// position the hotbar icons and add them to the ui batch
		icon.mShape.SetPosition(icon.mElement.GetPosition());
		mUIBatch.Add(icon.mShape);
	}
}

void SceneA::InitHotbarBottom(uair::LayoutDivision& division) {
	std::vector<glm::vec3> coloursTop = {
		glm::vec3(0.27f, 0.78f, 0.87f), glm::vec3(0.27f, 0.72f, 0.87f),
		glm::vec3(0.27f, 0.66f, 0.87f), glm::vec3(0.27f, 0.60f, 0.87f),
		glm::vec3(0.27f, 0.54f, 0.87f), glm::vec3(0.27f, 0.48f, 0.87f),
		glm::vec3(0.27f, 0.42f, 0.87f), glm::vec3(0.27f, 0.36f, 0.87f),
		glm::vec3(0.27f, 0.30f, 0.87f), glm::vec3(0.27f, 0.24f, 0.87f)
	};
	
	std::vector<uair::Keyboard> keys = {
		uair::Keyboard::Num1, uair::Keyboard::Num2,
		uair::Keyboard::Num3, uair::Keyboard::Num4,
		uair::Keyboard::Num5, uair::Keyboard::Num6,
		uair::Keyboard::Num7, uair::Keyboard::Num8,
		uair::Keyboard::Num9, uair::Keyboard::Num0
	};
	
	// create the hotbar icons, retrieve their position in the cell,
	// position them and then add them to the ui batch
	for (unsigned int i = 0u; i < keys.size(); ++i) {
		HotbarIcon icon(coloursTop.at(i), keys.at(i));
		
		division.PositionElements({&(icon.mElement)}, glm::uvec2(i, 0u));
		
		icon.mShape.SetPosition(icon.mElement.GetPosition());
		mUIBatch.Add(icon.mShape);
		
		mHotbarIconsBottom.push_back(std::move(icon));
	}
}

void SceneA::InitSplit(uair::LayoutDivision& left,
		uair::LayoutDivision& right) {
	
	std::list<uair::LayoutElement*> elements;
	for (unsigned int i = 0u; i < 11u; ++i) {
		Icon icon(32u, 32u, glm::vec3(0.74f, 0.30f, 0.84f), true);
		
		mSplitIcons.push_back(std::move(icon));
		elements.push_back(&(mSplitIcons.back().mElement));
	}
	
	left.PositionElements(elements, glm::uvec2(0u, 0u));
	
	for (auto& icon: mSplitIcons) {
		icon.mShape.SetPosition(icon.mElement.GetPosition());
		mUIBatch.Add(icon.mShape);
	}
	
	for (unsigned int i = 0u; i < 11u; ++i) {
		Icon icon(32u, 32u, glm::vec3(0.74f, 0.30f, 0.84f), true);
		
		right.PositionElements({&(icon.mElement)}, glm::uvec2(0u, i));
		
		icon.mShape.SetPosition(icon.mElement.GetPosition());
		mUIBatch.Add(icon.mShape);
		
		mSplitIcons.push_back(std::move(icon));
	}
}

void SceneA::UpdateInstructionText() {
	std::u16string string;
	string += u"(F1) - toggle debug";
	
	if (mShowInstruction) {
		string += u"\n";
		string += u"(Q) - toggle top-left floating icon on or off";
		string += u'\n';
		
		string += u"(W) - cycle top-right icon alignment";
		string += u'\n';
		
		string += u"(0 - 9 & SHIFT + 0 - 9) - activate hotbar icons";
		string += u'\n';
		
		string += u"(Z, X & C) - reduce resource bars";
		string += u'\n';
		
		string += u"(U) - toggle ui display on or off";
		string += u'\n';
		
		string += u"(L) - toggle layout lines on or off";
		
		mTextBatch.Add(mInstructionBack);
	}
	
	mInstructionString.SetText(string);
	
	mTextBatch.Add(mInstructionString);
	mTextBatch.Upload();
}
