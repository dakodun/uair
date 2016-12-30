/* **************************************************************** **
**	Uair Engine
**	Copyright (c) 2010 - 2016, Iain M. Crawford
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

#ifndef UAIRSHADER_HPP
#define UAIRSHADER_HPP

#include <string>
#include <functional>

#include "init.hpp"
#include "resource.hpp"

namespace uair {
class EXPORTDLL Shader : public Resource {
	public :
		typedef std::function<void ()> SetupCallback;
		
		Shader() = default;
		Shader(const Shader& other) = delete;
		Shader(Shader&& other);
		~Shader();
		
		Shader& operator=(Shader other);
		
		friend void swap(Shader& first, Shader& second);
		
		void LinkProgram();
		void UseProgram();
		
		void VertexFromFile(const std::string& vertexShaderFile);
		void VertexFromString(const std::string& vertexShaderString);
		
		void FragmentFromFile(const std::string& fragmentShaderFile);
		void FragmentFromString(const std::string& fragmentShaderString);
		
		virtual void InitCallback();
		virtual void VAOCallback();
		virtual void RenderCallback();
		
		void Initialise();
		void Clear();
		
		GLuint GetProgramID() const;
		
		static unsigned int GetTypeID();
	
	protected :
		GLuint mProgramID = 0;
		
		std::string mVertShaderString;
		std::string mFragShaderString;
	private :
		GLint mProjectionMatrixLocation;
		GLint mViewMatrixLocation;
		GLint mModelMatrixLocation;
		GLint mTextureLocation;
};
}

#endif
