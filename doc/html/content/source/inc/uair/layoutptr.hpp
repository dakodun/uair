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

#ifndef UAIRLAYOUTPTR_HPP
#define UAIRLAYOUTPTR_HPP

#include <iostream>

namespace uair {
class LayoutPtrBase {
	friend class LayoutBase;
	
	private :
		virtual void Reset() = 0;
		
		unsigned int GetID() const {
			return mID;
		}
	
	protected :
		unsigned int mID = 0u;
};

template<class T>
class LayoutPtr : public LayoutPtrBase {
	friend class LayoutBase;
	
	public :
		LayoutPtr() = default;
		
		LayoutPtr(T* ptr) : mPtr(ptr) {
			AddReference();
		}
		
		LayoutPtr(const LayoutPtr<T>& other) : LayoutPtr() {
			mPtr = other.mPtr;
			AddReference();
		}
		
		LayoutPtr(LayoutPtr<T>&& other) : LayoutPtr() {
			mPtr = other.mPtr;
			other.mPtr = nullptr;
			
			mID = other.mID;
			other.mID = 0u;
			
			AddReference(mID);
		}
		
		~LayoutPtr() {
			RemoveReference();
			Reset();
		}
		
		LayoutPtr<T>& operator=(LayoutPtr<T> other) {
			swap(*this, other);
			
			return *this;
		}
		
		friend void swap(LayoutPtr<T>& first, LayoutPtr<T>& second) {
			std::swap(first.mPtr, second.mPtr);
			std::swap(first.mID, second.mID);
			
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
