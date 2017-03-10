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

#ifndef UAIRGAME_HPP
#define UAIRGAME_HPP

#include <ft2build.h>
#include FT_FREETYPE_H

#include <vector>

#include "init.hpp"
#include "scenemanager.hpp"
#include "inputmanager.hpp"
#include "shader.hpp"
#include "timer.hpp"
#include "window.hpp"
#include "openglcontext.hpp"
#include "messagequeue.hpp"
#include "resource.hpp"
#include "manager.hpp"
#include "entitysystem.hpp"

namespace uair {
class EXPORTDLL Game {
	public :
		Game();
		~Game();
		
		void Run();
		
		void Input();
		void Process(float deltaTime);
		void PostProcess(const unsigned int & processed, float deltaTime);
		void Render(const unsigned int & pass);
		
		void Init();
		void Init(const std::string & windowTitle, const WindowDisplaySettings & settings,
				const unsigned long & windowStyle = WindowStyles::Visible);
		WindowPtr GetWindow();
		OpenGLContextPtr GetContext();
		
		void Quit();
		void Clear();
		
		void CreateDefaultShader();
		
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
		//
		
		// input manager related helper functions
			InputManagerPtr GetInputManager();
			bool GetKeyboardDown(const Keyboard & key) const;
			bool GetKeyboardPressed(const Keyboard & key) const;
			bool GetKeyboardReleased(const Keyboard & key) const;
			unsigned int GetKeyboardState(const Keyboard& key) const;
			std::u16string GetInputString() const;
			bool GetMouseDown(const Mouse & button) const;
			bool GetMousePressed(const Mouse & button) const;
			bool GetMouseReleased(const Mouse & button) const;
			unsigned int GetMouseState(const Mouse& button) const;
			int GetMouseWheel() const;
			void SetMouseCoords(const glm::ivec2& newCoords, const CoordinateSpace& coordinateSpace = CoordinateSpace::Local);
			glm::ivec2 GetMouseCoords(const CoordinateSpace& coordinateSpace = CoordinateSpace::Local) const;
			bool DeviceExists(const int& deviceID) const;
			const InputManager::InputDevice& GetDevice(const int& deviceID) const;
			unsigned int GetDeviceButtonCount(const int& deviceID) const;
			bool GetDeviceButtonDown(const int& deviceID, const unsigned int& button) const;
			bool GetDeviceButtonPressed(const int& deviceID, const unsigned int& button) const;
			bool GetDeviceButtonReleased(const int& deviceID, const unsigned int& button) const;
			unsigned int GetDeviceButtonState(const int& deviceID, const unsigned int& button) const;
			unsigned int GetDeviceControlCount(const int& deviceID) const;
			bool DeviceHasControl(const int& deviceID, const Device& control) const;
			int GetDeviceControl(const int& deviceID, const Device& control) const;
			int GetDeviceControlScaled(const int& deviceID, const Device& control, std::pair<int, int> range = std::make_pair(0, 255)) const;
			std::pair<int, int> GetDeviceControlRange(const int& deviceID, const Device& control) const;
			std::vector<Device> GetDeviceLinkedDevices(const int& deviceID, const unsigned int& collectionID) const;
			unsigned int GetDeviceLinkID(const int& deviceID, const Device& control) const;
		//
		
		// resource manager related helper functions
			// [todo] add more helpers
			Manager<Resource>& GetResourceManager();
			
			template <typename T>
			void RegisterResourceType();
			
			template <typename T, typename ...Ps>
			ResourceHandle AddResource(const std::string& name, const Ps&... params);
			
			template <typename T>
			void RemoveResource(const ResourceHandle& handle);
			
			template <typename T>
			void RemoveResource(const std::string& name);
			
			void RemoveResource(const ResourceHandle& handle);
			void RemoveResource(const std::string& name);
			
			template <typename T>
			T& GetResource(const ResourceHandle& handle);
			
			template <typename T>
			std::list< std::reference_wrapper<T> > GetResource(const std::string& name);
		//
		
		// begin entity system helpers...
			EntityManager::Handle AddEntity(const std::string& name);
			
			void RemoveEntity(const EntityManager::Handle& handle);
			void RemoveEntities(const std::string& name);
			void RemoveEntities();
			
			Entity& GetEntity(const EntityManager::Handle& handle);
			std::list< std::reference_wrapper<Entity> > GetEntities(const std::string& name);
			std::list< std::reference_wrapper<Entity> > GetEntities();
			
			std::list<EntityManager::Handle> GetEntityHandles(const std::string& name);
			std::list<EntityManager::Handle> GetEntityHandles();
			
			template <typename T>
			T& RegisterSystem();
			
			template <typename T>
			void RemoveSystem();
			
			template <typename T>
			T& GetSystem();
			
			template <class T>
			void PushMessage(const T& messageIn);
			void PushMessageString(const unsigned int& messageTypeID, const std::string& messageString);
			
			MessageQueue::Entry GetMessage() const;
			void PopMessage();
			void ClearQueue();
			bool IsEmpty() const;
			unsigned int GetMessageCount() const;
		// ...end entity system helpers
	private :
		void HandleMessageQueue(const MessageQueue::Entry& e);
		
		void AddWindow();
		void AddWindow(const std::string & windowTitle, const WindowDisplaySettings & settings,
				const unsigned long & windowStyle = WindowStyles::Visible);
		void AddContext();
		void AddContext(WindowPtr windowPtr);
		void MakeCurrent(WindowPtr windowPtr, OpenGLContextPtr contextPtr);
	
	public :
		bool mFrameLimit = false;
		float mFrameLowerLimit = 0.02f;
		float mFrameUpperLimit = 1.0f;
		
		unsigned int mRenderPasses = 1u;
	private :
		float mTotalFrameTime = 0.0f;
		float mAccumulator = 0.0f;
		Timer mTimer;
		
		WindowPtr mWindow;
		OpenGLContextPtr mContext;
		bool mOpen = true;
		
		MessageQueue mMessageQueue;
		
		SceneManagerPtr mSceneManager;
		InputManagerPtr mInputManager;
		Manager<Resource> mResourceManager;
		
		bool mDefaultShaderExists;
		Shader mDefaultShader;
		
		EntitySystem mEntitySystem;
		
		FT_Library mFTLibrary;
};

template <typename T>
void Game::RegisterResourceType() {
	try {
		mResourceManager.Register<T>();
	} catch (std::exception& e) {
		throw;
	}
}

template <typename T, typename ...Ps>
ResourceHandle Game::AddResource(const std::string& name, const Ps&... params) {
	try {
		return mResourceManager.Add<T, Ps...>(name, params...);
	} catch (std::exception& e) {
		throw;
	}
}

template <typename T>
void Game::RemoveResource(const ResourceHandle& handle) {
	try {
		mResourceManager.Remove<T>(handle);
	} catch (std::exception& e) {
		throw;
	}
}

template <typename T>
void Game::RemoveResource(const std::string& name) {
	try {
		mResourceManager.Remove<T>(name);
	} catch (std::exception& e) {
		throw;
	}
}

template <typename T>
T& Game::GetResource(const ResourceHandle& handle) {
	try {
		return mResourceManager.Get<T>(handle);
	} catch (std::exception& e) {
		throw;
	}
}

template <typename T>
std::list< std::reference_wrapper<T> > Game::GetResource(const std::string& name) {
	try {
		return mResourceManager.Get<T>(name);
	} catch (std::exception& e) {
		throw;
	}
}

// begin entity system helpers...
	template <typename T>
	T& Game::RegisterSystem() {
		try {
			return mEntitySystem.RegisterSystem<T>();
		} catch (std::exception& e) {
			throw;
		}
	}
	
	template <typename T>
	void Game::RemoveSystem() {
		try {
			mEntitySystem.RemoveSystem<T>();
		} catch (std::exception& e) {
			throw;
		}
	}
	
	template <typename T>
	T& Game::GetSystem() {
		try {
			return mEntitySystem.GetSystem<T>();
		} catch (std::exception& e) {
			throw;
		}
	}
	
	template <class T>
	void Game::PushMessage(const T& messageIn) {
		try {
			return mEntitySystem.PushMessage<T>(messageIn);
		} catch (std::exception& e) {
			throw;
		}
	}
// ...end entity system helpers
}

#endif
