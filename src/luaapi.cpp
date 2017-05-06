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

#ifndef UAIRNOLUA
#include "luaapi.hpp"

namespace uair {
LuaAPI::RegisteredType::RegisteredType(const unsigned int& counter, const std::string& name) :
		mCounter(counter), mName(name) {
	
	
}

std::string LuaAPI::RegisteredType::GetName() const {
	return mName;
}

int LuaAPI::RegisteredType::CallConstructor(lua_State* l) {
	return mConstructor->Call(l, mCounter, mName); // call the stored constructor (guaranteed to exist if type is callable)
}

int LuaAPI::RegisteredType::CallDestructor(lua_State* l) {
	return mDestructor->Call(l, mCounter, mName); // call the stored destructor (guaranteed to exist if type exists)
}

int LuaAPI::RegisteredType::CallFunction(lua_State* l, const std::string& funcName) {
	// attampt to retrieve and call the stored function pointer with the matching name
	auto funcPtr = mFunctions.find(funcName);	
	if (funcPtr != mFunctions.end()) {
		return (funcPtr->second)->Call(l, mCounter, mName);
	}
	
	std::cout << "(LuaAPI) Callback Error (Function): function with name " << funcName << " isn't registered." << std::endl;
	return 0;
}


unsigned int LuaAPI::mAPICounter = 1u;
std::map<unsigned int, LuaAPI*> LuaAPI::mAPIInstances;

LuaAPI::LuaAPI() {
	// set the instance's counter value and add a matching entry to the (static) instance store
	mCounter = mAPICounter++;
	mAPIInstances.emplace(mCounter, this);
	
	mState = luaL_newstate(); // create the lua state
	luaL_openlibs(mState); // register standard lua libraries
	
	if (!CreateSandbox()) { // if the sandbox script can't be created or executed...
		throw std::runtime_error("(LuaAPI) Initialisation Error: unable to properly intialise sandbox.");
	}
}

LuaAPI::~LuaAPI() {
	if (mState) { // if a lua state exists...
		lua_close(mState); // clean it up (not strictly necessary)
	}
	
	// remove the matching entry from the instance store after the state has been cleaned up
	// to ensure things are cleaned up properly
	mAPIInstances.erase(mCounter);
}

void LuaAPI::PushStack(const Nil& value) {
	lua_pushnil(mState); // push the value onto lua's stack
}

void LuaAPI::PushStack(const bool& value) {
	lua_pushboolean(mState, value);
}

void LuaAPI::PushStack(const int& value) {
	lua_pushinteger(mState, value);
}

void LuaAPI::PushStack(const float& value) {
	lua_pushnumber(mState, value);
}

void LuaAPI::PushStack(const std::string& value) {
	lua_pushstring(mState, value.c_str());
}

void LuaAPI::PushStack(const char* value) {
	lua_pushstring(mState, value);
}

void LuaAPI::PushStack(const unsigned int& counter, const Nil& value) {
	auto apiInstance = mAPIInstances.find(counter); // retrieve the instance matching counter
	if (apiInstance != mAPIInstances.end()) { // if a matching instance was found...
		(apiInstance->second)->PushStack(value); // call that instance's matching push function
	}
	else {
		std::cout << "(LuaAPI) Stack Error (Push): invalid LuaAPI instance counter." << std::endl;
	}
}

void LuaAPI::PushStack(const unsigned int& counter, const bool& value) {
	auto apiInstance = mAPIInstances.find(counter);
	if (apiInstance != mAPIInstances.end()) {
		(apiInstance->second)->PushStack(value);
	}
	else {
		std::cout << "(LuaAPI) Stack Error (Push): invalid LuaAPI instance counter." << std::endl;
	}
}

void LuaAPI::PushStack(const unsigned int& counter, const int& value) {
	auto apiInstance = mAPIInstances.find(counter);
	if (apiInstance != mAPIInstances.end()) {
		(apiInstance->second)->PushStack(value);
	}
	else {
		std::cout << "(LuaAPI) Stack Error (Push): invalid LuaAPI instance counter." << std::endl;
	}
}

void LuaAPI::PushStack(const unsigned int& counter, const float& value) {
	auto apiInstance = mAPIInstances.find(counter);
	if (apiInstance != mAPIInstances.end()) {
		(apiInstance->second)->PushStack(value);
	}
	else {
		std::cout << "(LuaAPI) Stack Error (Push): invalid LuaAPI instance counter." << std::endl;
	}
}

void LuaAPI::PushStack(const unsigned int& counter, const std::string& value) {
	auto apiInstance = mAPIInstances.find(counter);
	if (apiInstance != mAPIInstances.end()) {
		(apiInstance->second)->PushStack(value);
	}
	else {
		std::cout << "(LuaAPI) Stack Error (Push): invalid LuaAPI instance counter." << std::endl;
	}
}

void LuaAPI::PushStack(const unsigned int& counter, const char* value) {
	auto apiInstance = mAPIInstances.find(counter);
	if (apiInstance != mAPIInstances.end()) {
		(apiInstance->second)->PushStack(value);
	}
	else {
		std::cout << "(LuaAPI) Stack Error (Push): invalid LuaAPI instance counter." << std::endl;
	}
}

int LuaAPI::OnNew(lua_State* l) {
	// validate and retrieve the instance counter upvalue
	unsigned int counter = 0u;
	if (lua_isnumber(l, lua_upvalueindex(1))) {
		counter = lua_tointeger(l, lua_upvalueindex(1));
	}
	
	// validate and retrieve the type name upvalue
	std::string name = "";
	if (lua_isstring(l, lua_upvalueindex(2))) {
		name = lua_tostring(l, lua_upvalueindex(2));
	}
	
	auto apiInstance = mAPIInstances.find(counter); // retrieve the instance matching counter
	if (apiInstance == mAPIInstances.end()) { // if no instance was found...
		std::cout << "(LuaAPI) Callback Error (Constructor): invalid LuaAPI instance counter." << std::endl;
		return 0; // error
	}
	
	auto nameIndex = (apiInstance->second)->mNameIndexMap.find(name); // retrieve the typeid with matching name
	if (nameIndex == (apiInstance->second)->mNameIndexMap.end()) { // if the typeid doesn't exist...
		std::cout << "(LuaAPI) Callback Error (Constructor): type with name " << name << " isn't registered." << std::endl;
		return 0;
	}
	
	auto indexType = (apiInstance->second)->mIndexTypeMap.find((nameIndex->second)); // retrieve the registered type object with matching typeid
	if (indexType == (apiInstance->second)->mIndexTypeMap.end()) { // if the registered type object doesn't exist...
		std::cout << "(LuaAPI) Callback Error (Constructor): mismatched name/type pair." << std::endl;
		return 0;
	}
	
	return (indexType->second).CallConstructor(l); // call the constructor stored in the retrieved registered type object
}

int LuaAPI::OnDelete(lua_State* l) {
	unsigned int counter = 0u;
	if (lua_isnumber(l, lua_upvalueindex(1))) {
		counter = lua_tointeger(l, lua_upvalueindex(1));
	}
	
	std::string name = "";
	if (lua_isstring(l, lua_upvalueindex(2))) {
		name = lua_tostring(l, lua_upvalueindex(2));
	}
	
	auto apiInstance = mAPIInstances.find(counter);
	if (apiInstance == mAPIInstances.end()) {
		std::cout << "(LuaAPI) Callback Error (Destructor): invalid LuaAPI instance counter." << std::endl;
		return 0;
	}
	
	auto nameIndex = (apiInstance->second)->mNameIndexMap.find(name);
	if (nameIndex == (apiInstance->second)->mNameIndexMap.end()) {
		std::cout << "(LuaAPI) Callback Error (Destructor): type with name " << name << " isn't registered." << std::endl;
		return 0;
	}
	
	auto indexType = (apiInstance->second)->mIndexTypeMap.find((nameIndex->second));
	if (indexType == (apiInstance->second)->mIndexTypeMap.end()) {
		std::cout << "(LuaAPI) Callback Error (Destructor): mismatched name/type pair." << std::endl;
		return 0;
	}
	
	return (indexType->second).CallDestructor(l);
}

int LuaAPI::OnFunction(lua_State* l) {
	unsigned int counter = 0u;
	if (lua_isnumber(l, lua_upvalueindex(1))) {
		counter = lua_tointeger(l, lua_upvalueindex(1));
	}
	
	std::string name = "";
	if (lua_isstring(l, lua_upvalueindex(2))) {
		name = lua_tostring(l, lua_upvalueindex(2));
	}
	
	// validate and retrieve the function name upvalue
	std::string funcName = "";
	if (lua_isstring(l, lua_upvalueindex(3))) {
		funcName = lua_tostring(l, lua_upvalueindex(3));
	}
	
	auto apiInstance = mAPIInstances.find(counter);
	if (apiInstance == mAPIInstances.end()) {
		std::cout << "(LuaAPI) Callback Error (Function): invalid LuaAPI instance counter." << std::endl;
		return 0;
	}
	
	auto nameIndex = (apiInstance->second)->mNameIndexMap.find(name);
	if (nameIndex == (apiInstance->second)->mNameIndexMap.end()) {
		std::cout << "(LuaAPI) Callback Error (Function): type with name " << name << " isn't registered." << std::endl;
		return 0;
	}
	
	auto indexType = (apiInstance->second)->mIndexTypeMap.find((nameIndex->second));
	if (indexType == (apiInstance->second)->mIndexTypeMap.end()) {
		std::cout << "(LuaAPI) Callback Error (Function): mismatched name/type pair." << std::endl;
		return 0;
	}
	
	return (indexType->second).CallFunction(l, funcName);
}

bool LuaAPI::AddWhitelist(const std::string& var, std::string alias) {
	if (alias == "") { // if no alias was supplied...
		alias = var; // set the alias the the variable name
	}
	
	std::string key = ""; // the key should initially be blank (no nesting)
	std::string val = alias; // set the value to the entire alias
	
	size_t pos = alias.find_last_of('.'); // find the last dot
	if (pos != std::string::npos) { // if any dots exist...
		key = alias.substr(0, pos); // set the key to everything before the last dot
		val = alias.substr(pos + 1); // set the value to the single item after the last dot
	}
	
	// add (or retrieve existing) map with key, and then add the alias/variable pair to that
	auto result = mWhitelist.insert(std::make_pair(key, std::map<std::string, std::string>()));
	return ((result.first)->second).insert(std::make_pair(val, var)).second;
}

bool LuaAPI::CreateSandbox() {
	// when creating the environment table we need to transform nested aliases (x.y.z) into
	// nested table format (x = {y = {z = ___}})
		std::string env = "local env = {";
		
		if (!mWhitelist.empty()) {
			for (auto map = mWhitelist.begin(); map != mWhitelist.end(); ++map) {
				// process the current key by expanding it into table format
				std::string end = "";
				std::vector<std::string> parts = util::SplitString(map->first, '.');
				for (auto part = parts.begin(); part != parts.end(); ++part) {
					env += *part;
					env += " = {";
					
					end += "}"; // add matching closing braces to be appended later
				}
				
				// add alias/variable pairs into current table
					for (auto entry = map->second.begin(); entry != map->second.end(); ++entry) {
						env += entry->first;
						env += " = ";
						env += entry->second;
						env += ", ";
					}
				//
				
				// remove trailing comma and whitespace and append closing braces
				env.pop_back();
				env.pop_back();
				env += end;
				env += ", ";
			}
			
			// remove trailing comma and whitespace
			env.pop_back();
			env.pop_back();
		}
		
		env += "}";
	//
	
	// create the sandbox script by prepending the new environment table to the sandbox function code
	mSandbox = env + R"(
		
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
	
	// attempt to load the string
	int errNo = luaL_loadstring(mState, mSandbox.c_str());
	if (errNo) {
		std::cout << "(LuaAPI) Sandbox Error: unable to load sandbox script: " << errNo << "." << std::endl;
		return false;
	}
	
	// attempt to run the loaded chunk, updating the global function 'uair_run' to use the new environment table
	errNo = lua_pcall(mState, 0, 0, 0);
	if (errNo) {
		std::cout << "(LuaAPI) Sandbox Error: unable to execute sandbox script: " << errNo << "." << std::endl;
		
		std::string errMsg = ReadStack<std::string>(-1);
		if (errMsg != "") {
			std::cout << "  " << errMsg << std::endl;
		}
		
		return false;
	}
	
	return true;
}
}
#endif
