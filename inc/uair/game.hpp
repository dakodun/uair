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

#ifndef UAIRGAME_HPP
#define UAIRGAME_HPP

#include <ft2build.h>
#include FT_FREETYPE_H

#include <vector>

#include "scenemanager.hpp"
#include "inputmanager.hpp"
#include "resourcemanager.hpp"
#include "shaderprogram.hpp"
#include "timer.hpp"
#include "window.hpp"

namespace uair {
class Game {
	public :
		Game();
		~Game();
		
		void Run();
		
		void Input();
		void Process();
		void PostProcess(unsigned int processed);
		void Render();
		
		bool AddWindow();
		bool AddWindow(const std::string & windowTitle, const WindowDisplaySettings & settings,
				const unsigned long & windowStyle = WindowStyles::Visible);
		
		void Quit();
		
		void CreateDefaultShader();
		void SetShader();
		// void SetShader(const Shader & shader);
		
		// scene manager related helper functions
		SceneManagerPtr GetSceneManager();
		bool RequestSceneChange(Scene* newScene, const bool & restore = true);
		bool CurrentSceneExists();
		bool NextSceneExists();
		ScenePtr GetCurrentScene();
		ScenePtr GetNextScene();
		
		template<typename T>
		std::shared_ptr<T> GetCurrentSceneCast() {
			try {
				return CastScene<T>(GetCurrentScene());
			} catch(...) {
				throw;
			}
		}
		
		template<typename T>
		std::shared_ptr<T> GetNextSceneCast() {
			try {
				return CastScene<T>(GetNextScene());
			} catch(...) {
				throw;
			}
		}
		
		template<typename T>
		std::shared_ptr<T> CastScene(ScenePtr scenePtr) {
			try {
				return mSceneManager->CastScene<T>(scenePtr);
			} catch (...) {
				throw;
			}
		}
		
		// input manager related helper functions
		InputManagerPtr GetInputManager();
		bool GetKeyboardDown(const Keyboard & key) const;
		bool GetKeyboardPressed(const Keyboard & key) const;
		bool GetKeyboardReleased(const Keyboard & key) const;
		bool GetMouseDown(const Mouse & button) const;
		bool GetMousePressed(const Mouse & button) const;
		bool GetMouseReleased(const Mouse & button) const;
		int GetMouseWheel() const;
		glm::ivec2 GetLocalMouseCoords() const;
		glm::ivec2 GetGlobalMouseCoords() const;
		
		// resource manager related helper functions
		void SetResourceManager(ResourceManager* resMan);
		ResourceManagerPtr GetResourceManager();
		
		template<typename T>
		std::shared_ptr<T> GetResourceManagerCast() {
			try {
				return CastResourceManager<T>(mResourceManager);
			} catch (...) {
				throw;
			}
		}
		
		template<typename T>
		std::shared_ptr<T> CastResourceManager(ResourceManagerPtr resManPtr) {
			std::shared_ptr<T> converted = std::dynamic_pointer_cast<T>(resManPtr);
			if (converted) {
				return converted;
			}
			
			throw std::runtime_error("invalid resource manager conversion");
		}
		
		// 
		const FT_Library& GetFTLibrary() const {
			return mFTLibrary;
		}
	public :
		std::vector<WindowEvent> mEventQueue;
		
		float mFrameLowerLimit = 0.02f;
		float mFrameUpperLimit = 1.0f;
	private :
		void HandleEventQueue(const WindowEvent & e);
	private :
		float mTotalFrameTime = 0.0f;
		float mAccumulator = 0.0f;
		Timer mTimer;
		
		WindowPtr mWindow;
		bool mOpen = false;
		
		SceneManagerPtr mSceneManager;
		InputManagerPtr mInputManager;
		ResourceManagerPtr mResourceManager;
		
		ShaderProgram mDefaultShader;
		
		FT_Library mFTLibrary;
};

static Game GAME;
}

#endif
