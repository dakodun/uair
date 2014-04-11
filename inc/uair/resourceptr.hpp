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

#ifndef UAIRRESOURCEPTR_HPP
#define UAIRRESOURCEPTR_HPP

namespace uair {
template<class T>
class ResourcePtr {
	public :
		ResourcePtr(T* ptr = nullptr) : mPtr(ptr) {
			AddReference();
		}
		
		ResourcePtr(const ResourcePtr<T>& other) {
			RemoveReference();
			
			mPtr = other.mPtr;
			AddReference();
		}
		
		ResourcePtr(ResourcePtr<T>&& other) {
			RemoveReference();
			
			mPtr = other.mPtr;
			AddReference(other.mID);
			
			other.mPtr = nullptr;
			other.mID = 0u;
		}
		
		~ResourcePtr() {
			RemoveReference();
			mPtr = nullptr;
		}
		
		ResourcePtr<T>& operator=(const ResourcePtr<T>& other) {
			if (mPtr != other.mPtr || mID != other.mID) {
				RemoveReference();
				
				mPtr = other.mPtr;
				AddReference();
			}
			
			return *this;
		}
		
		ResourcePtr<T>& operator=(ResourcePtr<T>&& other) {
			if (mPtr != other.mPtr || mID != other.mID) {
				RemoveReference();
				
				mPtr = other.mPtr;
				AddReference(other.mID);
				
				other.mPtr = nullptr;
				other.mID = 0u;
			}
			
			return *this;
		}
		
		void SetResource(T* ptr) {
			if (ptr != mPtr) {
				RemoveReference();
				
				mPtr = ptr;
				AddReference();
			}
		}
		
		T* GetResource() {
			return mPtr;
		}
		
		void UnsetResource() {
			RemoveReference();
			mPtr = nullptr;
		}
		
		void AddReference(const unsigned int &id = 0u) {
			if (mPtr) {
				mID = mPtr->AddReference(this, id);
			}
		}
		
		void RemoveReference() {
			if (mPtr) {
				mID = mPtr->RemoveReference(this);
			}
		}
		
		unsigned int GetID() const {
			return mID;
		}
	private :
		T* mPtr = nullptr;
		unsigned int mID = 0u;
};
}

#endif
