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

#include "systemmanager.hpp"

#include "messagesystem.hpp"

namespace uair {
System::System(SystemManager* systemManager) : mSystemManagerPtr(systemManager) {
	
}

System::~System() {
	
}

void System::RegisterEntity(EntityManager::Handle handle) {
	mRegisteredEntities.push_back(std::move(handle));
}

EntityManager::Handle System::AddEntity(const std::string& entityName) {
	try {
		return mSystemManagerPtr->GetEntityManager().Add(entityName);
	} catch (std::exception& e) {
		throw;
	}
}

void System::RemoveEntity(const EntityManager::Handle& handle) {
	try {
		mSystemManagerPtr->GetEntityManager().Remove(handle);
	} catch (std::exception& e) {
		throw;
	}
}

Entity& System::GetEntity(const EntityManager::Handle& handle) {
	try {
		return mSystemManagerPtr->GetEntityManager().Get(handle);
	} catch (std::exception& e) {
		throw;
	}
}

void System::PushMessageString(const unsigned int& systemTypeID, const unsigned int& messageTypeID, const std::string& messageString) {
	mSystemManagerPtr->GetMessageSystem().PushMessageString(systemTypeID, messageTypeID, messageString);
}

unsigned int System::GetMessageCount() {
	return mSystemManagerPtr->GetMessageSystem().GetMessageCount();
}

unsigned int System::GetSystemType(const unsigned int& index) {
	try {
		return mSystemManagerPtr->GetMessageSystem().GetSystemType(index);
	} catch (std::exception& e) {
		throw;
	}
}

unsigned int System::GetMessageType(const unsigned int& index) {
	try {
		return mSystemManagerPtr->GetMessageSystem().GetMessageType(index);
	} catch (std::exception& e) {
		throw;
	}
}

int System::GetMessageState(const unsigned int& index) {
	try {
		return mSystemManagerPtr->GetMessageSystem().GetMessageState(index);
	} catch (std::exception& e) {
		throw;
	}
}

void System::PopMessage(const unsigned int& index) {
	mSystemManagerPtr->GetMessageSystem().PopMessage(index);
}


SystemManager::SystemManager(EntityManager& entityManager, Manager<Component>& componentManager, MessageSystem& messageSystem) :
		mEntityManager(entityManager), mComponentManager(componentManager), mMessageSystem(messageSystem) {
	
	
}

SystemManager::~SystemManager() {
	for (auto iter = mStore.begin(); iter != mStore.end(); ++iter) { // for all registered systems...
		delete iter->second;
	}
}

EntityManager& SystemManager::GetEntityManager() {
	return mEntityManager;
}

Manager<Component>& SystemManager::GetComponentManager() {
	return mComponentManager;
}

MessageSystem& SystemManager::GetMessageSystem() {
	return mMessageSystem;
}
}
