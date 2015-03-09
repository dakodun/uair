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

#include "shaderprogram.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

#include "openglstates.hpp"

namespace uair {
ShaderProgram::~ShaderProgram() {
	Clear();
}

void ShaderProgram::LinkProgram() {
	EnsureInitialised();
	
	GLint vertShaderID = 0;
	bool vertShaderAttached = false;
	
	if (mVertShaderString.empty() == false) { // if we have a valid vertex shader string
		vertShaderID = glCreateShader(GL_VERTEX_SHADER); // create a new vertex shader
		
		const char * c_str = mVertShaderString.c_str();
		glShaderSource(vertShaderID, 1, &c_str, NULL); // set the shader's source
		glCompileShader(vertShaderID); // compile the shader
		
		GLint status = GL_FALSE;
		glGetShaderiv(vertShaderID, GL_COMPILE_STATUS, &status);
		
		if (status == GL_TRUE) { // if the compilation was successful
			glAttachShader(mProgramID, vertShaderID); // attach the shader to our program
			
			// if successful
				vertShaderAttached = true;
		}
		else {
			std::cout << "Compilation of the Vertex Shader failed." << std::endl; // error
		}
	}
	
	GLint fragShaderID = 0;
	bool fragShaderAttached = false;
	
	if (mFragShaderString.empty() == false) { // if we have a valid fragment shader string
		fragShaderID = glCreateShader(GL_FRAGMENT_SHADER); // create a new fragment shader
		
		const char * c_str = mFragShaderString.c_str();
		glShaderSource(fragShaderID, 1, &c_str, NULL);
		glCompileShader(fragShaderID);
		
		GLint status = GL_FALSE;
		glGetShaderiv(fragShaderID, GL_COMPILE_STATUS, &status);
		
		if (status == GL_TRUE) {
			glAttachShader(mProgramID, fragShaderID);
			
			// if successful
				fragShaderAttached = true;
		}
		else {
			std::cout << "Compilation of the Fragment Shader failed." << std::endl; // error
		}
	}
	
	glLinkProgram(mProgramID); // link the shader program
	
	{
		if (vertShaderID != 0) {
			if (vertShaderAttached) {
				glDetachShader(mProgramID, vertShaderID);
			}
			
			glDeleteShader(vertShaderID);
		}
		
		if (fragShaderID != 0) {
			if (fragShaderAttached) {
				glDetachShader(mProgramID, fragShaderID);
			}
			
			glDeleteShader(fragShaderID);
		}
	}
	
	{
		GLint status = GL_FALSE;
		glGetProgramiv(mProgramID, GL_LINK_STATUS, &status);
		
		if (status == GL_FALSE) { // if the shader program failed to link
			std::cout << "Linking of the Program failed." << std::endl; // error
		}
	}
}

void ShaderProgram::UseProgram() {
	OpenGLStates::UseProgram(mProgramID);
}

void ShaderProgram::SetVertexShaderFromFile(const std::string & vertexShaderFile) {
	// read the shader from the file into the buffer
	std::ifstream vertShaderFile(vertexShaderFile.c_str());
	std::stringstream vertShaderBuffer;
	vertShaderBuffer << vertShaderFile.rdbuf();
	vertShaderFile.close();
	
	SetVertexShaderFromString(vertShaderBuffer.str()); // load the shader from the string
}

void ShaderProgram::VertexFromFile(const std::string & vertexShaderFile) {
	SetVertexShaderFromFile(vertexShaderFile);
}

void ShaderProgram::SetVertexShaderFromString(const std::string & vertexShaderString) {
	if (!vertexShaderString.empty()) { // if the string isn't empty
		mVertShaderString = vertexShaderString;
	}
	else {
		std::cout << "Unable to set Vertex Shader: shader string is empty." << std::endl; // error
	}
}

void ShaderProgram::VertexFromString(const std::string & vertexShaderString) {
	SetVertexShaderFromString(vertexShaderString);
}

void ShaderProgram::SetFragmentShaderFromFile(const std::string & fragmentShaderFile) {
	// read the shader from the file into the buffer
	std::ifstream fragShaderFile(fragmentShaderFile.c_str());
	std::stringstream fragShaderBuffer;
	fragShaderBuffer << fragShaderFile.rdbuf();
	fragShaderFile.close();
	
	SetFragmentShaderFromString(fragShaderBuffer.str()); // load the shader from the string
}

void ShaderProgram::FragmentFromFile(const std::string & fragmentShaderFile) {
	SetFragmentShaderFromFile(fragmentShaderFile);
}

void ShaderProgram::SetFragmentShaderFromString(const std::string & fragmentShaderString) {
	if (!fragmentShaderString.empty()) { // if the string isn't empty
		mFragShaderString = fragmentShaderString;
	}
	else {
		std::cout << "Unable to set Fragment Shader: shader string is empty." << std::endl; // error
	}
}

void ShaderProgram::FragmentFromString(const std::string & fragmentShaderString) {
	SetFragmentShaderFromString(fragmentShaderString);
}

void ShaderProgram::EnsureInitialised() {
	if (mProgramID == 0) {
		mProgramID = glCreateProgram(); // create a glsl shader program and store the id
	}
}

void ShaderProgram::Clear() {
	if (mProgramID != 0) {
		if (OpenGLStates::mCurrentProgram == mProgramID) { // if our program is in use
			glUseProgram(0);
			OpenGLStates::mCurrentProgram = 0;
		}
		
		glDeleteProgram(mProgramID); // destroy our glsl shader program
		mProgramID = 0;
	}
}

GLint ShaderProgram::GetProgramID() const {
	return mProgramID;
}
}
