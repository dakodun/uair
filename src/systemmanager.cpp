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

#include "systemmanager.hpp"

#include "messagesystem.hpp"

namespace uair {
System::System(System&& other) : System() {
	swap(*this, other);
}

System::~System() {
	
}

System& System::operator=(System other) {
	swap(*this, other);
	
	return *this;
}

void swap(System& first, System& second) {
	std::swap(first.mRegisteredEntities, second.mRegisteredEntities);
}

void System::RegisterEntity(EntityManager::Handle handle) {
	mRegisteredEntities.push_back(std::move(handle));
}

void System::UnregisterEntity(EntityManager::Handle handle) {
	if (!mRegisteredEntities.empty()) {
		for (unsigned int i = mRegisteredEntities.size() - 1; i >= 0u; ++i) {
			if (mRegisteredEntities.at(i) == handle) {
				mRegisteredEntities.erase(mRegisteredEntities.begin() + i);
			}
		}
	}
}


SystemManager::SystemManager(SystemManager&& other) : SystemManager() {
	swap(*this, other);
}

SystemManager::~SystemManager() {
	for (auto iter = mStore.begin(); iter != mStore.end(); ++iter) { // for all registered systems...
		delete iter->second; // delete the resource store via the base pointer
	}
}

SystemManager& SystemManager::operator=(SystemManager other) {
	swap(*this, other);
	
	return *this;
}

void swap(SystemManager& first, SystemManager& second) {
	std::swap(first.mStore, second.mStore);
}
}
