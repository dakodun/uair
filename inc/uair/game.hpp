/* **************************************************************** **
**	Uair Engine
**	Copyright (c) 20XX Iain M. Crawford
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
#include "openglcontext.hpp"

namespace uair {
class Game {
	public :
		Game();
		~Game();
		
		void Run();
		
		void Input();
		void Process();
		void PostProcess(const unsigned int & processed);
		void Render(const unsigned int & pass);
		
		void Init();
		void Init(const std::string & windowTitle, const WindowDisplaySettings & settings,
				const unsigned long & windowStyle = WindowStyles::Visible);
		WindowPtr GetWindow();
		OpenGLContextPtr GetContext();
		
		void Quit();
		void Clear();
		
		void CreateDefaultShader();
		void SetShader();
		
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
		void SetMouseCoords(const glm::ivec2& newCoords, const CoordinateSpace& coordinateSpace = CoordinateSpace::Local);
		glm::ivec2 GetMouseCoords(const CoordinateSpace& coordinateSpace = CoordinateSpace::Local) const;
		bool DeviceExists(const unsigned int& deviceID) const;
		const InputManager::InputDevice& GetDevice(const unsigned int& deviceID) const;
		unsigned int GetDeviceButtonCount(const int& deviceID) const;
		bool GetDeviceButtonDown(const int& deviceID, const unsigned int& button) const;
		bool GetDeviceButtonPressed(const int& deviceID, const unsigned int& button) const;
		bool GetDeviceButtonReleased(const int& deviceID, const unsigned int& button) const;
		unsigned int GetDeviceControlCount(const int& deviceID) const;
		bool DeviceHasControl(const int& deviceID, const Device& control) const;
		int GetDeviceControl(const int& deviceID, const Device& control) const;
		int GetDeviceControlScaled(const int& deviceID, const Device& control, std::pair<int, int> range = std::make_pair(0, 255)) const;
		std::pair<int, int> GetDeviceControlRange(const int& deviceID, const Device& control) const;
		std::vector<Device> GetDeviceLinkedDevices(const int& deviceID, const unsigned int& collectionID) const;
		unsigned int GetDeviceLinkID(const int& deviceID, const Device& control) const;
		
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
		float mFrameLowerLimit = 0.02f;
		float mFrameUpperLimit = 1.0f;
		
		unsigned int mRenderPasses = 1u;
	private :
		void HandleEventQueue(const WindowEvent& e);
		
		void AddWindow();
		void AddWindow(const std::string & windowTitle, const WindowDisplaySettings & settings,
				const unsigned long & windowStyle = WindowStyles::Visible);
		void AddContext();
		void AddContext(WindowPtr windowPtr);
		void MakeCurrent(WindowPtr windowPtr, OpenGLContextPtr contextPtr);
	private :
		float mTotalFrameTime = 0.0f;
		float mAccumulator = 0.0f;
		Timer mTimer;
		
		WindowPtr mWindow;
		OpenGLContextPtr mContext;
		bool mOpen = true;
		
		SceneManagerPtr mSceneManager;
		InputManagerPtr mInputManager;
		ResourceManagerPtr mResourceManager;
		
		static bool mDefaultShaderExists;
		ShaderProgram mDefaultShader;
		
		FT_Library mFTLibrary;
};

extern Game GAME;
}

#endif
