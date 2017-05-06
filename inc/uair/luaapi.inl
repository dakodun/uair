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
//
	template <class T, class ...Ps>
	int LuaAPI::RegisteredConstFunc<T, Ps...>::Call(lua_State* l, const unsigned int& counter, const std::string& name) {
		return CallAlias<T, Ps...>(l, counter, name); // call the specialistion depending on parameter count (Ps...)
	}
	
	template <class T, class ...Ps>
	template <typename Ta>
	int LuaAPI::RegisteredConstFunc<T, Ps...>::CallAlias(lua_State* l, const unsigned int& counter, const std::string& name) {
		void* userdata = lua_newuserdata(l, sizeof(Ta*)); // create a new block of memory to hold registered type
		
		if (!luaL_newmetatable(l, (name + ".mt").c_str())) { // attempt to retrieve the metatable associated with registered type...
			Ta** data = static_cast<Ta**>(userdata);
			*data = new Ta(); // create a new instance of registered type
			
			lua_setmetatable(l, -2); // assign the metatable to the new userdata
			return 1;
		}
		else { // if the metatable didn't exist (i.e., was just created)...
			// clean up the newly created metatable
			lua_pushnil(l);
			lua_setfield(l, LUA_REGISTRYINDEX, (name + ".mt").c_str());
			std::cout << "(LuaAPI) Callback Error (Constructor): metatable with name " << name << ".mt doesn't exist in current Lua State." << std::endl;
		}
		
		return 0;
	}
	
	template <class T, class ...Ps>
	template <typename Ta, typename Pa>
	int LuaAPI::RegisteredConstFunc<T, Ps...>::CallAlias(lua_State* l, const unsigned int& counter, const std::string& name) {
		int stackSize = lua_gettop(l);
		auto arg = ReadStack<Pa>(counter, stackSize); // read the single parameter (Pa) from the stack
		
		void* userdata = lua_newuserdata(l, sizeof(Ta*));
		
		if (!luaL_newmetatable(l, (name + ".mt").c_str())) {
			Ta** data = static_cast<Ta**>(userdata);
			*data = new Ta(arg);
			
			lua_setmetatable(l, -2);
			return 1;
		}
		else {
			lua_pushnil(l);
			lua_setfield(l, LUA_REGISTRYINDEX, (name + ".mt").c_str());
			std::cout << "(LuaAPI) Callback Error (Constructor): metatable with name " << name << ".mt doesn't exist in current Lua State." << std::endl;
		}
		
		return 0;
	}
	
	template <class T, class ...Ps>
	template <typename Ta, typename P1a, typename P2a, typename ...Psa>
	int LuaAPI::RegisteredConstFunc<T, Ps...>::CallAlias(lua_State* l, const unsigned int& counter, const std::string& name) {
		int stackSize = lua_gettop(l);
		auto args = ReadStack<P1a, P2a, Psa...>(counter, stackSize - (1 + sizeof...(Psa))); // read the parameters (P1a, P2a, Psa...) from the stack into a tuple
		
		// lambda function that creates a new type (T) using an expanded tuple (std::get<0>(tuple), std::get<1>(tuple), etc)
		auto construct = [l, &name](auto&... tupleElements)->int {
			void* userdata = lua_newuserdata(l, sizeof(Ta*));
			
			if (!luaL_newmetatable(l, (name + ".mt").c_str())) {
				Ta** data = static_cast<Ta**>(userdata);
				*data = new Ta(tupleElements...);
				
				lua_setmetatable(l, -2);
				return 1;
			}
			else {
				lua_pushnil(l);
				lua_setfield(l, LUA_REGISTRYINDEX, (name + ".mt").c_str());
				std::cout << "(LuaAPI) Callback Error (Constructor): metatable with name " << name << ".mt doesn't exist in current Lua State." << std::endl;
			}
			
			return 0;
		};
		
		// expand the tuple into a parameter list and call the lambda function
		return util::ExpandTuple(construct, args, std::make_index_sequence<std::tuple_size<decltype(args)>::value>());
	}
	
	
	template <class T>
	int LuaAPI::RegisteredDestFunc<T>::Call(lua_State* l, const unsigned int& counter, const std::string& name) {
		void* userdata = luaL_checkudata(l, 1, (name + ".mt").c_str()); // check that the userdata is the correct type
		if (userdata != nullptr) { // if the userdata is valid...
			T** data = static_cast<T**>(userdata);
			delete *data;
		}
		
		return 0;
	}
	
	
	template <class T, class R, class... Ps>
	LuaAPI::RegisteredFunc<T, R, Ps...>::RegisteredFunc(R(T::*func)(Ps...)) : mFunctionPtr(func) {
		
	}
	
	template <class T, class R, class... Ps>
	int LuaAPI::RegisteredFunc<T, R, Ps...>::Call(lua_State* l, const unsigned int& counter, const std::string& name) {
		return CallAlias<T, R, typename std::decay<Ps>::type...>(l, counter, name);
	}
	
	template <class T, class R, class... Ps>
	template <typename Ta, typename Ra>
	int LuaAPI::RegisteredFunc<T, R, Ps...>::CallAlias(lua_State* l, const unsigned int& counter, const std::string& name) {
		void* userdata = luaL_checkudata(l, 1, (name + ".mt").c_str());
		if (userdata != nullptr) {
			Ta** data = static_cast<Ta**>(userdata);
			
			auto res = mFunctionPtr(**data);
			PushStack(counter, res); // add the return value to the stack
			
			return 1;
		}
		
		return 0;
	}
	
	template <class T, class R, class... Ps>
	template <typename Ta, typename Ra, typename Pa>
	int LuaAPI::RegisteredFunc<T, R, Ps...>::CallAlias(lua_State* l, const unsigned int& counter, const std::string& name) {
		int stackSize = lua_gettop(l);
		void* userdata = luaL_checkudata(l, 1, (name + ".mt").c_str());
		
		if (userdata != nullptr) {
			Ta** data = static_cast<Ta**>(userdata);
			
			auto arg = ReadStack<Pa>(counter, stackSize);
			auto res = mFunctionPtr(**data, arg);
			PushStack(counter, res);
			
			return 1;
		}
		
		return 0;
	}
	
	template <class T, class R, class... Ps>
	template <typename Ta, typename Ra, typename P1a, typename P2a, typename ...Psa>
	int LuaAPI::RegisteredFunc<T, R, Ps...>::CallAlias(lua_State* l, const unsigned int& counter, const std::string& name) {
		int stackSize = lua_gettop(l);
		auto args = ReadStack<P1a, P2a, Psa...>(counter, stackSize - (1 + sizeof...(Psa)));
		
		auto call = [this, l, &counter, &name](auto&... tupleElements)->int {
			void* userdata = luaL_checkudata(l, 1, (name + ".mt").c_str());
			if (userdata != nullptr) {
				Ta** data = static_cast<Ta**>(userdata);
				
				auto res = mFunctionPtr(**data, tupleElements...);
				PushStack(counter, res);
				
				return 1;
			}
			
			return 0;
		};
		
		return util::ExpandTuple(call, args, std::make_index_sequence<std::tuple_size<decltype(args)>::value>());
	}
	
	
	template <class T, class... Ps>
	LuaAPI::RegisteredFunc<T, void, Ps...>::RegisteredFunc(void(T::*func)(Ps...)) : mFunctionPtr(func) {
		
	}
	
	template <class T, class... Ps>
	int LuaAPI::RegisteredFunc<T, void, Ps...>::Call(lua_State* l, const unsigned int& counter, const std::string& name) {
		return CallAlias<T, typename std::decay<Ps>::type...>(l, counter, name);
	}
	
	template <class T, class... Ps>
	template <typename Ta>
	int LuaAPI::RegisteredFunc<T, void, Ps...>::CallAlias(lua_State* l, const unsigned int& counter, const std::string& name) {
		void* userdata = luaL_checkudata(l, 1, (name + ".mt").c_str());
		if (userdata != nullptr) {
			Ta** data = static_cast<Ta**>(userdata);
			
			mFunctionPtr(**data);
			
			return 0;
		}
		
		return 0;
	}
	
	template <class T, class... Ps>
	template <typename Ta, typename Pa>
	int LuaAPI::RegisteredFunc<T, void, Ps...>::CallAlias(lua_State* l, const unsigned int& counter, const std::string& name) {
		int stackSize = lua_gettop(l);
		void* userdata = luaL_checkudata(l, 1, (name + ".mt").c_str());
		
		if (userdata != nullptr) {
			Ta** data = static_cast<Ta**>(userdata);
			
			auto arg = ReadStack<Pa>(counter, stackSize);
			mFunctionPtr(**data, arg);
			
			return 0;
		}
		
		return 0;
	}
	
	template <class T, class... Ps>
	template <typename Ta, typename P1a, typename P2a, typename ...Psa>
	int LuaAPI::RegisteredFunc<T, void, Ps...>::CallAlias(lua_State* l, const unsigned int& counter, const std::string& name) {
		int stackSize = lua_gettop(l);
		auto args = ReadStack<P1a, P2a, Psa...>(counter, stackSize - (1 + sizeof...(Psa)));
		
		auto call = [this, l, &counter, &name](auto&... tupleElements)->int {
			void* userdata = luaL_checkudata(l, 1, (name + ".mt").c_str());
			if (userdata != nullptr) {
				Ta** data = static_cast<Ta**>(userdata);
				
				mFunctionPtr(**data, tupleElements...);
			}
			
			return 0;
		};
		
		return util::ExpandTuple(call, args, std::make_index_sequence<std::tuple_size<decltype(args)>::value>());
	}
//


template <typename T, typename ...Ps>
void LuaAPI::RegisteredType::AddConstructor(lua_State* l) {
	int stackSize = lua_gettop(l);
	
	if (!luaL_newmetatable(l, (mName + ".mt").c_str())) { // attempt to retrieve the metatable associated with registered type...
		mConstructor = std::make_unique< RegisteredConstFunc<T, Ps...> >();
		int locType = LUA_GLOBALSINDEX; // set the location of the new type to the global table
		
		// split the name into parts (0, 1 ... n-1 => namespace, namespace ... typename)
		std::vector<std::string> parts = util::SplitString(mName, '.');
		if (parts.size() > 1u) { // if there was at least one namespace...
			locType = -2; // update the final location of the new type (nested table)
			size_t size = parts.size() - 1u; // discount the final entry (typename)
			
			// function to retrieve a nested table (or create it if it doesn't exist)
			auto lambdaGetTable = [l](const int& loc, const std::string& name) {
				// the location of the parent table on the stack when assigning (which is one less compared to retrieving)
				int locSet = loc;
				if (loc != LUA_GLOBALSINDEX) { // if the location is not the global table...
					--locSet; // update the location of the parent table for setting
				}
				
				lua_getfield(l, loc, name.c_str()); // attempt to get the nested table
				
				if (lua_isnil(l, -1)) { // if the nested table doesn't exist...
					lua_remove(l, -1); // remove the nil value
					lua_newtable(l); // create a new table and push it onto the stack
					lua_setfield(l, locSet, name.c_str()); // assign the new table as a nested table to the previous
					lua_getfield(l, loc, name.c_str()); // place the new table back onto the top of the stack
				}
			};
			
			// retrieve the top level table (from the global table)
			lambdaGetTable(LUA_GLOBALSINDEX, parts.front());
			for (unsigned int i = 1u; i < size; ++i) { // for all remaining nested tables...
				lambdaGetTable(-1, parts.at(i)); // retrieve the nested table (from the previous table)
			}
		}
		
		// add a closure to the lowest nested table with upvalues indicating the owning luaapi instance and type name
		lua_pushinteger(l, mCounter);
		lua_pushstring(l, mName.c_str());
		lua_pushcclosure(l, LuaAPI::OnNew, 2);
		lua_setfield(l, locType, (parts.back()).c_str());
	}
	else { // if the metatable didn't exist (i.e., was just created)...
		// clean up the newly created metatable
		lua_pushnil(l);
		lua_setfield(l, LUA_REGISTRYINDEX, (mName + ".mt").c_str());
		std::cout << "(LuaAPI) Constructor Registration Error: metatable with name " << mName << ".mt doesn't exist in current Lua State." << std::endl;
	}
	
	lua_settop(l, stackSize);
}

template <class T>
void LuaAPI::RegisteredType::AddDestructor(lua_State* l) {
	int stackSize = lua_gettop(l);
	
	if (!luaL_newmetatable(l, (mName + ".mt").c_str())) {
		mDestructor = std::make_unique< RegisteredDestFunc<T> >();
		
		// create a closure (as garbage collection method) with upvalues indicating the owning luaapi instance and type name
		lua_pushinteger(l, mCounter);
		lua_pushstring(l, mName.c_str());
		static const struct luaL_Reg funcReg[] = {
			{"__gc", LuaAPI::OnDelete},
			{nullptr, nullptr}
		};
		
		luaL_openlib(l, NULL, funcReg, 2); // register the function to the metatable
	}
	else {
		lua_pushnil(l);
		lua_setfield(l, LUA_REGISTRYINDEX, (mName + ".mt").c_str());
		std::cout << "(LuaAPI) Type Registration Error: metatable with name " << mName << ".mt doesn't exist in current Lua State." << std::endl;
	}
	
	lua_settop(l, stackSize);
}

template <typename T, typename R, typename... Ps>
void LuaAPI::RegisteredType::AddFunction(lua_State* l, const std::string& funcName, R(T::*func)(Ps...)) {
	int stackSize = lua_gettop(l);
	
	if (!luaL_newmetatable(l, (mName + ".mt").c_str())) {
		std::unique_ptr<RegisteredFuncBase> funcPtr = std::make_unique< RegisteredFunc<T, R, Ps...> >(func);
		auto res = mFunctions.insert(std::make_pair(funcName, std::move(funcPtr))); // attempt to insert the new function
		
		if (res.second) { // if the new function was successfully inserted...
			// create a closure with upvalues indicating the owning luaapi instance, type name and function name
			lua_pushinteger(l, mCounter);
			lua_pushstring(l, mName.c_str());
			lua_pushstring(l, funcName.c_str());
			static const struct luaL_Reg funcReg[] = {
				{funcName.c_str(), LuaAPI::OnFunction},
				{nullptr, nullptr}
			};
			
			luaL_openlib(l, NULL, funcReg, 3);
		}
		else { // otherwise the function already exists...
			std::cout << "(LuaAPI) Function Registration Error: function with name " << funcName << " is already registered." << std::endl;
		}
	}
	else {
		lua_pushnil(l);
		lua_setfield(l, LUA_REGISTRYINDEX, (mName + ".mt").c_str());
		std::cout << "(LuaAPI) Function Registration Error: metatable with name " << mName << ".mt doesn't exist in current Lua State." << std::endl;
	}
	
	lua_settop(l, stackSize);
}


//
	template <typename ...Ps>
	void LuaAPI::CallString(const std::string& script, const Ps&... params) {
		int stackSize = lua_gettop(mState);
		
		lua_getglobal(mState, "uair_run"); // get the sandbox function
		PushStack(script, params...); // push the script and any parameters to the stack
		
		// if the number of values on the stack doesn't match the number of parameters...
		if ((lua_gettop(mState) - stackSize) - 1 != sizeof...(params) + 1) {
			std::cout << "(LuaAPI) Script Error: argument/stack count mismatch." << std::endl;
		}
		
		// call the sandbox function (which in turn calls the script)
		if (lua_pcall(mState, (lua_gettop(mState) - stackSize) - 1, LUA_MULTRET, 0) != 0) {
			std::string errorMsg = ReadStack<std::string>(stackSize + 1);
			lua_settop(mState, stackSize);
			throw std::runtime_error(errorMsg);
		}
		
		if (ReadStack<bool>(stackSize + 1)) { // if script was called successfully...
			lua_settop(mState, stackSize);
		}
		else {
			std::string errorMsg = ReadStack<std::string>(stackSize + 2);
			lua_settop(mState, stackSize);
			throw std::runtime_error(errorMsg);
		}
	}
	
	template <typename R, typename ...Ps>
	R LuaAPI::CallString(const std::string& script, const Ps&... params) {
		int stackSize = lua_gettop(mState);
		
		lua_getglobal(mState, "uair_run");
		PushStack(script, params...);
		
		if ((lua_gettop(mState) - stackSize) - 1 != sizeof...(params) + 1) {
			std::cout << "(LuaAPI) Script Error: argument/stack count mismatch." << std::endl;
		}
		
		if (lua_pcall(mState, (lua_gettop(mState) - stackSize) - 1, LUA_MULTRET, 0) != 0) {
			std::string errorMsg = ReadStack<std::string>(stackSize + 1);
			lua_settop(mState, stackSize);
			throw std::runtime_error(errorMsg);
		}
		
		if (ReadStack<bool>(stackSize + 1)) {
			R result;
			
			result = ReadStack<R>(stackSize + 2); // get the return value from the stack
			lua_settop(mState, stackSize);
			return result;
		}
		else {
			std::string errorMsg = ReadStack<std::string>(stackSize + 2);
			
			lua_settop(mState, stackSize);
			throw std::runtime_error(errorMsg);
		}
	}
	
	template <typename R1, typename R2, typename ...Rs, typename ...Ps>
	std::tuple<R1, R2, Rs...> LuaAPI::CallString(const std::string& script, const Ps&... params) {
		int stackSize = lua_gettop(mState);
		
		lua_getglobal(mState, "uair_run");
		PushStack(script, params...);
		
		if ((lua_gettop(mState) - stackSize) - 1 != sizeof...(params) + 1) {
			std::cout << "(LuaAPI) Script Error: argument/stack count mismatch." << std::endl;
		}
		
		if (lua_pcall(mState, (lua_gettop(mState) - stackSize) - 1, LUA_MULTRET, 0) != 0) {
			std::string errorMsg = ReadStack<std::string>(stackSize + 1);
			lua_settop(mState, stackSize);
			throw std::runtime_error(errorMsg);
		}
		
		if (ReadStack<bool>(stackSize + 1)) {
			std::tuple<R1, R2, Rs...> result;
			
			result = ReadStack<R1, R2, Rs...>(stackSize + 2); // get all return values from the stack
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

//
	template <typename P>
	void LuaAPI::PushStack(const P& value) {
		auto indexType = mIndexTypeMap.find(std::type_index(typeid(P)));
		if (indexType != mIndexTypeMap.end()) { // if the type we're pushing has been registered...
			void* userdata = lua_newuserdata(mState, sizeof(P*));
			
			// attempt to retrieve the metatable associated with registered type...
			if (!luaL_newmetatable(mState, ((indexType->second).GetName() + ".mt").c_str())) {
				// create a new instance of type (P) and assign the passed in value to it
				// if special attention is needed (such as no default constructor, move semantics, etc)
				// then the template should be specialised for the type (P) instead
				P** data = static_cast<P**>(userdata);
				*data = new P();
				**data = value;
				
				lua_setmetatable(mState, -2);
			}
			else { // if the metatable didn't exist (i.e., was just created)...
				// clean up the newly created metatable
				lua_pushnil(mState);
				lua_setfield(mState, LUA_REGISTRYINDEX, ((indexType->second).GetName() + ".mt").c_str());
				std::cout << "(LuaAPI) Stack Error (Push): metatable with name " << (indexType->second).GetName() << ".mt doesn't exist in current Lua State." << std::endl;
			}
		}
		else { // if the type wasn't registered...
			std::cout << "(LuaAPI) Stack Error (Push): unregistered type (pushing nil value instead)." << std::endl;
			lua_pushnil(mState);
		}
	}
	
	template <typename P, typename... Ps>
	void LuaAPI::PushStack(const P& value, const Ps&... values) {
		PushStack(value); // call the matching specialisation of Push for T
		PushStack(values...); // recursive call placing the first of the remaining arguments Ps as T, and the rest as Ps (if any)
	}
	
	template <typename P>
	void LuaAPI::PushStack(const unsigned int& counter, const P& value) {
		auto apiInstance = mAPIInstances.find(counter); // retrieve the instance matching counter
		if (apiInstance != mAPIInstances.end()) { // if a matching instance was found...
			(apiInstance->second)->PushStack(value); // call that instance's matching push function
		}
		else {
			std::cout << "(LuaAPI) Stack Error (Push): invalid LuaAPI instance counter." << std::endl;
		}
	}
	
	template <typename P, typename... Ps>
	void LuaAPI::PushStack(const unsigned int& counter, const P& value, const Ps&... values) {
		auto apiInstance = mAPIInstances.find(counter);
		if (apiInstance != mAPIInstances.end()) {
			(apiInstance->second)->PushStack(value, values...);
		}
		else {
			std::cout << "(LuaAPI) Stack Error (Push): invalid LuaAPI instance counter." << std::endl;
		}
	}
//

//
	template <typename R>
	R LuaAPI::ReadStack(const int& index) {
		// create a new instance of type (R) to be used as the return value
		// if special attention is needed (such as no default constructor, move semantics, etc)
		// then the template should be specialised for the type (R) instead
		R result;
		
		auto indexType = mIndexTypeMap.find(std::type_index(typeid(R)));
		if (indexType == mIndexTypeMap.end()) {
			std::cout << "(LuaAPI) Stack Error (Read): unregistered type." << std::endl;
			return result;
		}
		
		void* userdata = luaL_checkudata(mState, index, ((indexType->second).GetName() + ".mt").c_str());
		if (userdata == nullptr) {
			std::cout << "(LuaAPI) Stack Error (Read): mismatched type." << std::endl;
			return result;
		}
		
		R** data = static_cast<R**>(userdata);
		result = **data;
		return result;
	}

	template <typename R>
	R LuaAPI::ReadStack(const unsigned int& counter, const int& index) {
		auto apiInstance = mAPIInstances.find(counter);
		if (apiInstance != mAPIInstances.end()) {
			return (apiInstance->second)->ReadStack<R>(index);
		}
		
		std::cout << "(LuaAPI) Stack Error (Read): invalid LuaAPI instance counter." << std::endl;
		R result;
		return result;
	}
//

// template specialisations to read basic types from the lua stack
	template <>
	inline bool LuaAPI::ReadStack(const int& index) {
		if (lua_isboolean(mState, index)) { // if the value on the stack matches requested type...
			return lua_toboolean(mState, index); // return the value from the stack
		}
		
		std::cout << "(LuaAPI) Stack Error (Read): invalid boolean." << std::endl;
		return false;
	}
	
	template <>
	inline int LuaAPI::ReadStack(const int& index) {
		if (lua_isnumber(mState, index)) {
			return lua_tointeger(mState, index);
		}
		
		std::cout << "(LuaAPI) Stack Error (Read): invalid integer." << std::endl;
		return 0;
	}
	
	template <>
	inline float LuaAPI::ReadStack(const int& index) {
		if (lua_isnumber(mState, index)) {
			return lua_tonumber(mState, index);
		}
		
		std::cout << "(LuaAPI) Stack Error (Read): invalid float." << std::endl;
		return 0.0f;
	}
	
	template <>
	inline LuaAPI::Nil LuaAPI::ReadStack(const int& index) {
		if (!lua_isnil(mState, index)) {
			std::cout << "(LuaAPI) Stack Error (Read): invalid nil." << std::endl;
		}
		
		return LuaAPI::Nil();
	}
	
	template <>
	inline std::string LuaAPI::ReadStack(const int& index) {
		if (lua_isstring(mState, index)) {
			return lua_tostring(mState, index);
		}
		
		std::cout << "(LuaAPI) Stack Error (Read): invalid string." << std::endl;
		return "";
	}
	
	template <>
	inline bool LuaAPI::ReadStack(const unsigned int& counter, const int& index) {
		auto apiInstance = mAPIInstances.find(counter);
		if (apiInstance != mAPIInstances.end()) { // if no instance was found...
			return (apiInstance->second)->ReadStack<bool>(index);
		}
		
		std::cout << "(LuaAPI) Stack Error (Read): invalid LuaAPI instance counter." << std::endl;
		return false;
	}
	
	template <>
	inline int LuaAPI::ReadStack(const unsigned int& counter, const int& index) {
		auto apiInstance = mAPIInstances.find(counter);
		if (apiInstance != mAPIInstances.end()) { // if no instance was found...
			return (apiInstance->second)->ReadStack<int>(index);
		}
		
		std::cout << "(LuaAPI) Stack Error (Read): invalid LuaAPI instance counter." << std::endl;
		return 0;
	}
	
	template <>
	inline float LuaAPI::ReadStack(const unsigned int& counter, const int& index) {
		auto apiInstance = mAPIInstances.find(counter);
		if (apiInstance != mAPIInstances.end()) { // if no instance was found...
			return (apiInstance->second)->ReadStack<float>(index);
		}
		
		std::cout << "(LuaAPI) Stack Error (Read): invalid LuaAPI instance counter." << std::endl;
		return 0.0f;
	}
	
	template <>
	inline LuaAPI::Nil LuaAPI::ReadStack(const unsigned int& counter, const int& index) {
		auto apiInstance = mAPIInstances.find(counter);
		if (apiInstance != mAPIInstances.end()) { // if no instance was found...
			return (apiInstance->second)->ReadStack<LuaAPI::Nil>(index);
		}
		
		std::cout << "(LuaAPI) Stack Error (Read): invalid LuaAPI instance counter." << std::endl;
		return LuaAPI::Nil();
	}
	
	template <>
	inline std::string LuaAPI::ReadStack(const unsigned int& counter, const int& index) {
		auto apiInstance = mAPIInstances.find(counter);
		if (apiInstance != mAPIInstances.end()) { // if no instance was found...
			return (apiInstance->second)->ReadStack<std::string>(index);
		}
		
		std::cout << "(LuaAPI) Stack Error (Read): invalid LuaAPI instance counter." << std::endl;
		return "";
	}
//

//
	template <typename P1, typename P2, typename... Ps>
	std::tuple<P1, P2, Ps...> LuaAPI::ReadStack(int index) {
		std::tuple<P1, P2, Ps...> result; // create an empty tuple matching the requested parameters
		
		auto fillTuple = [this, &index](auto &tupleElement) {
			// set the current tuple element to the result returned from the stack by calling the appropiate read function
			// (we need to strip the reference off the type of tupleElement e.g. int& -> int)
			tupleElement = ReadStack< std::remove_reference_t<decltype(tupleElement)> >(index++);
		};
		
		util::IterateTuple(fillTuple, result, std::make_index_sequence<std::tuple_size<decltype(result)>::value>());
		
		return result;
	}
	
	template <typename P1, typename P2, typename... Ps>
	std::tuple<P1, P2, Ps...> LuaAPI::ReadStack(const unsigned int& counter, int index) {
		auto apiInstance = mAPIInstances.find(counter);
		if (apiInstance != mAPIInstances.end()) { // if no instance was found...
			return (apiInstance->second)->ReadStack<P1, P2, Ps...>(index);
		}
		
		std::tuple<P1, P2, Ps...> result;
		return result;
	}
//

template <typename T>
bool LuaAPI::RegisterType(const std::string& name) {
	// split the name into parts (0, 1 ... n-1 => namespace, namespace ... typename)
	std::vector<std::string> parts = util::SplitString(name, '.');
	if (parts.size() > 1u) { // if there was at least one namespace...
		size_t size = parts.size() - 1u; // discount the final entry (typename)
		std::string partName = ""; // the full namespace
		
		for (unsigned int i = 0u; i < size; ++i) { // for all namespaces...
			partName += parts.at(i); // add the namespace to the full namespace
			
			auto nameIndex = mNameIndexMap.find(partName); // check if there is a type matching the current full namespace
			if (nameIndex != mNameIndexMap.end()) { // if a matching type was already registered...
				std::cout << "(LuaAPI) Type Registration Error: namespace with name " << partName << " is already a registered type." << std::endl;
				return false;
			}
			
			partName += '.';
		}
	}
	
	// attempt to retrieve the type registered with name
	auto nameIndex = mNameIndexMap.find(name);
	if (nameIndex != mNameIndexMap.end()) { // if a type using name was already registered...
		std::cout << "(LuaAPI) Type Registration Error: a type with name " << name << " is already registered." << std::endl;
		return false;
	}
	
	// attempt to retrieve the "registered type object" registered with type
	auto indexType = mIndexTypeMap.find(std::type_index(typeid(T)));
	if (indexType != mIndexTypeMap.end()) { // if the type was already registered...
		std::cout << "(LuaAPI) Type Registration Error: type is already registered under a different name." << std::endl;
		return false;
	}
	
	// attempt to create a new metatable on the lua registry with name
	if (!luaL_newmetatable(mState, (name + ".mt").c_str())) { // if the metatable already exists...
		std::cout << "(LuaAPI) Type Registration Error: metatable with name " << name << ".mt already exists in current Lua State." << std::endl;
		return false;
	}
	
	// assign the metatable as its own __index metamethod
	lua_pushliteral(mState, "__index");
	lua_pushvalue(mState, -2);
	lua_settable(mState, -3);
	
	// create a new registered type object and add a destructor to it
	RegisteredType newType(mCounter, name);
	newType.AddDestructor<T>(mState);
	
	// add entries for the new type in both maps
	mNameIndexMap.insert(std::make_pair(name, std::type_index(typeid(T))));
	mIndexTypeMap.insert(std::make_pair(std::type_index(typeid(T)), std::move(newType)));
	
	return true;
}

template <typename T, typename... Ps>
bool LuaAPI::RegisterConstructor() {
	// retrieve the registered type object for the type (T)
	auto indexType = mIndexTypeMap.find(std::type_index(typeid(T)));
	if (indexType == mIndexTypeMap.end()) { // if the type isn't registered...
		std::cout << "(LuaAPI) Constructor Registration Error: type not registered." << std::endl;
		return false;
	}
	
	// attempt to create a new metatable on the lua registry with name
	if (luaL_newmetatable(mState, ((indexType->second).GetName() + ".mt").c_str())) { // if the metatable doesn't exist...
		// remove the newly created (invalid) metatable from the lua registry
		lua_pushnil(mState);
		lua_setfield(mState, LUA_REGISTRYINDEX, ((indexType->second).GetName() + ".mt").c_str());
		
		std::cout << "(LuaAPI) Constructor Registration Error: metatable with name " << (indexType->second).GetName() << ".mt doesn't exist in current Lua State." << std::endl;
		return false;
	}
	
	(indexType->second).AddConstructor<T, Ps...>(mState);
	
	return true;
}

template <typename T, typename R, typename... Ps>
bool LuaAPI::RegisterFunction(const std::string& funcName, R(T::*func)(Ps...)) {
	auto indexType = mIndexTypeMap.find(std::type_index(typeid(T)));
	if (indexType == mIndexTypeMap.end()) {
		std::cout << "(LuaAPI) Function Registration Error: type not registered." << std::endl;
		return false;
	}
	
	if (luaL_newmetatable(mState, ((indexType->second).GetName() + ".mt").c_str())) {
		lua_pushnil(mState);
		lua_setfield(mState, LUA_REGISTRYINDEX, ((indexType->second).GetName() + ".mt").c_str());
		
		std::cout << "(LuaAPI) Function Registration Error: metatable with name " << (indexType->second).GetName() << ".mt doesn't exist in current Lua State." << std::endl;
		return false;
	}
	
	(indexType->second).AddFunction<T, R, Ps...>(mState, funcName, func);
	
	return true;
}
}
