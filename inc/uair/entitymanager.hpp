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

#ifndef UAIRENTITYMANAGER_HPP
#define UAIRENTITYMANAGER_HPP

#include "manager.hpp"
#include "component.hpp"

namespace uair {
class EntityManager;

// an entity represents a game object and is essentially just a listing of components
class Entity {
	public :
		// assign a pointer to the manager that creates this entity and a unique id
		Entity(EntityManager* entityManager, const unsigned int& entityID, const std::string& entityName = "");
		
		// 
		Entity(const Entity& other) = delete;
		
		// 
		Entity(Entity&& other);
		
		// remove all associated components
		~Entity();
		
		// 
		Entity& operator=(Entity other);
		
		// 
		friend void swap(Entity& first, Entity& second);
		
		// create and associate a custom component with this entity
		template<typename T, typename ...Ps>
		Manager<Component>::Handle AddComponent(const Ps&... params);
		
		// remove
		
		// return an array of handles to the components associated with this entity
		std::vector<Manager<Component>::Handle> GetComponents();
		
		unsigned int GetEntityID() const {
			return mEntityID;
		}
	private :
		EntityManager* mEntityManagerPtr; // a pointer to the entity manager that created this entity
		std::vector<Manager<Component>::Handle> mComponents; // store of component handles belonging to this entity
		
		unsigned int mEntityID = 0u;
		std::string mName = "";
};


// a specialised version of the manager<T> class that handles creation, retrieval and removal of entity objects
class EntityManager {
	public :
		// a handle that is used to refer to resources handled instead of a pointer
		class Handle {
			public :
				Handle(const unsigned int& index, const unsigned int& counter) : 
						mIndex(index), mCounter(counter) {
					
					
				}
			public :
				unsigned int mIndex = 0u; // the index of the resource in the store
				unsigned int mCounter = 0u; // the counter value of the index used to validate the handle
		};
	public :
		// assign a reference to the associated component manager allowing created entities to attach custom components
		EntityManager(Manager<Component>& componentManager);
		
		// add a new blank entity to the store and return a handle to it
		Handle Add(const std::string& entityName = "");
		
		// remove an entity from the store via its handle
		void Remove(const Handle& handle);
		
		// return a reference to an entity pointed to by its handle
		Entity& Get(const Handle& handle);
		
		// return a reference to the component manager associated with this entity manager
		Manager<Component>& GetComponentManager();
	private :
		Manager<Component>& mComponentManager; // a reference to the component manager associated with this entity manager
		Store<Entity> mStore;
		
		unsigned int mEntityCount = 1u;
};

template<typename T, typename ...Ps>
Manager<Component>::Handle Entity::AddComponent(const Ps&... params) {
	Manager<Component>::Handle newComponentHandle = mEntityManagerPtr->GetComponentManager().Add<T>(params...);
	mComponents.push_back(newComponentHandle);
	return newComponentHandle;
}
}

#endif
