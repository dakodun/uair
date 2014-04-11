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
* \file		exception.cpp
* \brief	Uair exception implementations.
**/

#include "exception.hpp"

namespace uair {
UairException::UairException() : std::runtime_error("A generic Uair exception was thrown.") { 
	
}

UairException::UairException(const std::string& errorString) : std::runtime_error(errorString) { 
	
}

ResourceError::ResourceError() : UairException("There was an error with a ResourceManager method.") { 
	
}

ResourceError::ResourceError(const std::string& errorString) : UairException(errorString) { 
	
}

ResourceExists::ResourceExists() : ResourceError("Resource with the name you requested already exists.") { 
	
}

ResourceInvalid::ResourceInvalid() : ResourceError("Resource with the name you requested does not exist.") { 
	
}

SceneError::SceneError() : UairException("There was an error with a SceneManager method.") { 
	
}

SceneError::SceneError(const std::string& errorString) : UairException(errorString) { 
	
}
}
