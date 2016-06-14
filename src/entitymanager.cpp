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
Entity::Entity(const unsigned int& entityID, const std::string& entityName) : mEntityID(entityID), mName(entityName) {
	
}

Entity::Entity(Entity&& other) : Entity(0u) {
	swap(*this, other);
}

Entity& Entity::operator=(Entity other) {
	swap(*this, other);
	
	return *this;
}

void swap(Entity& first, Entity& second) {
	std::swap(first.mComponentManager, second.mComponentManager);
	
	std::swap(first.mEntityID, second.mEntityID);
	std::swap(first.mName, second.mName);
}

void Entity::RemoveComponent(const Manager<Component>::Handle& handle) {
	try {
		mComponentManager.Remove(handle);
	} catch (std::exception& e) {
		throw;
	}
}

void Entity::RemoveComponents(const std::string& name) {
	try {
		mComponentManager.Remove(name);
	} catch (std::exception& e) {
		throw;
	}
}

void Entity::RemoveComponents() {
	try {
		mComponentManager.Remove();
	} catch (std::exception& e) {
		throw;
	}
}

std::list<Manager<Component>::Handle> Entity::GetComponentHandles(const std::string& name) {
	try {
		return mComponentManager.GetHandles(name);
	} catch (std::exception& e) {
		throw;
	}
}

std::list<Manager<Component>::Handle> Entity::GetComponentHandles() {
	try {
		return mComponentManager.GetHandles();
	} catch (std::exception& e) {
		throw;
	}
}

unsigned int Entity::GetEntityID() const {
	return mEntityID;
}

std::string Entity::GetName() const {
	return mName;
}


EntityManager::EntityManager(EntityManager&& other) : EntityManager() {
	swap(*this, other);
}

EntityManager& EntityManager::operator=(EntityManager other) {
	swap(*this, other);
	
	return *this;
}

void swap(EntityManager& first, EntityManager& second) {
	std::swap(first.mStore, second.mStore);
	
	std::swap(first.mEntityCount, second.mEntityCount);
}

EntityManager::Handle EntityManager::Add(const std::string& name) {
	std::pair<unsigned int, unsigned int> indexCounterPair; // 
	
	try {
		indexCounterPair = mStore.Add<unsigned int, std::string>(name, mEntityCount, name); // add a new entry to the store and save the index and counter value returned
	} catch (std::exception& e) {
		throw;
	}
	
	++mEntityCount;
	return Handle(indexCounterPair.first, indexCounterPair.second, name); // return a handle to the newly added entity
}

void EntityManager::Remove(const EntityManager::Handle& handle) {
	try {
		mStore.Remove(handle.mIndex, handle.mCounter); // remove the entity from the store using its index and counter (validity) value
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

void EntityManager::Remove() {
	try {
		mStore.Remove();
	} catch (std::exception& e) {
		throw;
	}
}

Entity& EntityManager::Get(const EntityManager::Handle& handle) {
	try {
		return mStore.Get(handle.mIndex, handle.mCounter); // return a reference to the stored resource using its index and counter (validity) value
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

std::list< std::reference_wrapper<Entity> > EntityManager::Get() {
	try {
		return mStore.Get(); // return a list of references to the stored entities 
	} catch (std::exception& e) {
		throw;
	}
}

std::list<EntityManager::Handle> EntityManager::GetHandles(const std::string& name) {
	try {
		std::list<Handle> handleList;
		std::list< std::tuple<unsigned int, unsigned int, std::string> >
				handleData = mStore.GetHandles(name); // get handle data from the store
		
		// for all handle data in the list...
		for (auto handleDataIter = handleData.begin(); handleDataIter != handleData.end(); ++handleDataIter) {
			// construct a handle to the entity and add it to the list
			handleList.emplace_back(std::get<0>(*handleDataIter),
					std::get<1>(*handleDataIter), std::get<2>(*handleDataIter));
		}
		
		return handleList;
	} catch (std::exception& e) {
		throw;
	}
}

std::list<EntityManager::Handle> EntityManager::GetHandles() {
	try {
		std::list<Handle> handleList;
		std::list< std::tuple<unsigned int, unsigned int, std::string> >
				handleData = mStore.GetHandles(); // get handle data from the store
		
		// for all handle data in the list...
		for (auto handleDataIter = handleData.begin(); handleDataIter != handleData.end(); ++handleDataIter) {
			// construct a handle to the entity and add it to the list
			handleList.emplace_back(std::get<0>(*handleDataIter),
					std::get<1>(*handleDataIter), std::get<2>(*handleDataIter));
		}
		
		return handleList;
	} catch (std::exception& e) {
		throw;
	}
}
}
