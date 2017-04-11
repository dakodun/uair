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

#ifndef UAIRLUAAPI_HPP
#define UAIRLUAAPI_HPP

#ifndef UAIRNOLUA
#include "luajit/lua/lua.hpp"

namespace uair {
class LuaAPI {
	public :
		struct Nil {
			
		};
	public :
		LuaAPI() {
			mState = luaL_newstate();
			luaL_openlibs(mState);
			
			CreateSandbox();
			if (luaL_loadstring(mState, mSandbox.c_str()) || lua_pcall(mState, 0, 0, 0)) {
				throw std::runtime_error("big probs");
			}
		}
		
		~LuaAPI() {
			if (mState) {
				lua_close(mState);
			}
		}
		
		// call a script as a string in the sandbox
			// call a script with no return values
			template <typename ...Ps>
			void CallString(const std::string& script, const Ps&... params) {
				int stackSize = lua_gettop(mState);
				
				lua_getglobal(mState, "uair_run");
				PushStack(script, params...);
				lua_pcall(mState, sizeof...(params) + 1, LUA_MULTRET, 0);
				
				if (ReadStack<bool>(stackSize + 1)) {
					lua_settop(mState, stackSize);
				}
				else {
					std::string errorMsg = ReadStack<std::string>(stackSize + 2);
					
					lua_settop(mState, stackSize);
					throw std::runtime_error(errorMsg);
				}
			}
			
			// call a script with 1 or more return values
			template <typename R, typename ...Rs, typename ...Ps>
			std::tuple<R, Rs...> CallString(const std::string& script, const Ps&... params) {
				int stackSize = lua_gettop(mState);
				
				lua_getglobal(mState, "uair_run");
				PushStack(script, params...);
				lua_pcall(mState, sizeof...(params) + 1, LUA_MULTRET, 0);
				
				if (ReadStack<bool>(stackSize + 1)) {
					std::tuple<R, Rs...> result;
					
					result = ReadStack<R, Rs...>(stackSize + 2);
					lua_settop(mState, stackSize);
					return result;
				}
				else {
					std::string errorMsg = ReadStack<std::string>(stackSize + 2);
					
					lua_settop(mState, stackSize);
					throw std::runtime_error(errorMsg);
				}
			}
		//
		
		// using overloaded functions, push a single value to the stack
			void PushStack() {
				
			}
			
			void PushStack(const Nil& value) {
				lua_pushnil(mState);
			}
			
			void PushStack(const bool& value) {
				lua_pushboolean(mState, value);
			}
			
			void PushStack(const int& value) {
				lua_pushinteger(mState, value);
			}
			
			void PushStack(const float& value) {
				lua_pushnumber(mState, value);
			}
			
			void PushStack(const std::string& value) {
				lua_pushstring(mState, value.c_str());
			}
			
			void PushStack(const char* value) {
				lua_pushstring(mState, value);
			}
		//
		
		// using recursion, push multiple values to the stack
			// define a templated singular push to prevent infinite recursion if a non-recognised type is passed
			template <typename T>
			void PushStack(const T& value) {
				std::cout << "attempt to push an unregistered type: pushing nil instead" << std::endl;
				lua_pushnil(mState);
			}
			
			template <typename T, typename... Ps>
			void PushStack(const T& value, const Ps&... values) {
				PushStack(value); // call the matching specialisation of Push for T
				PushStack(values...); // recursive call placing the first of the remaining arguments Ps as T, and the rest as Ps (if any)
			}
		//
		
		// using template specialisation, read a single value from the stack
			template <typename T>
			T ReadStack(const int& index);
		//
		
		// using compile time index sequences and polymorphic lambdas (c++14) read multiple values from the stack
			// iterate over a tuple using an index sequence and copy each value from the stack into the corresponding
			// tuple element
			template<typename... Ps, std::size_t... Is>
			void TupleFromStack(std::tuple<Ps...>& tuple, int index, std::index_sequence<Is...>){
				using expander = int[]; // an alias allowing us to use brace list expansion
				
				auto fillTuple = [this, &index](auto &tupleElement) {
					// set the current tuple element to the result returned from the stack by calling the appropiate read function
					// (we need to strip the reference off the type of tupleElement e.g. int& -> int)
					tupleElement = ReadStack< std::remove_reference_t<decltype(tupleElement)> >(index++);
				};
				
				// expand the parameter list inside a brace list calling
				(void)expander{0, ((void)fillTuple(std::get<Is>(tuple)), 0)...};
			}
			
			// require a minimum of 2 paramaters (T, U) to avoid ambiguity
			template <typename T, typename U, typename... Ps>
			std::tuple<T, U, Ps...> ReadStack(const int& index) {
				std::tuple<T, U, Ps...> result; // create an empty tuple matching the requested parameters
				
				// set the tuple using an index sequence (size of tuple) and iterating through each of the types
				TupleFromStack(result, index, std::make_index_sequence<std::tuple_size<decltype(result)>::value>());
				
				return result;
			}
		//
	private :
		void CreateSandbox() {
			// [todo] create env table from outside
			
			mSandbox = R"(
				local env = {print = print, unpack = unpack}
				
				function uair_run(untrusted_code, ...)
					if untrusted_code:byte(1) == 27 then
						return false, "binary bytecode prohibited"
					end
					
					local untrusted_function, message = loadstring(untrusted_code)
					if not untrusted_function then
						return false, message
					end
					
					setfenv(untrusted_function, env)
					return pcall(untrusted_function, ...)
				end
			)";
		}
	
	private :
		lua_State* mState = nullptr;
		std::string mSandbox;
};

template <>
inline bool LuaAPI::ReadStack(const int& index) {
	if (lua_isboolean(mState, index)) {
		return lua_toboolean(mState, index);
	}
	
	std::cout << "not a valid boolean" << std::endl;
	return false;
}

template <>
inline int LuaAPI::ReadStack(const int& index) {
	if (lua_isnumber(mState, index)) {
		return lua_tointeger(mState, index);
	}
	
	std::cout << "not a valid integer" << std::endl;
	return 0;
}

template <>
inline float LuaAPI::ReadStack(const int& index) {
	if (lua_isnumber(mState, index)) {
		return lua_tonumber(mState, index);
	}
	
	std::cout << "not a valid float" << std::endl;
	return 0.0f;
}

template <>
inline LuaAPI::Nil LuaAPI::ReadStack(const int& index) {
	if (!lua_isnil(mState, index)) {
		std::cout << "not a valid nil" << std::endl;
	}
	
	return LuaAPI::Nil();
}

template <>
inline std::string LuaAPI::ReadStack(const int& index) {
	if (lua_isstring(mState, index)) {
		return lua_tostring(mState, index);
	}
	
	std::cout << "not a valid string" << std::endl;
	return "";
}
}
#endif

#endif
