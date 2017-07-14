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

#ifndef UAIRREFPTR_HPP
#define UAIRREFPTR_HPP

#include <iostream>
#include <map>

#include "init.hpp"

namespace uair {
class RefType;

// base reference counted pointer class allows manipulation of templated
// reference pointers without knowledge of templated type (T)
class EXPORTDLL RefPtrBase {
	friend class RefType;
	
	public :
		virtual ~RefPtrBase() {
			
		}
	protected :
		// allows an associated reftype to reset a pointer
		// back to its default state
		virtual void Reset() = 0;
		
		// allows an associated reftype to point a pointer at itself
		// without triggering an reference counting
		virtual void SetDirect(RefType* ptr) = 0;
	
	protected :
		// the unique id (within associated reftype) of this pointer
		unsigned int mID = 0u;
};


// templated reference counted pointer class which manages a pointer
// to a class derived from reftype class (T)
template <class T>
class RefPtr : public RefPtrBase {
	public :
		RefPtr() = default;
		RefPtr(T* ptr);
		RefPtr(const RefPtr& other);
		RefPtr(RefPtr&& other);
		
		~RefPtr();
		
		RefPtr<T>& operator=(RefPtr other);
		T& operator*();
		T* operator->();
		explicit operator bool() const;
		
		friend void swap(RefPtr& first, RefPtr& second) {
			using std::swap;
			
			swap(first.mPtr, second.mPtr);
			swap(first.mID, second.mID);
			
			// update reference of both ptrs to point to new refptr
			first.AddReference(first.mID);
			second.AddReference(second.mID);
		}
		
		void Set(T* ptr);
		T* Get();
		const T* Get() const;
		void Unset();
	protected :
		void Reset();
		void SetDirect(RefType* ptr);
	private :
		// add a reference to the associated reftype (if any)
		void AddReference(const unsigned int& id = 0u);
		
		// remove a reference from the associated reftype (if any)
		void RemoveReference();
	
	private :
		T* mPtr = nullptr;
	
	#ifdef _UAIR_DEBUG
	public :
		friend std::ostream& operator<<(std::ostream& os,
				const RefPtr& obj) {
			
			if (obj.mPtr) {
				os << obj.mPtr << ":";
			}
			else {
				os << "nullptr:";
			}
			
			os << obj.mID;
			
			return os;
		}
	#endif
};

// base class that allows use of reference counted pointers (via refptr)
// for classes that derive from it
class EXPORTDLL RefType {
	public :
		RefType() = default;
		RefType(const RefType& other) = delete;
		RefType(RefType&& other);
		
		virtual ~RefType();
		
		RefType& operator=(RefType other);
		
		friend void swap(RefType& first, RefType& second) {
			using std::swap;
			
			swap(first.mStore, second.mStore);
			swap(first.mCounter, second.mCounter);
			
			// update all ptrs to point at correct reftype
			first.SetAll();
			second.SetAll();
		}
		
		// register a refptr with this reftype
		unsigned int AddReference(RefPtrBase* refPtr,
			const unsigned int& id = 0u);
		
		// unregister a refptr from this reftype
		unsigned int RemoveReference(RefPtrBase* refPtr);
	protected :
		// ensure all refptrs that are registered to this reftype
		// actually point at this reftype
		void SetAll();
	
	protected :
		// store that associates all registered refptrs with their uid
		std::map<unsigned int, RefPtrBase*> mStore;
		
		// counter used to assign a unique id to registered refptrs
		unsigned int mCounter = 0u;
	
	#ifdef _UAIR_DEBUG
	public :
		friend std::ostream& operator<<(std::ostream& os,
				const RefType& obj) {
			
			os << obj.mStore.size() << ":" << obj.mCounter;
			
			return os;
		}
	#endif
};
}

#include "refptr.inl"
#endif
