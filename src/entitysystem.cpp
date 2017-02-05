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

#include "entitysystem.hpp"

namespace uair {
EntitySystem::EntitySystem(EntitySystem&& other) : EntitySystem() {
	swap(*this, other);
}

EntitySystem& EntitySystem::operator=(EntitySystem other) {
	swap(*this, other);
	
	return *this;
}

void swap(EntitySystem& first, EntitySystem& second) {
	std::swap(first.mEntityManager, second.mEntityManager);
	std::swap(first.mSystemManager, second.mSystemManager);
	
	// std::swap(first.mMessageSystem, second.mMessageSystem);
}

// begin entity manager helpers...
	EntityManager::Handle EntitySystem::AddEntity(const std::string& name) {
		try {
			return mEntityManager.Add(name);
		} catch (std::exception& e) {
			throw;
		}
	}
	
	void EntitySystem::RemoveEntity(const EntityManager::Handle& handle) {
		try {
			mEntityManager.Remove(handle);
		} catch (std::exception& e) {
			throw;
		}
	}
	
	void EntitySystem::RemoveEntities(const std::string& name) {
		try {
			mEntityManager.Remove(name);
		} catch (std::exception& e) {
			throw;
		}
	}
	
	void EntitySystem::RemoveEntities() {
		try {
			mEntityManager.Remove();
		} catch (std::exception& e) {
			throw;
		}
	}
	
	Entity& EntitySystem::GetEntity(const EntityManager::Handle& handle) {
		try {
			return mEntityManager.Get(handle);
		} catch (std::exception& e) {
			throw;
		}
	}
	
	std::list< std::reference_wrapper<Entity> > EntitySystem::GetEntities(const std::string& name) {
		try {
			return mEntityManager.Get(name);
		} catch (std::exception& e) {
			throw;
		}
	}
	
	std::list< std::reference_wrapper<Entity> > EntitySystem::GetEntities() {
		try {
			return mEntityManager.Get();
		} catch (std::exception& e) {
			throw;
		}
	}
	
	std::list<EntityManager::Handle> EntitySystem::GetEntityHandles(const std::string& name) {
		try {
			return mEntityManager.GetHandles(name);
		} catch (std::exception& e) {
			throw;
		}
	}
	
	std::list<EntityManager::Handle> EntitySystem::GetEntityHandles() {
		try {
			return mEntityManager.GetHandles();
		} catch (std::exception& e) {
			throw;
		}
	}
// ...end entity manager helpers

// begin message system helpers...
	/* void EntitySystem::PushMessageString(const unsigned int& systemTypeID, const unsigned int& messageTypeID, const std::string& messageString) {
		mMessageSystem.PushMessageString(systemTypeID, messageTypeID, messageString);
	}

	unsigned int EntitySystem::GetMessageCount() {
		return mMessageSystem.GetMessageCount();
	}

	unsigned int EntitySystem::GetSystemType(const unsigned int& index) {
		try {
			return mMessageSystem.GetSystemType(index);
		} catch (std::exception& e) {
			throw;
		}
	}

	unsigned int EntitySystem::GetMessageType(const unsigned int& index) {
		try {
			return mMessageSystem.GetMessageType(index);
		} catch (std::exception& e) {
			throw;
		}
	}

	int EntitySystem::GetMessageState(const unsigned int& index) {
		try {
			return mMessageSystem.GetMessageState(index);
		} catch (std::exception& e) {
			throw;
		}
	}

	void EntitySystem::PopMessage(const unsigned int& index) {
		mMessageSystem.PopMessage(index);
	} */
// ...end message system helpers
}
