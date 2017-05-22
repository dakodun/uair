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

#ifndef UAIRRESOURCEPTR_HPP
#define UAIRRESOURCEPTR_HPP

#include <iostream>

#include "init.hpp"

namespace uair {
class EXPORTDLL ResourcePtrBase {
	friend class Resource;
	
	private :
		virtual void Reset() = 0;
		
		unsigned int GetID() const {
			return mID;
		}
	
	protected :
		unsigned int mID = 0u;
};

template<class T>
class EXPORTDLL ResourcePtr : public ResourcePtrBase {
	friend class Resource;
	
	public :
		ResourcePtr() = default;
		
		ResourcePtr(T* ptr) : mPtr(ptr) {
			AddReference();
		}
		
		ResourcePtr(const ResourcePtr<T>& other) : ResourcePtr() {
			mPtr = other.mPtr;
			AddReference();
		}
		
		ResourcePtr(ResourcePtr<T>&& other) : ResourcePtr() {
			mPtr = other.mPtr;
			other.mPtr = nullptr;
			
			mID = other.mID;
			other.mID = 0u;
			
			AddReference(mID);
		}
		
		~ResourcePtr() {
			RemoveReference();
			Reset();
		}
		
		ResourcePtr<T>& operator=(ResourcePtr<T> other) {
			swap(*this, other);
			
			return *this;
		}
		
		friend void swap(ResourcePtr<T>& first, ResourcePtr<T>& second) {
			using std::swap;
			
			swap(first.mPtr, second.mPtr);
			swap(first.mID, second.mID);
			
			first.AddReference(first.mID);
			second.AddReference(second.mID);
		}
		
		void Set(T* ptr) {
			if (ptr != mPtr) {
				RemoveReference();
				
				mPtr = ptr;
				AddReference();
			}
		}
		
		T& operator*() const {
			return *mPtr;
		}
		
		T* operator->() const {
			return mPtr;
		}
		
		T* Get() {
			return mPtr;
		}
		
		const T* GetConst() const {
			return mPtr;
		}
		
		void Unset() {
			RemoveReference();
			Reset();
		}
		
		explicit operator bool() const {
			if (mPtr) {
				return true;
			}
			
			return false;
		}
	private :
		void Reset() {
			mPtr = nullptr;
			mID = 0u;
		}
		
		void AddReference(const unsigned int& id = 0u) {
			if (mPtr) {
				mID = mPtr->AddReference(this, id);
			}
		}
		
		void RemoveReference() {
			if (mPtr) {
				mID = mPtr->RemoveReference(this);
			}
		}
	private :
		T* mPtr = nullptr;
};
}

#endif
