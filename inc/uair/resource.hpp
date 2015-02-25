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

#ifndef UAIRRESOURCE_HPP
#define UAIRRESOURCE_HPP

#include <map>

#include "resourceptr.hpp"

namespace uair {
template<class T>
class Resource {
	public :
		~Resource() {
			mStore.clear(); // remove all referenced resource pointers
			mPtrCount = 1u; // set the reference count back to default
		}
		
		unsigned int AddReference(ResourcePtr<T>* resPtr, const unsigned int &id = 0u) {
			if (id == 0u) { // if no id was supplied...
				mStore.insert(std::make_pair(mPtrCount, resPtr)); // add to the end of the container
				return mPtrCount++; // return reference count and increment it
			}
			else {
				mStore.at(id) = resPtr; // replace the reference at the position specified
				return id; // return the replaced id
			}
		}
		
		unsigned int RemoveReference(ResourcePtr<T>* resPtr) {
			mStore.erase(resPtr->GetID()); // remove the reference from the store
			
			if (mStore.empty()) { // if there are no references left...
				mPtrCount = 1u; // set the reference count back to default
			}
			
			return 0;
		}
	protected :
		std::map<unsigned int, ResourcePtr<T>*> mStore; // container holding references and associated id
		unsigned int mPtrCount = 1u; // reference counter - current count implies reference id (1-index)
};
}

#endif
