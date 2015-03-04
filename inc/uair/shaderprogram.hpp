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
* \file		shaderprogram.hpp
* \brief	ShaderProgram.
**/

#ifndef UAIRSHADERPROGRAM_HPP
#define UAIRSHADERPROGRAM_HPP

#include <string>

#include "init.hpp"

namespace uair {
class ShaderProgram {
	public :
		// [!] update copy/move/destruct/assign/swap
		ShaderProgram() = default;
		ShaderProgram(const ShaderProgram & copyFrom) = delete;
		~ShaderProgram();
		
		ShaderProgram & operator=(const ShaderProgram & other) = delete;
		
		void LinkProgram();
		void UseProgram();
		
		void SetVertexShaderFromFile(const std::string & vertexShaderFile);
		void VertexFromFile(const std::string & vertexShaderFile);
		void SetVertexShaderFromString(const std::string & vertexShaderString);
		void VertexFromString(const std::string & vertexShaderString);
		
		void SetFragmentShaderFromFile(const std::string & fragmentShaderFile);
		void FragmentFromFile(const std::string & fragmentShaderFile);
		void SetFragmentShaderFromString(const std::string & fragmentShaderString);
		void FragmentFromString(const std::string & fragmentShaderString);
		
		void EnsureInitialised();
		void Clear();
		
		GLint GetProgramID() const;
	private :
		GLuint mProgramID = 0;
		
		std::string mVertShaderString;
		std::string mFragShaderString;
};
}

#endif
