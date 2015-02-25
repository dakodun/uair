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

#ifndef UAIRRESOURCESTORE_HPP
#define UAIRRESOURCESTORE_HPP

#include <iostream>
#include <map>

namespace uair {
template<class T>
class ResourcePtr;

template<class T>
class ResourceStore {
	public :
		template<typename ...Ts>
		ResourcePtr<T> AddResource(const std::string& name, const Ts&... params) {
			auto result = mStore.emplace(std::piecewise_construct, std::forward_as_tuple(name),
					std::forward_as_tuple(params...));
			
			if (result.second == true) {
				return ResourcePtr<T>(&((result.first)->second));
			}
			
			throw std::runtime_error("error: resource with name " + name + " already exists");
		}
		
		ResourcePtr<T> AddResource(const std::string& name, T&& res) {
			auto result = mStore.emplace(name, std::move(res));
			
			if (result.second == true) {
				return ResourcePtr<T>(&((result.first)->second));
			}
			
			throw std::runtime_error("error: resource with name " + name + " already exists");
		}
		
		void RemoveResource(const std::string& name) {
			auto iter = mStore.find(name);
			
			if (iter != mStore.end()) {
				mStore.erase(iter);
				return;
			}
			
			throw std::runtime_error("error: resource with name " + name + " doesn't exist");
		}
		
		ResourcePtr<T> GetResource(const std::string& name) {
			if (!mStore.empty()) {
				auto iter = mStore.find(name);
				
				if (iter != mStore.end()) {
					return ResourcePtr<T>(&(iter->second));
				}
			}
			
			throw std::runtime_error("error: resource with name " + name + " doesn't exist");
		}
	private :
		std::map<std::string, T> mStore;
};
}

#endif
