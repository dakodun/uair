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
		
		// entities shouldn't be copied
		Entity(const Entity& other) = delete;
		
		// entities can be moved (required by manager)
		Entity(Entity&& other);
		
		// remove all associated components
		~Entity();
		
		Entity& operator=(Entity other);
		
		friend void swap(Entity& first, Entity& second);
		
		// create and associate a custom component with this entity
		template<typename T, typename ...Ps>
		Manager<Component>::Handle AddComponent(const Ps&... params);
		
		// remove all components from the entity
		void RemoveAllComponents();
		
		// remove all components of a type from the entity
		template<typename T>
		void RemoveComponents();
		
		// remove all components of a type with the specified name from the entity
		template<typename T>
		void RemoveComponents(const std::string& name);
		
		// return an array of handles to all components associated with this entity
		std::vector<Manager<Component>::Handle> GetAllComponents();
		
		// return an array of handles to all components of a type associated with this entity
		template<typename T>
		std::vector<Manager<Component>::Handle> GetComponents();
		
		// return an array of handles to all components of a type with the specified name associated with this entity
		template<typename T>
		std::vector<Manager<Component>::Handle> GetComponents(const std::string& name);
		
		// return the unique id assigned to this entity
		unsigned int GetEntityID() const;
		
		// return the name (if any) assigned to this entity
		std::string GetName() const;
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
				Handle(const unsigned int& index, const unsigned int& counter, const std::string& name = "") : 
						mIndex(index), mCounter(counter), mName(name) {
					
					
				}
			public :
				unsigned int mIndex = 0u; // the index of the resource in the store
				unsigned int mCounter = 0u; // the counter value of the index used to validate the handle
				std::string mName = ""; // a name that the handled resource can be identified by (non-unique)
		};
	public :
		// assign a reference to the associated component manager allowing created entities to attach custom components
		EntityManager(Manager<Component>& componentManager);
		
		// add a new entity with the specified name to the store and return a custom handle to it
		Handle Add(const std::string& name);
		
		// remove an entity from the store
		void Remove(const Handle& handle);
		
		// remove all entities owith the specified name from the store
		void Remove(const std::string& name);
		
		// return a reference to an entity pointed to by the supplied handle
		Entity& Get(const Handle& handle);
		
		// return a list of references matching name
		std::list< std::reference_wrapper<Entity> > Get(const std::string& name);
		
		// return a reference to the component manager associated with this entity manager
		Manager<Component>& GetComponentManager();
	private :
		Manager<Component>& mComponentManager; // a reference to the component manager associated with this entity manager
		Store<Entity> mStore; // the store used to hold entities (see manager<T> class)
		
		unsigned int mEntityCount = 1u; // the counter used to assign a unique id to an entity
};

template<typename T, typename ...Ps>
Manager<Component>::Handle Entity::AddComponent(const Ps&... params) {
	Manager<Component>::Handle newComponentHandle = mEntityManagerPtr->GetComponentManager().Add<T>(params...);
	mComponents.push_back(newComponentHandle);
	return newComponentHandle;
}

template<typename T>
void Entity::RemoveComponents() {
	T temp;
	
	for (unsigned int i = 0u; i < mComponents.size(); ) {
		if (mComponents.at(i).mTypeID == temp.GetTypeID()) {
			try {
				mEntityManagerPtr->GetComponentManager().Remove(*(mComponents.begin() + i)); // invoke removal via the component manager
				mComponents.erase(mComponents.begin() + i);
			} catch (std::exception& e) {
				throw;
			}
			
			continue;
		}
		
		++i;
	}
}

template<typename T>
void Entity::RemoveComponents(const std::string& name) {
	T temp;
	
	for (unsigned int i = 0u; i < mComponents.size(); ) {
		if (mComponents.at(i).mTypeID == temp.GetTypeID()) {
			Component& component = mEntityManagerPtr->GetComponentManager().Get<T>(mComponents.at(i));
			
			if (component.GetName() == name) {
				try {
					mEntityManagerPtr->GetComponentManager().Remove(*(mComponents.begin() + i)); // invoke removal via the component manager
					mComponents.erase(mComponents.begin() + i);
				} catch (std::exception& e) {
					throw;
				}
				
				continue;
			}
		}
		
		++i;
	}
}

template<typename T>
std::vector<Manager<Component>::Handle> Entity::GetComponents() {
	std::vector<Manager<Component>::Handle> components;
	T temp;
	
	for (auto iter = mComponents.begin(); iter != mComponents.end(); ++iter) {
		if (iter->mTypeID == temp.GetTypeID()) {
			components.push_back(*iter);
		}
	}
	
	return components;
}

template<typename T>
std::vector<Manager<Component>::Handle> Entity::GetComponents(const std::string& name) {
	std::vector<Manager<Component>::Handle> components;
	T temp;
	
	for (auto iter = mComponents.begin(); iter != mComponents.end(); ++iter) {
		if (iter->mTypeID == temp.GetTypeID()) {
			Component& component = mEntityManagerPtr->GetComponentManager().Get<T>(*iter);
			
			if (component.GetName() == name) {
				components.push_back(*iter);
			}
		}
	}
	
	return components;
}
}

#endif
