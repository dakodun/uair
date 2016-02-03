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

#include "entitymanager.hpp"

namespace uair {
Entity::Entity(EntityManager* entityManager, const unsigned int& entityID) : mEntityManagerPtr(entityManager), mEntityID(entityID) {
	
}

Entity::~Entity() {
	for (auto iter = mComponents.begin(); iter != mComponents.end(); ++iter) { // for all component attached to entity...
		try {
			mEntityManagerPtr->GetComponentManager().Remove(*iter); // invoke removal via the component manager
		} catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}
	}
}


EntityManager::EntityManager(Manager<Component>& componentManager) : mComponentManager(componentManager) {
	
}

EntityManager::Handle EntityManager::Add() {
	std::pair<unsigned int, unsigned int> indexCounterPair; // the index and counter (validity) of the new entity
	
	try {
		indexCounterPair = mStore.Add<EntityManager*>(this, mEntityCount); // add a new entry to the store and save the index and counter value returned
	} catch (std::exception& e) {
		throw;
	}
	
	++mEntityCount; // increment the entity count
	return Handle(indexCounterPair.first, indexCounterPair.second); // return a handle to the newly added resource
}

void EntityManager::Remove(const Handle& handle) {
	try {
		// remove the resource from the store using its index and counter (validity) value (using the base pointer)
		mStore.Remove(handle.mIndex, handle.mCounter);
	} catch (std::exception& e) {
		throw;
	}
}

Entity& EntityManager::Get(const Handle& handle) {
	try {
		return mStore.Get(handle.mIndex, handle.mCounter); // return a reference to the stored resource using its index and counter (validity) value
	} catch (std::exception& e) {
		throw;
	}
}

Manager<Component>& EntityManager::GetComponentManager() {
	return mComponentManager;
}
}
