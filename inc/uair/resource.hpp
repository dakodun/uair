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
			for (auto iter = mStore.begin(); iter != mStore.end(); ++iter) {
				iter->second->UnsetResource();
			}
		}
		
		unsigned int AddReference(ResourcePtr<T>* resPtr, const unsigned int &id = 0u) {
			if (id == 0u) {
				mStore.insert(std::make_pair(mPtrCount, resPtr));
				return mPtrCount++;
			}
			else {
				mStore.at(id) = resPtr;
				return id;
			}
		}
		
		unsigned int RemoveReference(ResourcePtr<T>* resPtr) {
			mStore.erase(resPtr->GetID());
			
			if (mStore.empty()) {
				mPtrCount = 1u;
			}
			
			return 0;
		}
		
		/* {
			mPtrCount = 1u;
			iterate map
				move value from key to mPtrCount++;
				notify value about change
		} */
	protected :
		std::map<unsigned int, ResourcePtr<T>*> mStore;
		unsigned int mPtrCount = 1u;
};
}

#endif
