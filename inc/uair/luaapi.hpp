/* **************************************************************** **
**	Uair Engine
**	Copyright (c) 2010 - 2017, Iain M. Crawford
**
**	This software is provided 'as-is', without any express orOrient
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
#include <iostream>
#include <tuple>
#include <utility>
#include <map>
#include <functional>
#include <typeindex>

#include "util.hpp"
#include "luajit/lua/lua.hpp"

namespace uair {
class EXPORTDLL LuaAPI {
	public :
		// an empty struct representing a nil value in lua
		struct Nil {
			
		};
		
		// classes to handle registered type callbacks
			class RegisteredFuncBase {
				public :
					// call the function handling parameters and return values (using the stack (l))
					virtual int Call(lua_State* l, const unsigned int& counter, const std::string& name) = 0;
			};
			
			// stores a constructor (T(Ps...))
			template <class T, class ...Ps>
			class RegisteredConstFunc : public RegisteredFuncBase {
				public :
					int Call(lua_State* l, const unsigned int& counter, const std::string& name);
				private :
					// specialisation for no parameters
					template <typename Ta>
					int CallAlias(lua_State* l, const unsigned int& counter, const std::string& name);
					
					// specialisation for one parameter
					template <typename Ta, typename Pa>
					int CallAlias(lua_State* l, const unsigned int& counter, const std::string& name);
					
					// specialisation for two or more parameters
					template <typename Ta, typename P1a, typename P2a, typename ...Psa>
					int CallAlias(lua_State* l, const unsigned int& counter, const std::string& name);
			};
			
			// stores a destructor (~T())
			template <class T>
			class RegisteredDestFunc : public RegisteredFuncBase {
				public :
					int Call(lua_State* l, const unsigned int& counter, const std::string& name);
			};
			
			// stores a function (R T::Func(Ps...))
			template <class T, class R, class... Ps>
			class RegisteredFunc : public RegisteredFuncBase {
				public :
					RegisteredFunc(R(T::*func)(Ps...));
					
					int Call(lua_State* l, const unsigned int& counter, const std::string& name);
				private :
					template <typename Ta, typename Ra>
					int CallAlias(lua_State* l, const unsigned int& counter, const std::string& name);
					
					template <typename Ta, typename Ra, typename Pa>
					int CallAlias(lua_State* l, const unsigned int& counter, const std::string& name);
					
					template <typename Ta, typename Ra, typename P1a, typename P2a, typename ...Psa>
					int CallAlias(lua_State* l, const unsigned int& counter, const std::string& name);
				
				private :
					std::function<R(T&, Ps...)> mFunctionPtr;
			};
			
			// specialisation to allow functions return void
			template <class T, class... Ps>
			class RegisteredFunc<T, void, Ps...> : public RegisteredFuncBase {
				public :
					RegisteredFunc(void(T::*func)(Ps...));
					
					int Call(lua_State* l, const unsigned int& counter, const std::string& name);
				private :
					template <typename Ta>
					int CallAlias(lua_State* l, const unsigned int& counter, const std::string& name);
					
					template <typename Ta, typename Pa>
					int CallAlias(lua_State* l, const unsigned int& counter, const std::string& name);
					
					template <typename Ta, typename P1a, typename P2a, typename ...Psa>
					int CallAlias(lua_State* l, const unsigned int& counter, const std::string& name);
				
				private :
					std::function<void(T&, Ps...)> mFunctionPtr;
			};
		//
		
		// holds the constructor, destructor and functions for a type registered with lua
		class EXPORTDLL RegisteredType {
			public :
				RegisteredType(const unsigned int& counter, const std::string& name);
				
				std::string GetName() const;
				
				int CallConstructor(lua_State* l);
				int CallDestructor(lua_State* l);
				int CallFunction(lua_State* l, const std::string& funcName);
				
				template <typename T, typename ...Ps>
				void AddConstructor(lua_State* l);
				
				template <typename T>
				void AddDestructor(lua_State* l);
				
				template <typename T, typename R, typename... Ps>
				void AddFunction(lua_State* l, const std::string& funcName, R(T::*func)(Ps...));
			
			private :
				std::unique_ptr<RegisteredFuncBase> mConstructor;
				std::unique_ptr<RegisteredFuncBase> mDestructor;
				std::map< std::string, std::unique_ptr<RegisteredFuncBase> > mFunctions;
				
				unsigned int mCounter;
				std::string mName;
		};
	public :
		LuaAPI();
		LuaAPI(const LuaAPI& other)  = delete;
		LuaAPI(LuaAPI&& other) = delete;
		
		~LuaAPI();
		
		LuaAPI& operator=(LuaAPI other) = delete;
		
		// call a script as a string in the sandbox
			// call a script with no return values
			template <typename ...Ps>
			void CallString(const std::string& script, const Ps&... params);
			
			// call a script with 1 return value
			template <typename R, typename ...Ps>
			R CallString(const std::string& script, const Ps&... params);
			
			// call a script with 2 or more return values
			template <typename R1, typename R2, typename ...Rs, typename ...Ps>
			std::tuple<R1, R2, Rs...> CallString(const std::string& script, const Ps&... params);
		//
		
		// using overloaded functions, push a single value to the stack
			void PushStack(const Nil& value);
			void PushStack(const bool& value);
			void PushStack(const int& value);
			void PushStack(const float& value);
			void PushStack(const std::string& value);
			void PushStack(const char* value);
			
			static void PushStack(const unsigned int& counter, const Nil& value);
			static void PushStack(const unsigned int& counter, const bool& value);
			static void PushStack(const unsigned int& counter, const int& value);
			static void PushStack(const unsigned int& counter, const float& value);
			static void PushStack(const unsigned int& counter, const std::string& value);
			static void PushStack(const unsigned int& counter, const char* value);
		//
		
		// using recursion, push multiple values to the stack
			template <typename P>
			void PushStack(const P& value);
			
			template <typename P, typename... Ps>
			void PushStack(const P& value, const Ps&... values);
			
			template <typename P>
			static void PushStack(const unsigned int& counter, const P& value);
			
			template <typename P, typename... Ps>
			static void PushStack(const unsigned int& counter, const P& value, const Ps&... values);
		//
		
		// using template specialisation, read a single value from the stack
			template <typename R>
			R ReadStack(const int& index);
			
			template <typename R>
			static R ReadStack(const unsigned int& counter, const int& index);
		//
		
		// using compile time index sequences and polymorphic lambdas (c++14) read multiple values from the stack
			// require a minimum of 2 paramaters (P1, P2) to avoid ambiguity
			template <typename P1, typename P2, typename... Ps>
			std::tuple<P1, P2, Ps...> ReadStack(int index);
			
			template <typename P1, typename P2, typename... Ps>
			static std::tuple<P1, P2, Ps...> ReadStack(const unsigned int& counter, int index);
		//
		
		// register a type to be used from lua
			// register a type (T) with lua
			template <typename T>
			bool RegisterType(const std::string& name);
			
			// register a constructor to type (T) that takes parameters (Ps...)
			// also registers default push and read functions allowing stack interaction
			template <typename T, typename... Ps>
			bool RegisterConstructor();
			
			// register a function (R T::Func(Ps...)) to type (T)
			template <typename T, typename R, typename... Ps>
			bool RegisterFunction(const std::string& funcName, R(T::*func)(Ps...));
		//
		
		// static callbacks that maps from lua to a luaapi(via upvalues)
			static int OnNew(lua_State* l);
			static int OnDelete(lua_State* l);
			static int OnFunction(lua_State* l);
		//
		
		bool AddWhitelist(const std::string& var, std::string alias = "");
		bool CreateSandbox();
	
	public :
		// counter to assign a unique id to a LuaAPI for map access
		static unsigned int mAPICounter;
		
		// static map that holds pointers to LuaAPI instances to allow access via static lua function
		static std::map<unsigned int, LuaAPI*> mAPIInstances;
	private :
		lua_State* mState = nullptr;
		std::string mSandbox;
		unsigned int mCounter; // a unique id for use with map access
		
		// maps that allow retrieval of register types via unqie names or typeids
		std::map<std::string, std::type_index> mNameIndexMap;
		std::map<std::type_index, RegisteredType> mIndexTypeMap;
		
		std::map< std::string, std::map<std::string, std::string> > mWhitelist;
};
}

#include "luaapi.inl"
#endif
#endif
