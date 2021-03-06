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

#ifndef UAIRSCENE_HPP
#define UAIRSCENE_HPP

#include <string>
#include <memory>

#include "init.hpp"
#include "messagequeue.hpp"

namespace uair {
class EXPORTDLL Scene {
	public :
		Scene(std::string name) : mName(name) {
			
		}
		
		virtual ~Scene() = default;
		
		virtual void HandleMessageQueue(const MessageQueue::Entry& e) = 0;
		virtual void Input() = 0;
		virtual void Process(float deltaTime) = 0;
		virtual void PostProcess(const unsigned int& processed, float deltaTime) = 0;
		virtual void Render(const unsigned int& pass) = 0;
		
		virtual void OnEnter() = 0;
		virtual void OnLeave() = 0;
		virtual bool OnQuit() = 0;
		
		virtual std::string GetTag() = 0;
		
		std::string GetName() {
			return mName;
		}
		
		bool mPersist = false;
	protected :
		std::string mName = "";
};

typedef std::shared_ptr<Scene> ScenePtr;
}

#endif
