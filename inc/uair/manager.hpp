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

#ifndef UAIRMANAGER_HPP
#define UAIRMANAGER_HPP

#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <list>
#include <functional>
#include <memory>

namespace uair {
// abstract base class that all typed stores inherit from ensuring they have the same base pointer
// B is the base type managed by the store (can be the same as the derived type)
template <class B>
class StoreBase {
	public :
		// virtual destructor allows deletion of derived type using base pointer
		virtual ~StoreBase() {
			
		}
		
		// virtual remove function allows us to delete an entry without knowing its type (using only its handle)
		virtual void Remove(const unsigned int& index, const unsigned int& counter) = 0;
		
		// virtual remove function allows us to delete entries without knowing their type (using only a name)
		virtual void Remove(const std::string& name) = 0;
		
		// virtual remove function allows us to delete ALL entries without knowing their type
		virtual void Remove() = 0;
		
		// virtual get function allows us to return an entry as a base pointer
		virtual B* GetAsBase(const unsigned int& index, const unsigned int& counter) const = 0;
		
		// virtual get function allows us to return handles to entries without knowing their type (using only a name)
		virtual std::list< std::tuple<unsigned int, unsigned int, std::string> > GetHandles(const std::string& name) const = 0;
		
		// virtual get function allows us to return handles to ALL entries without knowing their type
		virtual std::list< std::tuple<unsigned int, unsigned int, std::string> > GetHandles() const = 0;
};

// templated store class that maintains resources of the specified derived type (T)
// B is the base type managed by the store (can be the same as the derived type)
template <class B, class T>
class Store : public StoreBase<B> {
	public :
		// an entry into the store that holds the object as well as data to ensure validity
		class StoreEntry {
			public :
				template <typename ...Ps>
				StoreEntry(const std::string& name, const Ps&... params) : mName(name), mData(new T(params...)) {
					
				}
			public :
				std::string mName = ""; // the non-unique name associated with this entry
				bool mActive = true; // is the entry active (i.e., has it been removed)
				unsigned int mCounter = 0u; // the counter that ensures handles are still valid (i.e., its resource hasn't been removed and replaced)
				std::unique_ptr<T> mData; // a pointer to an object of the type that this store handles
		};
		
		Store() {
			mStore.reserve(mReserveCap); // reserve initial space to cut back on re-allocations
		}
		
		// construct a new resource and add it to the store
		template <typename ...Ps>
		std::pair<unsigned int, unsigned int> Add(const std::string& name, const Ps&... params) {
			if (!mFreeIndices.empty()) { // if there exists a previously used index that is now free...
				unsigned int freeIndex = mFreeIndices.top(); // get the lowest free index
				
				// if the free index is valid and the resource located there has been removed...
				if (freeIndex < mStore.size() && !mStore.at(freeIndex).mActive) {
					unsigned int counter = mStore.at(freeIndex).mCounter; // get the counter (validity) value
					
					// construct a new resource, assign it the current counter value, move it into the free index and then remove the index from the queue
					StoreEntry newEntry(name, params...); newEntry.mCounter = counter;
					mStore.at(freeIndex) = std::move(newEntry);
					mFreeIndices.pop();
					
					return std::make_pair(freeIndex, counter); // return a pair containing the index that was used and its current counter value
				}
				
				throw std::runtime_error("index marked as free is invalid"); // an error has occurred, free index is improperly marked
			}
			else { // otherwise if there are no free indices...
				if (mStore.size() + 1 > mStore.capacity()) { // if adding a new entry will breach the reserved space for the store...
					mStore.reserve(mStore.capacity() + mReserveCap); // reserve a chunk more space
				}
				
				mStore.emplace_back(name, params...); // add a new entry to the store
				return std::make_pair(mStore.size() - 1, 0); // return a pair containing the new index and a counter value of 0 (first entry)
			}
		}
		
		// remove a resource from the store given its index and counter (validity) value
		void Remove(const unsigned int& index, const unsigned int& counter) {
			// if the index is valid, the resource has not been removed already and the counters match...
			if (index < mStore.size() && mStore.at(index).mActive && mStore.at(index).mCounter == counter) {
				mStore.at(index).mActive = false; // indicate the resource has been removed
				++mStore.at(index).mCounter; // increment the counter to invalidate existing handles
				mStore.at(index).mData.reset(nullptr); // delete the handled object
				mFreeIndices.push(index); // add its index to the sorted queue of now free indices
			}
			else {
				throw std::runtime_error("handle is invalid"); // an error has occurred, handle is invalid
			}
		}
		
		// remove all resources from the store with matching name
		void Remove(const std::string& name) {
			for (unsigned int i = 0u; i < mStore.size(); ++i) { // for all resources in the store...
				if (mStore.at(i).mActive && mStore.at(i).mName == name) {
					mStore.at(i).mActive = false; // indicate the resource has been removed
					++mStore.at(i).mCounter; // increment the counter to invalidate existing handles
					mStore.at(i).mData.reset(nullptr); // delete the handled object
					mFreeIndices.push(i); // add its index to the sorted queue of now free indices
				}
			}
		}
		
		void Remove() {
			for (unsigned int i = 0u; i < mStore.size(); ++i) { // for all resources in the store...
				if (mStore.at(i).mActive) {
					mStore.at(i).mActive = false; // indicate the resource has been removed
					++mStore.at(i).mCounter; // increment the counter to invalidate existing handles
					mStore.at(i).mData.reset(nullptr); // delete the handled object
					mFreeIndices.push(i); // add its index to the sorted queue of now free indices
				}
			}
		}
		
		// return a reference to a handled resource given its index and counter (validity) value
		T& Get(const unsigned int& index, const unsigned int& counter) {
			// if the index is valid, the resource has not been removed and the counters match...
			if (index < mStore.size() && mStore.at(index).mActive && mStore.at(index).mCounter == counter) {
				return *(mStore.at(index).mData.get()); // return a reference to the object stored
			}
			
			throw std::runtime_error("handle is invalid"); // an error has occurred, handle is invalid
		}
		
		// return a list of references to handled resources with matching name
		std::list< std::reference_wrapper<T> > Get(const std::string& name) {
			std::list< std::reference_wrapper<T> > result;
			
			for (auto iter = mStore.begin(); iter != mStore.end(); ++iter) { // for all resources in the store...
				if (iter->mActive && iter->mName == name) { // if the resource has not been removed and the name matches...
					result.push_back(std::ref(*(iter->mData.get()))); // add a reference to the resource to the list
				}
			}
			
			return result;
		}
		
		// return a list of references to handled resources
		std::list< std::reference_wrapper<T> > Get() {
			std::list< std::reference_wrapper<T> > result;
			
			for (auto iter = mStore.begin(); iter != mStore.end(); ++iter) { // for all resources in the store...
				if (iter->mActive) { // if the resource has not been removed...
					result.push_back(std::ref(*(iter->mData.get()))); // add a reference to the resource to the list
				}
			}
			
			return result;
		}
		
		// return a base pointer to a resource
		B* GetAsBase(const unsigned int& index, const unsigned int& counter) const {
			if (!std::is_base_of<B, T>::value) { // if template type T is not base of template type B...
				throw std::runtime_error("not of base type"); // an error has occurred, can't convert to base pointer
			}
			
			// if the index is valid, the resource has not been removed and the counters match...
			if (index < mStore.size() && mStore.at(index).mActive && mStore.at(index).mCounter == counter) {
				return mStore.at(index).mData.get(); // return a base pointer to the object stored
			}
			
			throw std::runtime_error("handle is invalid"); // an error has occurred, handle is invalid
		}
		
		// return a list of tuples containing the information required to build handles to resources with matching name
		std::list< std::tuple<unsigned int, unsigned int, std::string> > GetHandles(const std::string& name) const {
			std::list< std::tuple<unsigned int, unsigned int, std::string> > result;
			
			unsigned int i = 0u;
			for (auto iter = mStore.begin(); iter != mStore.end(); ++iter, ++i) { // for all resources in the store...
				if (iter->mActive && iter->mName == name) { // if the resource has not been removed and the name matches...
					result.emplace_back(std::make_tuple(i, iter->mCounter, iter->mName));
				}
			}
			
			return result;
		}
		
		// return a list of tuples containing the information required to build handles to ALL resources
		std::list< std::tuple<unsigned int, unsigned int, std::string> > GetHandles() const {
			std::list< std::tuple<unsigned int, unsigned int, std::string> > result;
			
			unsigned int i = 0u;
			for (auto iter = mStore.begin(); iter != mStore.end(); ++iter, ++i) { // for all resources in the store...
				if (iter->mActive) { // if the resource has not been removed...
					result.emplace_back(std::make_tuple(i, iter->mCounter, iter->mName));
				}
			}
			
			return result;
		}
	private :
		unsigned int mReserveCap = 100u; // how many slots to reserve in the store
		std::vector<StoreEntry> mStore; // the resources handled by this store
		
		// a sequential list (ascending) of indices that were previously occupied but are now freed and available for re-use
		std::priority_queue<unsigned int> mFreeIndices;
};

// templated manager class that handles resources registered to it that derive from a common base
template <typename B>
class Manager {
	public :
		// a handle that is used to refer to resources handled instead of a pointer
		class Handle {
			public :
				Handle(const unsigned int& typeID, const unsigned int& index, const unsigned int& counter, const std::string& name = "") : mTypeID(typeID), 
						mIndex(index), mCounter(counter), mName(name) {
					
					
				}
				
				bool operator ==(const Handle &other) const {
					return (mTypeID == other.mTypeID && mIndex == other.mIndex &&
							mCounter == other.mCounter && mName == other.mName);
				}
			public :
				unsigned int mTypeID = 0u; // a unique identifier that identifies a the managed type the resource belongs to
				unsigned int mIndex = 0u; // the index of the resource in the store
				unsigned int mCounter = 0u; // the counter value of the index used to validate the handle
				std::string mName = ""; // a name that the handled resource can be identified by (non-unique)
		};
		
		Manager() = default;
		Manager(const Manager& other) = delete;
		Manager(Manager&& other) : Manager() {
			swap(*this, other);
		}
		
		~Manager() {
			for (auto iter = mLookup.begin(); iter != mLookup.end(); ++iter) { // for all types of resource in the table...
				delete iter->second; // delete the resource store via the base pointer
			}
		}
		
		Manager& operator=(Manager other) {
			swap(*this, other);
			
			return *this;
		}
		
		friend void swap(Manager& first, Manager& second) {
			std::swap(first.mLookup, second.mLookup);
		}
		
		// register a derived resource type with the manager
		template <typename T>
		void Register() {
			if (!std::is_base_of<B, T>::value) { // if resource type to register does not inherit from base class...
				throw std::runtime_error("not of base type"); // an error has occurred, don't register resource type
			}
			
			unsigned int typeID = T::GetTypeID(); // get the type id relating to the resource type
			
			auto lookupResult = mLookup.find(typeID); // search for the resource type in the lookup table
			if (lookupResult != mLookup.end()) { // if the type id relating to the resource type is already registered...
				throw std::runtime_error("type already registered"); // an error has occurred, don't register resource type
			}
			
			mLookup.insert(std::make_pair(typeID, new Store<B, T>)); // add the unique id and new store to the lookup table
		}
		
		template <typename T>
		bool IsRegistered() {
			if (!std::is_base_of<B, T>::value) { // if resource type to register does not inherit from base class...
				return false;
			}
			
			unsigned int typeID = T::GetTypeID(); // get the type id relating to the resource type
			auto lookupResult = mLookup.find(typeID); // search for the resource type in the lookup table
			if (lookupResult != mLookup.end()) { // if the type id relating to the resource type is already registered...
				return true;
			}
			
			return false;
		}
		
		// add a new resource of the specified type with the specified name to the store and return a custom handle to it
		template <typename T, typename ...Ps>
		Handle Add(const std::string& name, const Ps&... params) {
			if (!std::is_base_of<B, T>::value) { // if resource type to register does not inherit from base class...
				throw std::runtime_error("not of base type"); // an error has occurred, resource type is invalid
			}
			
			unsigned int typeID = T::GetTypeID(); // get the type id relating to the resource type
			
			auto lookupResult = mLookup.find(typeID); // search for the resource type in the lookup table
			if (lookupResult == mLookup.end()) { // if the type id relating to the resource type isn't registered...
				throw std::runtime_error("type not registered"); // an error has occurred, resource type is invalid
			}
			
			Store<B, T>* storePtr = static_cast< Store<B, T>* >(lookupResult->second); // get the correct type of store from the lookup and cast it from its base pointer
			std::pair<unsigned int, unsigned int> indexCounterPair; // 
			
			try {
				indexCounterPair = storePtr->Add<Ps...>(name, params...); // add a new entry to the store and save the index and counter value returned
			} catch (std::exception& e) {
				throw;
			}
			
			return Handle(typeID, indexCounterPair.first, indexCounterPair.second, name); // return a handle to the newly added resource
		}
		
		// remove a resource of the specified type from the store
		template <typename T>
		void Remove(const Handle& handle) {
			unsigned int typeID = T::GetTypeID(); // get the type id relating to the resource type
			
			if (handle.mTypeID != typeID) { // if the handle and temporary object's unique type identifier don't match...
				throw std::runtime_error("handle/type mismatch"); // an error has occurred, handle is invalid
			}
			
			auto lookupResult = mLookup.find(typeID); // search for the resource type in the lookup table
			if (lookupResult == mLookup.end()) { // if the type id relating to the resource type isn't registered...
				throw std::runtime_error("type not registered"); // an error has occurred, handle is invalid
			}
			
			Store<B, T>* storePtr = static_cast< Store<B, T>* >(lookupResult->second); // get the correct type of store from the lookup and cast it from its base pointer
			
			try {
				storePtr->Remove(handle.mIndex, handle.mCounter); // remove the resource from the store using its index and counter (validity) value
			} catch (std::exception& e) {
				throw;
			}
		}
		
		// remove all resources of the specified type with the specified name from the store
		template <typename T>
		void Remove(const std::string& name) {
			unsigned int typeID = T::GetTypeID(); // get the type id relating to the resource type
			
			auto lookupResult = mLookup.find(typeID); // search for the resource type in the lookup table
			if (lookupResult == mLookup.end()) { // if the type id relating to the resource type isn't registered...
				throw std::runtime_error("type not registered"); // an error has occurred, handle is invalid
			}
			
			Store<B, T>* storePtr = static_cast< Store<B, T>* >(lookupResult->second); // get the correct type of store from the lookup and cast it from its base pointer
			
			try {
				storePtr->Remove(name); // remove all resources from the store with matching name
			} catch (std::exception& e) {
				throw;
			}
		}
		
		// remove ALL resources of the specified type from the store
		template <typename T>
		void Remove() {
			unsigned int typeID = T::GetTypeID(); // get the type id relating to the resource type
			
			auto lookupResult = mLookup.find(typeID); // search for the resource type in the lookup table
			if (lookupResult == mLookup.end()) { // if the type id relating to the resource type isn't registered...
				throw std::runtime_error("type not registered"); // an error has occurred, handle is invalid
			}
			
			Store<B, T>* storePtr = static_cast< Store<B, T>* >(lookupResult->second); // get the correct type of store from the lookup and cast it from its base pointer
			
			try {
				storePtr->Remove();
			} catch (std::exception& e) {
				throw;
			}
		}
		
		// remove a resource using only a handle (type unknown beforehand, derived from handle)
		void Remove(const Handle& handle) {
			auto lookupResult = mLookup.find(handle.mTypeID); // search for the resource type in the lookup table
			if (lookupResult == mLookup.end()) { // if the type id relating to the resource type isn't registered...
				throw std::runtime_error("type not registered"); // an error has occurred, handle is invalid
			}
			
			try {
				// remove the resource from the store using its index and counter (validity) value (using the base pointer)
				(lookupResult->second)->Remove(handle.mIndex, handle.mCounter);
			} catch (std::exception& e) {
				throw;
			}
		}
		
		// remove a resource using only a name FROM EVERY STORE
		void Remove(const std::string& name) {
			try {
				for (auto iter = mLookup.begin(); iter != mLookup.end(); ++iter) { // for all stored types...
					(iter->second)->Remove(name); // remove any entry matching name
				}
			} catch (std::exception& e) {
				throw;
			}
		}
		
		// remove ALL resources from ALL EVERY STORE
		void Remove() {
			try {
				for (auto iter = mLookup.begin(); iter != mLookup.end(); ++iter) { // for all stored types...
					(iter->second)->Remove();
				}
			} catch (std::exception& e) {
				throw;
			}
		}
		
		// return a reference to a resource pointed to by the supplied handle
		template <typename T>
		T& Get(const Handle& handle) {
			unsigned int typeID = T::GetTypeID(); // get the type id relating to the resource type
			
			if (handle.mTypeID != typeID) { // if the handle and temporary object's unique type identifier don't match...
				throw std::runtime_error("handle/type mismatch"); // an error has occurred, handle is invalid
			}
			
			auto lookupResult = mLookup.find(typeID); // search for the resource type in the lookup table
			if (lookupResult == mLookup.end()) { // if the type id relating to the resource type isn't registered...
				throw std::runtime_error("type not registered"); // an error has occurred, handle is invalid
			}
			
			Store<B, T>* storePtr = static_cast< Store<B, T>* >(lookupResult->second); // get the correct type of store from the lookup and cast it from its base pointer
			
			try {
				return storePtr->Get(handle.mIndex, handle.mCounter); // return a reference to the stored resource using its index and counter (validity) value
			} catch (std::exception& e) {
				throw;
			}
		}
		
		// return a list of references matching name
		template <typename T>
		std::list< std::reference_wrapper<T> > Get(const std::string& name) {
			unsigned int typeID = T::GetTypeID(); // get the type id relating to the resource type
			
			auto lookupResult = mLookup.find(typeID); // search for the resource type in the lookup table
			if (lookupResult == mLookup.end()) { // if the type id relating to the resource type isn't registered...
				throw std::runtime_error("type not registered"); // an error has occurred, handle is invalid
			}
			
			Store<B, T>* storePtr = static_cast< Store<B, T>* >(lookupResult->second); // get the correct type of store from the lookup and cast it from its base pointer
			
			try {
				return storePtr->Get(name); // return a list of references to the stored resources 
			} catch (std::exception& e) {
				throw;
			}
		}
		
		// return a list of references to ALL resources of type
		template <typename T>
		std::list< std::reference_wrapper<T> > Get() {
			unsigned int typeID = T::GetTypeID(); // get the type id relating to the resource type
			
			auto lookupResult = mLookup.find(typeID); // search for the resource type in the lookup table
			if (lookupResult == mLookup.end()) { // if the type id relating to the resource type isn't registered...
				throw std::runtime_error("type not registered"); // an error has occurred, handle is invalid
			}
			
			Store<B, T>* storePtr = static_cast< Store<B, T>* >(lookupResult->second); // get the correct type of store from the lookup and cast it from its base pointer
			
			try {
				return storePtr->Get(); // return a list of references to the stored resources 
			} catch (std::exception& e) {
				throw;
			}
		}
		
		// return a base pointer to a resource pointed to by the supplied handle
		B* GetAsBase(const Handle& handle) {
			auto lookupResult = mLookup.find(handle.mTypeID); // search for the resource type in the lookup table
			if (lookupResult == mLookup.end()) { // if the type id relating to the resource type isn't registered...
				throw std::runtime_error("type not registered"); // an error has occurred, handle is invalid
			}
			
			try {
				return (lookupResult->second)->GetAsBase(handle.mIndex, handle.mCounter);
			} catch (std::exception& e) {
				throw;
			}
		}
		
		// return handles to resources of type with matching name
		template <typename T>
		std::list<Handle> GetHandles(const std::string& name) const {
			unsigned int typeID = T::GetTypeID(); // get the type id relating to the resource type
			
			auto lookupResult = mLookup.find(typeID); // search for the resource type in the lookup table
			if (lookupResult == mLookup.end()) { // if the type id relating to the resource type isn't registered...
				throw std::runtime_error("type not registered"); // an error has occurred, handle is invalid
			}
			
			Store<B, T>* storePtr = static_cast< Store<B, T>* >(lookupResult->second); // get the correct type of store from the lookup and cast it from its base pointer
			
			try {
				std::list<Handle> handleList;
				std::list< std::tuple<unsigned int, unsigned int, std::string> >
						handleData = storePtr->GetHandles(name); // get handle data from the store
				
				// for all handle data in the list...
				for (auto handleDataIter = handleData.begin(); handleDataIter != handleData.end(); ++handleDataIter) {
					// construct a handle to the resource and add it to the list
					handleList.emplace_back(typeID, std::get<0>(*handleDataIter),
							std::get<1>(*handleDataIter), std::get<2>(*handleDataIter));
				}
				
				return handleList;
			} catch (std::exception& e) {
				throw;
			}
		}
		
		// return handles to ALL resources of type
		template <typename T>
		std::list<Handle> GetHandles() const {
			unsigned int typeID = T::GetTypeID(); // get the type id relating to the resource type
			
			auto lookupResult = mLookup.find(typeID); // search for the resource type in the lookup table
			if (lookupResult == mLookup.end()) { // if the type id relating to the resource type isn't registered...
				throw std::runtime_error("type not registered"); // an error has occurred, handle is invalid
			}
			
			Store<B, T>* storePtr = static_cast< Store<B, T>* >(lookupResult->second); // get the correct type of store from the lookup and cast it from its base pointer
			
			try {
				std::list<Handle> handleList;
				std::list< std::tuple<unsigned int, unsigned int, std::string> >
						handleData = storePtr->GetHandles(); // get handle data from the store
				
				// for all handle data in the list...
				for (auto handleDataIter = handleData.begin(); handleDataIter != handleData.end(); ++handleDataIter) {
					// construct a handle to the resource and add it to the list
					handleList.emplace_back(typeID, std::get<0>(*handleDataIter),
							std::get<1>(*handleDataIter), std::get<2>(*handleDataIter));
				}
				
				return handleList;
			} catch (std::exception& e) {
				throw;
			}
		}
		
		// return handles to resources with matching name FROM EVERY STORE
		std::list<Handle> GetHandles(const std::string& name) const {
			try {
				std::list<Handle> handleList;
				
				for (auto lookupIter = mLookup.begin(); lookupIter != mLookup.end(); ++lookupIter) { // for all stored types...
					std::list< std::tuple<unsigned int, unsigned int, std::string> >
							handleData = (lookupIter->second)->GetHandles(name); // get handle data from the store
					
					// for all handle data in the list...
					for (auto handleDataIter = handleData.begin(); handleDataIter != handleData.end(); ++handleDataIter) {
						// construct a handle to the resource and add it to the list
						handleList.emplace_back(lookupIter->first, std::get<0>(*handleDataIter),
								std::get<1>(*handleDataIter), std::get<2>(*handleDataIter));
					}
				}
				
				return handleList;
			} catch (std::exception& e) {
				throw;
			}
		}
		
		// return handles to ALL resources FROM EVERY STORE
		std::list<Handle> GetHandles() const {
			try {
				std::list<Handle> handleList;
				
				for (auto lookupIter = mLookup.begin(); lookupIter != mLookup.end(); ++lookupIter) { // for all stored types...
					std::list< std::tuple<unsigned int, unsigned int, std::string> >
							handleData = (lookupIter->second)->GetHandles(); // get handle data from the store
					
					// for all handle data in the list...
					for (auto handleDataIter = handleData.begin(); handleDataIter != handleData.end(); ++handleDataIter) {
						// construct a handle to the resource and add it to the list
						handleList.emplace_back(lookupIter->first, std::get<0>(*handleDataIter),
								std::get<1>(*handleDataIter), std::get<2>(*handleDataIter));
					}
				}
				
				return handleList;
			} catch (std::exception& e) {
				throw;
			}
		}
	
	private :
		std::map<unsigned int, StoreBase<B>*> mLookup; // a table mapping a resource's unique identifier to its storage
};
}

#endif
