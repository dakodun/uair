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

#ifndef UAIRSYSTEMMANAGER_HPP
#define UAIRSYSTEMMANAGER_HPP

#include "manager.hpp"
#include "component.hpp"
#include "entitymanager.hpp"

namespace uair {
class System {
	public :
		virtual ~System();
		
		// register an entity handle to the system indicating it should be processed by the system
		void RegisterEntity(EntityManager::Handle handle);
		
		// derived systems must implement this function and return a unique id (unique amongst other derived systems)
		virtual unsigned int GetTypeID() = 0;
	protected :
		std::vector<EntityManager::Handle> mRegisteredEntities;
};


class SystemManager {
	public :
		// assign a reference to the associated entity and component managers allowing access to both to registered systems
		SystemManager(EntityManager& entityManager, Manager<Component>& componentManager);
		
		~SystemManager();
		
		// register a system to the system manager (creates a single instance of it)
		template <typename T>
		void Register();
		
		// remove a system from the system manager
		template <typename T>
		void Remove();
		
		// return a reference to a registered system
		template <typename T>
		T& Get();
		
		// return a reference to the entity manager associated with this system manager
		EntityManager& GetEntityManager();
		
		// return a reference to the component manager associated with this system manager
		Manager<Component>& GetComponentManager();
	private :
		EntityManager& mEntityManager; // a reference to the entity manager associated with this system manager
		Manager<Component>& mComponentManager; // a reference to the component manager associated with this system manager
		std::map<unsigned int, System*> mStore;
};

template <typename T>
void SystemManager::Register() {
	if (!std::is_base_of<System, T>::value) { // if the system being registered doesn't inherit from base class...
		throw std::runtime_error("not of base type"); // an error has occurred, don't register system
	}
	
	T tempT(nullptr); // create a temporary object of the system being registered
	unsigned int typeID = tempT.GetTypeID(); // retrieve the unique type id from the temp
	
	auto storeResult = mStore.find(typeID); // search the registered systems store for a match with the unique id
	if (storeResult != mStore.end()) { // if a match was found...
		throw std::runtime_error("type already registered"); // an error has occurred, don't register system
	}
	
	mStore.insert(std::make_pair(typeID, new T(this))); // add a new instance of the system to the store
}

// 
template <typename T>
void SystemManager::Remove() {
	T tempT(nullptr); // create a temporary object of the system being removed
	unsigned int typeID = tempT.GetTypeID(); // retrieve the unique type id from the temp
	
	auto storeResult = mStore.find(typeID); // search the registered systems store for a match with the unique id 
	if (storeResult == mStore.end()) { // if a match was not found... 
		throw std::runtime_error("type not registered"); // an error has occurred, don't remove a system
	}
	
	delete storeResult->second; // destroy the registered system object
	mStore.erase(storeResult); // remove the associated base pointer from the store
}

// 
template <typename T>
T& SystemManager::Get() {
	T tempT(nullptr); // create a temporary object of the system being removed
	unsigned int typeID = tempT.GetTypeID(); // retrieve the unique type id from the temp
	
	auto storeResult = mStore.find(typeID); // search the registered systems store for a match with the unique id 
	if (storeResult == mStore.end()) { // if a match was not found... 
		throw std::runtime_error("type not registered"); // an error has occurred, don't retrieve a system 
	}
	
	T* systemPtr = static_cast<T*>(storeResult->second); // get the associated base pointer from the store and cast it to its derived type
	return *systemPtr; // return a reference
}
}

#endif
