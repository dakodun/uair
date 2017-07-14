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

#ifndef UAIRHVECTOR_HPP
#define UAIRHVECTOR_HPP

#include "init.hpp"

namespace uair {
template <class T>
class HVector {
	public :
		// an entry into the store that holds the object as well
		// as data to ensure validity
		class Entry {
			public :
				// create an entry (T) by copying or moving an
				// existing object
				explicit Entry(const T& value);
				
				// create an entry (T) in place using constructor parameters
				template <typename ...Ps>
				explicit Entry(Ps&&... params);
				
				// std::unique_ptr can't be copied
				Entry(const Entry& other) = delete;
				Entry(Entry&& other); // std::unique_ptr can be moved
				
				Entry& operator=(Entry other);
				
				friend void swap(Entry& first, Entry& second) {
					using std::swap;
					
					swap(first.mActive, second.mActive);
					swap(first.mCounter, second.mCounter);
					swap(first.mData, second.mData);
				}
			
			public :
				// is the entry active (i.e., has it been removed)
				bool mActive = true;
				
				// the counter that ensures handles are still valid
				// (i.e., its resource hasn't been removed and replaced)
				unsigned int mCounter = 0u;
				
				// a pointer to an object of the type that
				// this store handles
				std::unique_ptr<T> mData;
		};
		
		// a handle that is used to refer to resources handled
		// instead of a pointer
		class Handle {
			public :
				Handle() = default;
				
				explicit Handle(const unsigned int& index,
						const unsigned int& counter) : mIndex(index),
						mCounter(counter) {
					
				}
			public :
				// the index of the resource in the store
				unsigned int mIndex = 0u;
				
				// the counter value of the index used to
				// validate the handle
				unsigned int mCounter = 0u;
		};
	
	public :
		HVector();
		explicit HVector(const unsigned int& reserveCap);
		
		// std::unique_ptr can't be copied
		HVector(const HVector& other) = delete;
		HVector(HVector&& other); // std::unique_ptr can be moved
		
		HVector& operator=(HVector other);
		
		friend void swap(HVector& first, HVector& second) {
			using std::swap;
			
			swap(first.mReserveCap, second.mReserveCap);
			
			swap(first.mStore, second.mStore);
			swap(first.mFreeIndices, second.mFreeIndices);
		}
		
		// add an entry to the store via copying
		Handle Push(const T& value);
		
		// add an entry to the store using move semantics
		Handle Push(T&& value);
		
		template <typename ...Ps>
		Handle Emplace(Ps&&... params); // construct an entry in the store
		
		void Pop(const Handle& handle); // remove an entry from the store
		
		// return a reference to an entry in the store
		T& Get(const Handle& handle);
		
		unsigned int Size() const;
	
	public :
		// how many slots to reserve in the store
		unsigned int mReserveCap = 100u;
	private :
		std::vector<Entry> mStore; // the resources handled by this store
		
		// a sequential list (ascending) of indices that were previously
		// occupied but are now freed and available for re-use
		std::priority_queue<unsigned int, std::vector<unsigned int>,
				std::greater<unsigned int>> mFreeIndices;
};
}

#include "hvector.inl"
#endif
