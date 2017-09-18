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
#include "uielements.hpp"

class SceneA : public uair::Scene {	
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
		// initialise regular icons (top-left and top-right)
		void InitIcons(uair::LayoutDivision& division,
				uair::LayoutContainer& large, uair::LayoutDivision& small);
		
		// initalise the resource bars (top-left)
		void InitResourceBars(uair::LayoutDivision& division);
		
		// initalise both hotbars (bottom-centre)
		void InitHotbarTop(uair::LayoutContainer& container);
		void InitHotbarBottom(uair::LayoutDivision& division);
		
		// initialise the split bar (bottom-left)
		void InitSplit(uair::LayoutDivision& left,
				uair::LayoutDivision& right);
		
		void UpdateInstructionText();
	
	public :
		bool mPersist = false;
	protected :
		std::string mName = "";
	private :
		// batches used to render layout lines, ui elements
		// and instruction text
		uair::RenderBatch mLinesBatch;
		uair::RenderBatch mUIBatch;
		uair::RenderBatch mTextBatch;
		
		// flags to show/hide layout lines, ui elements and
		// instruction text
		bool mShowLines = true;
		bool mShowUI = true;
		bool mShowInstruction = false;
		
		// the main container that all other layout objects are children
		// of and handles to some child objects to allow changing of
		// alignment
		uair::LayoutContainer mContainerBase;
		uair::ContainerHandle mContainerHandle;
		uair::DivisionHandle mDivisionHandle;
		
		Icon mIcon;
		Icon mIconFloat;
		Icon mIconAlignLarge;
		Icon mIconAlignSmall;
		
		// flag to show/hide top-left floating icon
		bool mShowIconFloat = true;
		
		std::list<ResourceBar> mResourceBars;
		
		std::list<HotbarIcon> mHotbarIconsTop;
		std::list<HotbarIcon> mHotbarIconsBottom;
		
		std::list<Icon> mSplitIcons;
		
		// onscreen usage instruction text
		uair::ResourceHandle mFontHandle;
		uair::RenderString mInstructionString;
		uair::Shape mInstructionBack;
};

#endif
