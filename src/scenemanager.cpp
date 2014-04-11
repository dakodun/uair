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

#include "scenemanager.hpp"

namespace uair {
bool SceneManager::RequestSceneChange(Scene* newScene, const bool & restore) {
	bool found = false; // indicates if we've found a stored scene or not
	ScenePtr tempNextScene; // temporary shared_ptr that holds the next scene
	
	auto range = mStore.equal_range(newScene->GetName()); // get the range of scenes that match the scene name
	if (range.first == range.second) { // if both of the ranges are the same
		found = false; // no item with the key was present
	}
	else { // otherwise the ranges differ
		for (auto iter = range.first; iter != range.second; ++iter) { // for all scenes in the range
			if (newScene->GetTag() == (iter->second)->GetTag()) { // if the scenes tag matches
				if (restore == true) { // if we are to restore the previosuly saved scene
					tempNextScene = iter->second; // set it as the temp next scene
					found = true; // indicate we have our next scene
				}
				
				mStore.erase(iter); // remove it from the store
				break; // stop searching
			}
		}
	}
	
	if (found == false) { // if we didn't find the requested scene in the store
		tempNextScene = ScenePtr(newScene); // use the scene supplied as a new scene
	}
	
	tempNextScene.swap(mNextScene); // swap the temp with our next scene member
	return true; // scene change request was successful
}

bool SceneManager::CurrentSceneExists() {
	if (mCurrScene) {
		return true;
	}
	
	return false;
}

bool SceneManager::NextSceneExists() {
	if (mNextScene) {
		return true;
	}
	
	return false;
}

ScenePtr SceneManager::GetCurrentScene() {
	if (mCurrScene) {
		return ScenePtr(mCurrScene);
	}
	
	throw std::runtime_error("");
}

ScenePtr SceneManager::GetNextScene() {
	if (mNextScene) {
		return ScenePtr(mNextScene);
	}
	
	throw std::runtime_error("");
}

bool SceneManager::ChangeScene() {
	if (mNextScene) { // if we have a next scene ready to go
		if (mCurrScene) { // if we have a current scene that we're switching from
			mCurrScene->OnLeave(); // perform function on leaving the scene
			
			if (mCurrScene->mPersist == true) { // if the current scene is to be persistent
				auto iter = mStore.emplace(mCurrScene->GetName(), ScenePtr()); // create a new scene pointer
				(iter->second).swap(mCurrScene); // swap our current scene into the stored scene pointer
			}
			
			mCurrScene.reset(); // reset the current scene pointer
		}
		
		mCurrScene.swap(mNextScene); // swap the next scene pointer into our current scene pointer
		mCurrScene->OnEnter(); // perform function on entering the scene
		
		return true; // scene change was successful
	}
	
	return false;
}
}
