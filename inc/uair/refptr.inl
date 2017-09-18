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

namespace uair {
template <class T>
RefPtr<T>::RefPtr(T* ptr) : mPtr(ptr) {
	if (mPtr) { // if we have a valid object pointer...
		AddReference(); // add a reference to the object pointed to by ptr
	}
}

template <class T>
RefPtr<T>::RefPtr(const RefPtr& other) : mPtr(other.mPtr) {
	if (mPtr) {
		AddReference();
	}
}

template <class T>
RefPtr<T>::RefPtr(RefPtr&& other) : RefPtr() {
	swap(*this, other);
}

template <class T>
RefPtr<T>::~RefPtr() {
	// remove reference from ptr and reset this back to a default state
	RemoveReference();
	Reset();
}

template <class T>
RefPtr<T>& RefPtr<T>::operator=(RefPtr other) {
	swap(*this, other);
	
	return *this;
}

template <class T>
T& RefPtr<T>::operator*() {
	return *mPtr;
}

template <class T>
const T& RefPtr<T>::operator*() const {
	return *mPtr;
}

template <class T>
T* RefPtr<T>::operator->() {
	return mPtr;
}

template <class T>
const T* RefPtr<T>::operator->() const {
	return mPtr;
}

template <class T>
RefPtr<T>::operator bool() const {
	if (mPtr) { // if we have a valid object pointer...
		return true;
	}
	
	// otherwise no valid obhect pointer (nullptr)
	return false;
}

template <class T>
void RefPtr<T>::Set(T* ptr) {
	if (ptr != mPtr) { // if new ptr isn't the same as current ptr...
		RemoveReference(); // remove reference from current ptr
		
		// update ptr and add a reference to it
		mPtr = ptr;
		AddReference();
	}
}

template <class T>
T* RefPtr<T>::Get() {
	return mPtr;
}

template <class T>
const T* RefPtr<T>::Get() const {
	return mPtr;
}

template <class T>
void RefPtr<T>::Unset() {
	RemoveReference();
	Reset();
}

template <class T>
void RefPtr<T>::Reset() {
	mPtr = nullptr;
	mID = 0u;
}

template <class T>
void RefPtr<T>::SetDirect(RefType* ptr) {
	// set the ptr object without modifying any references
	mPtr = static_cast<T*>(ptr);
}

template <class T>
void RefPtr<T>::AddReference(const unsigned int& id) {
	if (mPtr) { // if current ptr exists...
		// add a reference to current ptr object and store the returned id
		mID = mPtr->AddReference(this, id);
	}
}

template <class T>
void RefPtr<T>::RemoveReference() {
	if (mPtr) { // if current ptr exists...
		// remove reference from current ptr object and store
		// the returned id (0)
		mID = mPtr->RemoveReference(this);
	}
}
}
