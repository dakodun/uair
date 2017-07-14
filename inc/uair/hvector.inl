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
HVector<T>::Entry::Entry(const T& value) {
	mData = std::make_unique<T>(value);
}

template <class T>
template <typename ...Ps>
HVector<T>::Entry::Entry(Ps&&... params) {
	mData = std::make_unique<T>(std::forward<Ps>(params)...);
}

template <class T>
HVector<T>::Entry::Entry(Entry&& other) : Entry() {
	swap(*this, other);
}

template <class T>
typename HVector<T>::Entry& HVector<T>::Entry::operator=(Entry other) {
	swap(*this, other);
	
	return *this;
}


template <class T>
HVector<T>::HVector() {
	// reserve initial space to cut back on re-allocations
	mStore.reserve(mReserveCap);
}

template <class T>
HVector<T>::HVector(const unsigned int& reserveCap) :
		mReserveCap(reserveCap) {
	
	mStore.reserve(mReserveCap);
}

template <class T>
HVector<T>::HVector(HVector&& other) : HVector() {
	swap(*this, other);
}

template <class T>
HVector<T>& HVector<T>::operator=(HVector other) {
	swap(*this, other);
	
	return *this;
}

template <class T>
typename HVector<T>::Handle HVector<T>::Push(const T& value) {
	// if there exists a previously used index that is now free...
	if (!mFreeIndices.empty()) {
		// get the lowest free index
		unsigned int freeIndex = mFreeIndices.top();
		
		// if the free index is valid and the resource located
		// there has been removed...
		if (freeIndex < mStore.size() && !mStore.at(freeIndex).mActive) {
			// assign the resource, set as active and then remove the
			// (no longer) free index from the queue
			mStore.at(freeIndex).mData = std::make_unique<T>(value);
			mStore.at(freeIndex).mActive = true;
			mFreeIndices.pop();
			
			return Handle(freeIndex, mStore.at(freeIndex).mCounter);
		}
		
		throw std::runtime_error("Store Error: "
				"index marked as free is invalid.");
	}
	else { // otherwise if there are no free indices...
		// if adding a new entry will breach the reserved
		// space for the store...
		if (mStore.size() + 1 > mStore.capacity()) {
			// reserve a chunk more space
			mStore.reserve(mStore.capacity() + mReserveCap);
		}
		
		// add a new entry to the end of the store
		mStore.emplace_back(value);
		
		return Handle(mStore.size() - 1, 0);
	}
}

template <class T>
typename HVector<T>::Handle HVector<T>::Push(T&& value) {
	if (!mFreeIndices.empty()) {
		unsigned int freeIndex = mFreeIndices.top();
		
		if (freeIndex < mStore.size() && !mStore.at(freeIndex).mActive) {
			mStore.at(freeIndex).mData =
					std::make_unique<T>(std::forward<T>(value));
			mStore.at(freeIndex).mActive = true;
			mFreeIndices.pop();
			
			return Handle(freeIndex, mStore.at(freeIndex).mCounter);
		}
		
		throw std::runtime_error("Store Error: "
				"index marked as free is invalid.");
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
typename HVector<T>::Handle HVector<T>::Emplace(Ps&&... params) {
	if (!mFreeIndices.empty()) {
		unsigned int freeIndex = mFreeIndices.top();
		
		if (freeIndex < mStore.size() && !mStore.at(freeIndex).mActive) {
			mStore.at(freeIndex).mData =
					std::make_unique<T>(std::forward<Ps>(params)...);
			mStore.at(freeIndex).mActive = true;
			mFreeIndices.pop();
			
			return Handle(freeIndex, mStore.at(freeIndex).mCounter);
		}
		
		throw std::runtime_error("Store Error: "
				"index marked as free is invalid.");
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
void HVector<T>::Pop(const Handle& handle) {
	// if the index is valid, the resource has not been removed
	// already and the counters match...
	if (handle.mIndex < mStore.size() && mStore.at(handle.mIndex).mActive &&
			mStore.at(handle.mIndex).mCounter == handle.mCounter) {
		
		// indicate the resource has been removed
		mStore.at(handle.mIndex).mActive = false;
		
		// increment the counter to invalidate existing handles
		++mStore.at(handle.mIndex).mCounter;
		
		// delete the handled object
		mStore.at(handle.mIndex).mData.reset(nullptr);
		
		// add its index to the sorted queue of now free indices
		mFreeIndices.push(handle.mIndex);
	}
	else {
		throw std::runtime_error("Store Error: handle is invalid.");
	}
}

template <class T>
T& HVector<T>::Get(const Handle& handle) {
	// if the index is out of bounds...
	if (handle.mIndex >= mStore.size()) {
		throw std::runtime_error("Store Error: "
				"handle index out of bounds.");
	}
	
	if (!mStore.at(handle.mIndex).mActive) { // if the index isn't in use...
		throw std::runtime_error("Store Error: handle index isn't in use.");
	}
	
	// if the counter values don't match...
	if (mStore.at(handle.mIndex).mCounter != handle.mCounter) {
		throw std::runtime_error("Store Error: handle counter mismatch.");
	}
	
	// return a reference to the object stored
	return *(mStore.at(handle.mIndex).mData.get());
}

template <class T>
unsigned int HVector<T>::Size() const {
	return mStore.size();
}
}
