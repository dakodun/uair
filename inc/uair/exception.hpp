/* **************************************************************** **
**	Uair Engine
**	Copyright (c) 2013 Iain M. Crawford
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

/** 
* \file		exception.hpp
* \brief	Uair exception declarations.
**/

#ifndef UAIREXCEPTION_HPP
#define UAIREXCEPTION_HPP

#include <stdexcept>
#include <string>

namespace uair {
class UairException : public std::runtime_error {
	public:
		UairException();
		UairException(const std::string& errorString);
};

class ResourceError : public UairException  {
	public:
		ResourceError();
		ResourceError(const std::string& errorString);
};

class ResourceExists : public ResourceError  {
	public:
		ResourceExists();
};

class ResourceInvalid : public ResourceError  {
	public:
		ResourceInvalid();
};


class SceneError : public UairException  {
	public:
		SceneError();
		SceneError(const std::string& errorString);
};
}

#endif
