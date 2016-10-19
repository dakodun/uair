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
// #include "messagesystem.hpp"

namespace uair {
class SystemManager;

class System {
	public :
		System() = default;
		System(const System& other) = delete;
		System(System&& other);
		
		virtual ~System();
		
		System& operator=(System other);
		
		friend void swap(System& first, System& second);
		
		// register an entity handle to the system indicating it should be processed by the system
		void RegisterEntity(EntityManager::Handle handle);
		
		// unregister an entity handle from the system to prevent further processing by the system
		void UnregisterEntity(EntityManager::Handle handle);
	protected :
		std::vector<EntityManager::Handle> mRegisteredEntities;
};


// a specialised version of the manager<T> class that handles creation, retrieval and removal of system objects
class SystemManager {
	public :
		SystemManager() = default;
		SystemManager(const SystemManager& other) = delete;
		SystemManager(SystemManager&& other);
		
		~SystemManager();
		
		SystemManager& operator=(SystemManager other);
		
		friend void swap(SystemManager& first, SystemManager& second);
		
		// register a system to the system manager (creates a single instance of it) and return a reference
		template <typename T>
		T& Register();
		
		// remove a system from the system manager
		template <typename T>
		void Remove();
		
		// return a reference to a registered system
		template <typename T>
		T& Get();
	private :
		std::map<unsigned int, System*> mStore;
};

template <typename T>
T& SystemManager::Register() {
	if (!std::is_base_of<System, T>::value) { // if the system being registered doesn't inherit from base class...
		throw std::runtime_error("not of base type"); // an error has occurred, don't register system
	}
	
	unsigned int typeID = T::GetTypeID(); // get the type id relating to the resource type
	
	auto storeResult = mStore.find(typeID); // search the registered systems store for a match with the unique id
	if (storeResult != mStore.end()) { // if a match was found...
		throw std::runtime_error("type already registered"); // an error has occurred, don't register system
	}
	
	T* systemPtr = static_cast<T*>((mStore.insert(std::make_pair(typeID, new T)).first)->second); // add a new instance of the system to the store and cast it to its derived type
	return *systemPtr; // return a reference
}

template <typename T>
void SystemManager::Remove() {
	unsigned int typeID = T::GetTypeID(); // get the type id relating to the resource type
	
	auto storeResult = mStore.find(typeID); // search the registered systems store for a match with the unique id 
	if (storeResult == mStore.end()) { // if a match was not found... 
		throw std::runtime_error("type not registered"); // an error has occurred, don't remove a system
	}
	
	delete storeResult->second; // destroy the registered system object
	mStore.erase(storeResult); // remove the associated base pointer from the store
}

template <typename T>
T& SystemManager::Get() {
	unsigned int typeID = T::GetTypeID(); // get the type id relating to the resource type
	
	auto storeResult = mStore.find(typeID); // search the registered systems store for a match with the unique id 
	if (storeResult == mStore.end()) { // if a match was not found... 
		throw std::runtime_error("type not registered"); // an error has occurred, don't retrieve a system 
	}
	
	T* systemPtr = static_cast<T*>(storeResult->second); // get the associated base pointer from the store and cast it to its derived type
	return *systemPtr; // return a reference
}
}

#endif
