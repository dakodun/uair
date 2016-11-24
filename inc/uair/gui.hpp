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

#ifndef UAIRGUI_HPP
#define UAIRGUI_HPP

#include <memory>

#include "manager.hpp"
#include "inputmanager.hpp"
#include "messagequeue.hpp"

namespace uair {
class RenderBatch;

class GUI;
class GUIElement {
	public :
		virtual void HandleMessageQueue(const MessageQueue::Entry& e, GUI* caller = nullptr) {
			
		}
		
		virtual void Input(GUI* caller = nullptr) {
			
		}
		
		virtual void Process(float deltaTime, GUI* caller = nullptr) {
			
		}
		
		virtual void PostProcess(const unsigned int& processed, float deltaTime, GUI* caller = nullptr) {
			
		}
		
		virtual void AddToBatch(RenderBatch& batch, GUI* caller = nullptr) {
			
		}
};

typedef Manager<GUIElement>::Handle ElementHandle;
enum class GUIElements : unsigned int {
	Button = 1u,
	CheckBox,
	InputBox
};

class GUI {
	public :
		GUI();
		
		// begin...
			template <typename T>
			void RegisterElementType();
			
			template <typename T, typename ...Ps>
			ElementHandle AddElement(const std::string& name, const Ps&... params);
			
			template <typename T>
			void RemoveElement(const ElementHandle& handle);
			
			template <typename T>
			void RemoveElement(const std::string& name);
			
			void RemoveElement(const ElementHandle& handle);
			void RemoveElement(const std::string& name);
			
			template <typename T>
			T& GetElement(const ElementHandle& handle);
			
			template <typename T>
			std::list< std::reference_wrapper<T> > GetElement(const std::string& name);
		// ...end
		
		void HandleMessageQueue(const MessageQueue::Entry& e);
		void Input();
		void Process(float deltaTime);
		void PostProcess(const unsigned int& processed, float deltaTime);
		
		void AddToBatch(RenderBatch& batch);
	private :
		static bool CompareHandles(const ElementHandle& first, const ElementHandle& second);
	
	public :
		static std::weak_ptr<InputManager> mInputManagerPtr;
		static MessageQueue* mMessageQueuePtr;
		
		bool mUpdated = false;
	private :
		Manager<GUIElement> mElementManager;
		std::set< ElementHandle, std::function<bool (const ElementHandle&,
				const ElementHandle&)> > mElementHandles;
};

template <typename T>
void GUI::RegisterElementType() {
	try {
		mElementManager.Register<T>();
	} catch (std::exception& e) {
		throw;
	}
}

template <typename T, typename ...Ps>
ElementHandle GUI::AddElement(const std::string& name, const Ps&... params) {
	try {
		auto handle = mElementManager.Add<T, Ps...>(name, params...);
		
		mElementHandles.insert(handle);
		mUpdated = true;
		
		return handle;
	} catch (std::exception& e) {
		throw;
	}
}

template <typename T>
void GUI::RemoveElement(const ElementHandle& handle) {
	try {
		auto result = mElementHandles.find(handle);
		if (result != mElementHandles.end()) {
			mElementHandles.erase(result);
			mUpdated = true;
		}
		
		mElementManager.Remove<T>(handle);
	} catch (std::exception& e) {
		throw;
	}
}

template <typename T>
void GUI::RemoveElement(const std::string& name) {
	try {
		auto handles = mElementManager.GetHandles<T>(name);
		for (auto handleIter = handles.begin(); handleIter != handles.end(); ++handleIter) {
			auto result = mElementHandles.find(*handleIter);
			if (result != mElementHandles.end()) {
				mElementHandles.erase(result);
				mUpdated = true;
			}
		}
		
		mElementManager.Remove<T>(name);
	} catch (std::exception& e) {
		throw;
	}
}

template <typename T>
T& GUI::GetElement(const ElementHandle& handle) {
	try {
		return mElementManager.Get<T>(handle);
	} catch (std::exception& e) {
		throw;
	}
}

template <typename T>
std::list< std::reference_wrapper<T> > GUI::GetElement(const std::string& name) {
	try {
		return mElementManager.Get<T>(name);
	} catch (std::exception& e) {
		throw;
	}
}
}

#endif
