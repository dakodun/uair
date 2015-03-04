/* **************************************************************** **
**	Uair Engine
**	Copyright (c) 2014 Iain M. Crawford
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

#ifndef UAIRSCENEMANAGER_HPP
#define UAIRSCENEMANAGER_HPP

#include <string>
#include <unordered_map>
#include <memory>

#include "scene.hpp"

namespace uair {
class SceneManager {
	friend class Game;
	
	public :
		bool RequestSceneChange(Scene* newScene, const bool & restore = true);
		
		bool CurrentSceneExists();
		bool NextSceneExists();
		
		ScenePtr GetCurrentScene();
		ScenePtr GetNextScene();
		
		template<typename T>
		std::shared_ptr<T> CastScene(ScenePtr scenePtr) {
			std::shared_ptr<T> converted = std::dynamic_pointer_cast<T>(scenePtr);
			if (converted) {
				return converted;
			}
			
			throw std::runtime_error("invalid scene conversion");
		}
		
		void Clear();
	private :
		bool ChangeScene();
	private :
		typedef std::unordered_multimap<std::string, ScenePtr> SceneMap;
		SceneMap mStore;
		
		ScenePtr mCurrScene;
		ScenePtr mNextScene;
};

typedef std::shared_ptr<SceneManager> SceneManagerPtr;
}

#endif
