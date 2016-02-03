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

namespace uair {
// the main class of the ECS architecture that pulls the three concepts (of entity, component and system) together into one package
class EntitySystem {
	public :
		typedef Manager<Component>::Handle ComponentHandle;
		typedef EntityManager::Handle EntityHandle;
	public :
		EntitySystem();
		
		// convenience functions that invoke behaviour of the respective underlying manager
		template <typename T>
		void RegisterComponentType();
		
		EntityHandle AddEntity();
		void RemoveEntity(const EntityHandle& handle);
		Entity& GetEntity(const EntityHandle& handle);
		
		template <typename T>
		void RegisterSystem();
		
		template <typename T>
		T& GetSystem();
	private :
		Manager<Component> mComponentManager;
		EntityManager mEntityManager;
		SystemManager mSystemManager;
};

template <typename T>
void EntitySystem::RegisterComponentType() {
	mComponentManager.Register<T>();
}

template <typename T>
void EntitySystem::RegisterSystem() {
	mSystemManager.Register<T>();
}

template <typename T>
T& EntitySystem::GetSystem() {
	return mSystemManager.Get<T>();
}
}

#endif
