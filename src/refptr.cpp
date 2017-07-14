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

#include "refptr.hpp"

namespace uair {
RefType::RefType(RefType&& other) : RefType() {
	swap(*this, other);
}

RefType::~RefType() {
	for (auto& ptr: mStore) { // for all ptrs that reference this type...
		ptr.second->Reset(); // reset them back to default
	}
	
	// remove all ptrs from the store and reset the id counter
	mStore.clear();
	mCounter = 0u;
}

RefType& RefType::operator=(RefType other) {
	swap(*this, other);
	
	return *this;
}

unsigned int RefType::AddReference(RefPtrBase* refPtr,
		const unsigned int& id) {
	
	if (id == 0u) { // if no id was supplied...
		// add a base pointer to the refptr to the store under a new id
		mStore.insert(std::make_pair(mCounter + 1u, refPtr));
		
		return ++mCounter; // increment and return the id counter
	}
	else { // otherwise an id was supplied...
		auto result = mStore.find(id);
		if (result != mStore.end()) {
			result->second = refPtr; // replace the pointer with matching id
			return id; // return the new id
		}
		else {
			return 0u; // return and id of 0 (invalid id)
		}
	}
}

unsigned int RefType::RemoveReference(RefPtrBase* refPtr) {
	mStore.erase(refPtr->mID); // remove the reference from the store
	
	if (mStore.empty()) { // if there are no references left...
		// set the reference counter back to 0 (can now safely re-use ids)
		mCounter = 0u;
	}
	
	// return 0 for consistency (sets refptr id to 0 i.e., not valid)
	return 0u;
}

void RefType::SetAll() {
	for (auto& ptr: mStore) { // for all ptrs in store...
		ptr.second->SetDirect(this); // directly set them to point to this
	}
}
}
