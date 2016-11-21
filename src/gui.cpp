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

#include "gui.hpp"

#include "renderbatch.hpp"

namespace uair {
std::weak_ptr<InputManager> GUI::mInputManagerPtr = std::weak_ptr<InputManager>();
MessageQueue* GUI::mMessageQueuePtr = nullptr;

GUI::GUI() : mElementHandles(CompareHandles) {
	
}

void GUI::RemoveElement(const ElementHandle& handle) {
	try {
		auto result = mElementHandles.find(handle);
		if (result != mElementHandles.end()) {
			mElementHandles.erase(result);
			mUpdated = true;
		}
		
		mElementManager.Remove(handle);
	} catch (std::exception& e) {
		throw;
	}
}

void GUI::RemoveElement(const std::string& name) {
	try {
		auto handles = mElementManager.GetHandles(name);
		for (auto handleIter = handles.begin(); handleIter != handles.end(); ++handleIter) {
			auto result = mElementHandles.find(*handleIter);
			if (result != mElementHandles.end()) {
				mElementHandles.erase(result);
				mUpdated = true;
			}
		}
		
		mElementManager.Remove(name);
	} catch (std::exception& e) {
		throw;
	}
}

void GUI::HandleMessageQueue(const MessageQueue::Entry& e) {
	for (auto eleHandle = mElementHandles.begin(); eleHandle != mElementHandles.end(); ++eleHandle) {
		auto ele = mElementManager.GetAsBase(*eleHandle);
		ele->HandleMessageQueue(e, this);
	}
}

void GUI::Input() {
	for (auto eleHandle = mElementHandles.begin(); eleHandle != mElementHandles.end(); ++eleHandle) {
		auto ele = mElementManager.GetAsBase(*eleHandle);
		ele->Input(this);
	}
}

void GUI::Process(float deltaTime) {
	for (auto eleHandle = mElementHandles.begin(); eleHandle != mElementHandles.end(); ++eleHandle) {
		auto ele = mElementManager.GetAsBase(*eleHandle);
		ele->Process(deltaTime, this);
	}
}

void GUI::PostProcess(const unsigned int& processed, float deltaTime) {
	for (auto eleHandle = mElementHandles.begin(); eleHandle != mElementHandles.end(); ++eleHandle) {
		auto ele = mElementManager.GetAsBase(*eleHandle);
		ele->PostProcess(processed, deltaTime, this);
	}
}

void GUI::AddToBatch(RenderBatch& batch) {
	for (auto eleHandle = mElementHandles.begin(); eleHandle != mElementHandles.end(); ++eleHandle) {
		auto ele = mElementManager.GetAsBase(*eleHandle);
		ele->AddToBatch(batch, this);
		mUpdated = false;
	}
}

bool GUI::CompareHandles(const ElementHandle& first, const ElementHandle& second) {
	if (first.mTypeID < second.mTypeID) {
		return false;
	}
	else if (first.mTypeID == second.mTypeID) {
		if (first.mIndex < second.mIndex || first.mIndex == second.mIndex) {
			return false;
		}
	}
	
	return true;
}
}
