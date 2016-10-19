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

#ifndef UAIRENTITYSYSTEM_HPP
#define UAIRENTITYSYSTEM_HPP

#include "component.hpp"
#include "manager.hpp"
#include "entitymanager.hpp"
#include "systemmanager.hpp"
// #include "messagesystem.hpp"

namespace uair {
// the main class of the ECS architecture that pulls the three concepts (of entity, component and system) together into one package
class EntitySystem {
	friend class Game;
	
	public :
		typedef Manager<Component>::Handle ComponentHandle;
		typedef EntityManager::Handle EntityHandle;
	public :
		EntitySystem() = default;
		EntitySystem(const EntitySystem& other) = delete;
		EntitySystem(EntitySystem&& other);
		
		~EntitySystem() = default;
		
		EntitySystem& operator=(EntitySystem other);
		
		friend void swap(EntitySystem& first, EntitySystem& second);
		
		// begin entity manager helpers...
			EntityManager::Handle AddEntity(const std::string& name);
			
			void RemoveEntity(const EntityManager::Handle& handle);
			void RemoveEntities(const std::string& name);
			void RemoveEntities();
			
			Entity& GetEntity(const EntityManager::Handle& handle);
			std::list< std::reference_wrapper<Entity> > GetEntities(const std::string& name);
			std::list< std::reference_wrapper<Entity> > GetEntities();
			
			std::list<EntityManager::Handle> GetEntityHandles(const std::string& name);
			std::list<EntityManager::Handle> GetEntityHandles();
		// ...end entity manager helpers
		
		// begin system manager helpers...
			template <typename T>
			T& RegisterSystem();
			
			template <typename T>
			void RemoveSystem();
			
			template <typename T>
			T& GetSystem();
		// ...end system manager helpers
		
		// begin message system helpers...
			/* template <class T>
			void PushMessage(const unsigned int& systemTypeID, const T& messageIn);
			void PushMessageString(const unsigned int& systemTypeID, const unsigned int& messageTypeID, const std::string& messageString);
			
			unsigned int GetMessageCount();
			unsigned int GetSystemType(const unsigned int& index);
			unsigned int GetMessageType(const unsigned int& index);
			int GetMessageState(const unsigned int& index);
			
			template <class T>
			T PeekMessage(const unsigned int& index);
			
			template <class T>
			T GetMessage(const unsigned int& index);
			
			void PopMessage(const unsigned int& index); */
		// ...end message system helpers
	private :
		EntityManager mEntityManager;
		SystemManager mSystemManager;
		
		// MessageSystem mMessageSystem;
};

// begin system manager helpers...
	template <typename T>
	T& EntitySystem::RegisterSystem() {
		try {
			return mSystemManager.Register<T>();
		} catch (std::exception& e) {
			throw;
		}
	}
	
	template <typename T>
	void EntitySystem::RemoveSystem() {
		try {
			mSystemManager.Remove<T>();
		} catch (std::exception& e) {
			throw;
		}
	}
	
	template <typename T>
	T& EntitySystem::GetSystem() {
		try {
			return mSystemManager.Get<T>();
		} catch (std::exception& e) {
			throw;
		}
	}
// ...end system manager helpers

// begin message system helpers...
	/* template <class T>
	void EntitySystem::PushMessage(const unsigned int& systemTypeID, const T& messageIn) {
		try {
			mMessageSystem.PushMessage<T>(systemTypeID, messageIn);
		} catch (std::exception& e) {
			throw;
		}
	}

	template <class T>
	T EntitySystem::PeekMessage(const unsigned int& index) {
		try {
			return mMessageSystem.PeekMessage<T>(index);
		} catch (std::exception& e) {
			throw;
		}
	}

	template <class T>
	T EntitySystem::GetMessage(const unsigned int& index) {
		try {
			return mMessageSystem.GetMessage<T>(index);
		} catch (std::exception& e) {
			throw;
		}
	} */
// ...end message system helpers
}

#endif
