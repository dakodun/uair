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

#ifndef UAIRRESOURCE_HPP
#define UAIRRESOURCE_HPP

#include <map>

#include "init.hpp"
#include "resourceptr.hpp"
#include "manager.hpp"

namespace uair {
class EXPORTDLL Resource {
	public :
		~Resource() {
			for (auto iter = mStore.begin(); iter != mStore.end(); ++iter) { // for all resourceptrs pointing to this resource...
				iter->second->Reset(); // reset the resourceptr back to default
			}
			
			mStore.clear(); // remove all resourceptrs
			mPtrCount = 1u; // set the reference count back to default
		}
		
		unsigned int AddReference(ResourcePtrBase* resPtr, const unsigned int &id = 0u) {
			if (id == 0u) { // if no id was supplied...
				mStore.insert(std::make_pair(mPtrCount, resPtr)); // add to the end of the container
				return mPtrCount++; // return reference count and increment it
			}
			else {
				mStore.at(id) = resPtr; // replace the reference at the position specified
				return id; // return the replaced id
			}
		}
		
		unsigned int RemoveReference(ResourcePtrBase* resPtr) {
			mStore.erase(resPtr->GetID()); // remove the reference from the store
			
			if (mStore.empty()) { // if there are no references left...
				mPtrCount = 1u; // set the reference count back to default
			}
			
			return 0;
		}
	protected :
		std::map<unsigned int, ResourcePtrBase*> mStore; // container holding references and associated id
		unsigned int mPtrCount = 1u; // reference counter - current count implies reference id (1-index)
};

typedef Manager<Resource>::Handle ResourceHandle;
enum class Resources : unsigned int {
	Texture = 1u,
	SDFFont,
	Font,
	RenderBuffer,
	Shader,
	SoundBuffer
};
}

#endif
