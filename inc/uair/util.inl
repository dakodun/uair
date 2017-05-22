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
namespace util {
template <class T>
HandleStore<T>::Entry::Entry(const T& value) {
	mData = std::make_unique<T>(value);
}

template <class T>
template <typename ...Ps>
HandleStore<T>::Entry::Entry(Ps&&... params) {
	mData = std::make_unique<T>(std::forward<Ps>(params)...);
}

template <class T>
HandleStore<T>::Entry::Entry(Entry&& other) : Entry() {
	swap(*this, other);
}

template <class T>
typename HandleStore<T>::Entry& HandleStore<T>::Entry::operator=(Entry other) {
	swap(*this, other);
	
	return *this;
}


template <class T>
HandleStore<T>::HandleStore() {
	mStore.reserve(mReserveCap); // reserve initial space to cut back on re-allocations
}

template <class T>
HandleStore<T>::HandleStore(const unsigned int& reserveCap) : mReserveCap(reserveCap) {
	mStore.reserve(mReserveCap);
}

template <class T>
HandleStore<T>::HandleStore(HandleStore&& other) : HandleStore() {
	swap(*this, other);
}

template <class T>
HandleStore<T>& HandleStore<T>::operator=(HandleStore other) {
	swap(*this, other);
	
	return *this;
}

template <class T>
typename HandleStore<T>::Handle HandleStore<T>::Push(const T& value) {
	if (!mFreeIndices.empty()) { // if there exists a previously used index that is now free...
		unsigned int freeIndex = mFreeIndices.top(); // get the lowest free index
		
		// if the free index is valid and the resource located there has been removed...
		if (freeIndex < mStore.size() && !mStore.at(freeIndex).mActive) {
			// assign the resource, set as active and then remove the (no longer) free index from the queue
			mStore.at(freeIndex).mData = std::make_unique<T>(value);
			mStore.at(freeIndex).mActive = true;
			mFreeIndices.pop();
			
			return Handle(freeIndex, mStore.at(freeIndex).mCounter);
		}
		
		throw std::runtime_error("Store Error: index marked as free is invalid.");
	}
	else { // otherwise if there are no free indices...
		if (mStore.size() + 1 > mStore.capacity()) { // if adding a new entry will breach the reserved space for the store...
			mStore.reserve(mStore.capacity() + mReserveCap); // reserve a chunk more space
		}
		
		// add a new entry to the end of the store
		mStore.emplace_back(value);
		
		return Handle(mStore.size() - 1, 0);
	}
}

template <class T>
typename HandleStore<T>::Handle HandleStore<T>::Push(T&& value) {
	if (!mFreeIndices.empty()) {
		unsigned int freeIndex = mFreeIndices.top();
		
		if (freeIndex < mStore.size() && !mStore.at(freeIndex).mActive) {
			mStore.at(freeIndex).mData = std::make_unique<T>(std::forward<T>(value));
			mStore.at(freeIndex).mActive = true;
			mFreeIndices.pop();
			
			return Handle(freeIndex, mStore.at(freeIndex).mCounter);
		}
		
		throw std::runtime_error("Store Error: index marked as free is invalid.");
	}
	else {
		if (mStore.size() + 1 > mStore.capacity()) {
			mStore.reserve(mStore.capacity() + mReserveCap);
		}
		
		mStore.emplace_back(std::forward<T>(value));
		
		return Handle(mStore.size() - 1, 0);
	}
}

template <class T>
template <typename ...Ps>
typename HandleStore<T>::Handle HandleStore<T>::Emplace(Ps&&... params) {
	if (!mFreeIndices.empty()) {
		unsigned int freeIndex = mFreeIndices.top();
		
		if (freeIndex < mStore.size() && !mStore.at(freeIndex).mActive) {
			mStore.at(freeIndex).mData = std::make_unique<T>(std::forward<Ps>(params)...);
			mStore.at(freeIndex).mActive = true;
			mFreeIndices.pop();
			
			return Handle(freeIndex, mStore.at(freeIndex).mCounter);
		}
		
		throw std::runtime_error("Store Error: index marked as free is invalid.");
	}
	else {
		if (mStore.size() + 1 > mStore.capacity()) {
			mStore.reserve(mStore.capacity() + mReserveCap);
		}
		
		mStore.emplace_back(std::forward<Ps>(params)...);
		
		return Handle(mStore.size() - 1, 0);
	}
}

template <class T>
void HandleStore<T>::Pop(const Handle& handle) {
	// if the index is valid, the resource has not been removed already and the counters match...
	if (handle.mIndex < mStore.size() && mStore.at(handle.mIndex).mActive &&
			mStore.at(handle.mIndex).mCounter == handle.mCounter) {
		
		mStore.at(handle.mIndex).mActive = false; // indicate the resource has been removed
		++mStore.at(handle.mIndex).mCounter; // increment the counter to invalidate existing handles
		mStore.at(handle.mIndex).mData.reset(nullptr); // delete the handled object
		mFreeIndices.push(handle.mIndex); // add its index to the sorted queue of now free indices
	}
	else {
		throw std::runtime_error("Store Error: handle is invalid.");
	}
}

template <class T>
T& HandleStore<T>::Get(const Handle& handle) {
	if (handle.mIndex >= mStore.size()) { // if the index is out of bounds...
		throw std::runtime_error("Store Error: handle index out of bounds.");
	}
	
	if (!mStore.at(handle.mIndex).mActive) { // if the index isn't in use...
		throw std::runtime_error("Store Error: handle index isn't in use.");
	}
	
	if (mStore.at(handle.mIndex).mCounter != handle.mCounter) { // if the counter values don't match...
		throw std::runtime_error("Store Error: handle counter mismatch.");
	}
	
	return *(mStore.at(handle.mIndex).mData.get()); // return a reference to the object stored
}

template <class T>
unsigned int HandleStore<T>::Size() const {
	return mStore.size();
}
}
}
