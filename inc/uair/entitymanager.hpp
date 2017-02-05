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

#ifndef UAIRENTITYMANAGER_HPP
#define UAIRENTITYMANAGER_HPP

#include "init.hpp"
#include "manager.hpp"
#include "component.hpp"

namespace uair {
// an entity represents a game object and is essentially just a listing of components
class EXPORTDLL Entity {
	public :
		// assign a unique entity id (unique within entity manager that created entity) and an entity name
		Entity(const unsigned int& entityID, const std::string& entityName = "");
		
		// entities shouldn't be copied
		Entity(const Entity& other) = delete;
		
		// entities can be moved (required by manager)
		Entity(Entity&& other);
		
		Entity& operator=(Entity other);
		
		friend void swap(Entity& first, Entity& second);
		
		// begin manager helper functions...
			template <typename T>
			void RegisterComponent();
			
			template <typename T>
			bool IsComponentRegistered();
			
			template <typename T, typename ...Ps>
			Manager<Component>::Handle AddComponent(const std::string& name, const Ps&... params);
			
			template <typename T>
			void RemoveComponent(const Manager<Component>::Handle& handle);
			
			template <typename T>
			void RemoveComponents(const std::string& name);
			
			template <typename T>
			void RemoveComponents();
			
			void RemoveComponent(const Manager<Component>::Handle& handle);
			
			void RemoveComponents(const std::string& name);
			
			void RemoveComponents();
			
			template <typename T>
			T& GetComponent(const Manager<Component>::Handle& handle);
			
			template <typename T>
			std::list< std::reference_wrapper<T> > GetComponents(const std::string& name);
			
			template <typename T>
			std::list< std::reference_wrapper<T> > GetComponents();
			
			template <typename T>
			std::list<Manager<Component>::Handle> GetComponentHandles(const std::string& name);
			
			template <typename T>
			std::list<Manager<Component>::Handle> GetComponentHandles();
			
			std::list<Manager<Component>::Handle> GetComponentHandles(const std::string& name);
			
			std::list<Manager<Component>::Handle> GetComponentHandles();
		// ...end manager helper functions
		
		// return the unique id assigned to this entity
		unsigned int GetEntityID() const;
		
		// return the name (if any) assigned to this entity
		std::string GetName() const;
	private :
		Manager<Component> mComponentManager; // manager to handle creation, storage and destruction of components
		
		unsigned int mEntityID = 0u;
		std::string mName = "";
};


// a specialised version of the manager<T> class that handles creation, retrieval and removal of entity objects
class EXPORTDLL EntityManager {
	public :
		// a handle that is used to refer to resources handled instead of a pointer
		class Handle {
			public :
				Handle(const unsigned int& index, const unsigned int& counter, const std::string& name = "") : 
						mIndex(index), mCounter(counter), mName(name) {
					
					
				}
				
				bool operator ==(const Handle &other) const {
					return (mIndex == other.mIndex && mCounter == other.mCounter &&
							mName == other.mName);
				}
			public :
				unsigned int mIndex = 0u; // the index of the resource in the store
				unsigned int mCounter = 0u; // the counter value of the index used to validate the handle
				std::string mName = ""; // a name that the handled resource can be identified by (non-unique)
		};
	public :
		EntityManager() = default;
		EntityManager(const EntityManager& other) = delete;
		EntityManager(EntityManager&& other);
		
		~EntityManager() = default;
		
		EntityManager& operator=(EntityManager other);
		
		friend void swap(EntityManager& first, EntityManager& second);
		
		// add a new entity with the specified name to the store and return a custom handle to it
		Handle Add(const std::string& name);
		
		// remove an entity from the store
		void Remove(const Handle& handle);
		
		// remove all entities with the specified name from the store
		void Remove(const std::string& name);
		
		// remove ALL entities from the store
		void Remove();
		
		// return a reference to en entity to by the supplied handle
		Entity& Get(const Handle& handle);
		
		// return a list of references matching name
		std::list< std::reference_wrapper<Entity> > Get(const std::string& name);
		
		// return a list of references to ALL entities
		std::list< std::reference_wrapper<Entity> > Get();
		
		// return handles to entities matching name
		std::list<Handle> GetHandles(const std::string& name);
		
		// return handles to ALL entities
		std::list<Handle> GetHandles();
	private :
		Store<Entity, Entity> mStore; // the store used to hold entities (see manager<T> class)
		
		unsigned int mEntityCount = 0u; // the counter used to assign a unique id to an entity
};

template <typename T>
void Entity::RegisterComponent() {
	try {
		if (!mComponentManager.IsRegistered<T>()) {
			mComponentManager.Register<T>();
		}
	} catch (std::exception& e) {
		throw;
	}
}

template <typename T>
bool Entity::IsComponentRegistered() {
	return mComponentManager.IsRegistered<T>();
}

template <typename T, typename ...Ps>
Manager<Component>::Handle Entity::AddComponent(const std::string& name, const Ps&... params) {
	try {
		if (!mComponentManager.IsRegistered<T>()) {
			mComponentManager.Register<T>();
		}
		
		return mComponentManager.Add<T, Ps...>(name, params...);
	} catch (std::exception& e) {
		throw;
	}
}

template <typename T>
void Entity::RemoveComponent(const Manager<Component>::Handle& handle) {
	try {
		mComponentManager.Remove<T>(handle);
	} catch (std::exception& e) {
		throw;
	}
}

template <typename T>
void Entity::RemoveComponents(const std::string& name) {
	try {
		mComponentManager.Remove<T>(name);
	} catch (std::exception& e) {
		throw;
	}
}

template <typename T>
void Entity::RemoveComponents() {
	try {
		mComponentManager.Remove<T>();
	} catch (std::exception& e) {
		throw;
	}
}

template <typename T>
T& Entity::GetComponent(const Manager<Component>::Handle& handle) {
	try {
		return mComponentManager.Get<T>(handle);
	} catch (std::exception& e) {
		throw;
	}
}

template <typename T>
std::list< std::reference_wrapper<T> > Entity::GetComponents(const std::string& name) {
	try {
		return mComponentManager.Get<T>(name);
	} catch (std::exception& e) {
		throw;
	}
}

template <typename T>
std::list< std::reference_wrapper<T> > Entity::GetComponents() {
	try {
		return mComponentManager.Get<T>();
	} catch (std::exception& e) {
		throw;
	}
}

template <typename T>
std::list<Manager<Component>::Handle> Entity::GetComponentHandles(const std::string& name) {
	try {
		return mComponentManager.GetHandles<T>(name);
	} catch (std::exception& e) {
		throw;
	}
}

template <typename T>
std::list<Manager<Component>::Handle> Entity::GetComponentHandles() {
	try {
		return mComponentManager.GetHandles<T>();
	} catch (std::exception& e) {
		throw;
	}
}
}

#endif
