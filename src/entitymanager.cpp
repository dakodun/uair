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
Entity::Entity(EntityManager* entityManager, const unsigned int& entityID, const std::string& entityName) : mEntityManagerPtr(entityManager),
		mEntityID(entityID), mName(entityName) {
	
	
}

Entity::Entity(Entity&& other) : Entity(other.mEntityManagerPtr, 0u) {
	swap(*this, other);
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

Entity& Entity::operator=(Entity other) {
	swap(*this, other);
	
	return *this;
}

void swap(Entity& first, Entity& second) {
	std::swap(first.mEntityManagerPtr, second.mEntityManagerPtr);
	std::swap(first.mComponents, second.mComponents);
	std::swap(first.mEntityID, second.mEntityID);
	std::swap(first.mName, second.mName);
}

void Entity::RemoveAllComponents() {
	for (auto iter = mComponents.begin(); iter != mComponents.end(); ++iter) { // for all component attached to entity...
		try {
			mEntityManagerPtr->GetComponentManager().Remove(*iter); // invoke removal via the component manager
		} catch (std::exception& e) {
			throw;
		}
	}
}


std::vector<Manager<Component>::Handle> Entity::GetAllComponents() {
	return mComponents;
}

unsigned int Entity::GetEntityID() const {
	return mEntityID;
}

std::string Entity::GetName() const {
	return mName;
}


EntityManager::EntityManager(Manager<Component>& componentManager) : mComponentManager(componentManager) {
	
}

EntityManager::Handle EntityManager::Add(const std::string& name) {
	std::pair<unsigned int, unsigned int> indexCounterPair;
	
	try {
		// add a new entry to the store and save the index and counter value returned
		indexCounterPair = mStore.Add<EntityManager*, unsigned int, std::string>(name, this, mEntityCount, name);
	} catch (std::exception& e) {
		throw;
	}
	
	++mEntityCount;
	return Handle(indexCounterPair.first, indexCounterPair.second, name); // return a handle to the newly added entity
}

void EntityManager::Remove(const Handle& handle) {
	try {
		// remove the entity from the store using its index and counter (validity) value
		mStore.Remove(handle.mIndex, handle.mCounter);
	} catch (std::exception& e) {
		throw;
	}
}

void EntityManager::Remove(const std::string& name) {
	try {
		mStore.Remove(name); // remove all entities from the store with matching name
	} catch (std::exception& e) {
		throw;
	}
}

Entity& EntityManager::Get(const Handle& handle) {
	try {
		// return a reference to the stored entity using its index and counter (validity) value
		return mStore.Get(handle.mIndex, handle.mCounter);
	} catch (std::exception& e) {
		throw;
	}
}

std::list< std::reference_wrapper<Entity> > EntityManager::Get(const std::string& name) {
	try {
		return mStore.Get(name); // return a list of references to the stored entities 
	} catch (std::exception& e) {
		throw;
	}
}

Manager<Component>& EntityManager::GetComponentManager() {
	return mComponentManager;
}
}
