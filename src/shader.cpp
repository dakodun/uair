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

#include "shader.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

#include "openglstates.hpp"
#include "vbo.hpp"

namespace uair {
Shader::Shader(Shader&& other) : Shader() {
	swap(*this, other);
}

Shader::~Shader() {
	
}

Shader& Shader::operator=(Shader other) {
	swap(*this, other);
	
	return *this;
}

void swap(Shader& first, Shader& second) {
	using std::swap;
	
	swap(first.mProgramID, second.mProgramID);
	swap(first.mVertShaderString, second.mVertShaderString);
	swap(first.mFragShaderString, second.mFragShaderString);
}

void Shader::LinkProgram() {
	Initialise();
	
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

void Shader::UseProgram() {
	OpenGLStates::UseProgram(mProgramID);
}

void Shader::VertexFromFile(const std::string& vertexShaderFile) {
	// read the shader from the file into the buffer
	std::ifstream vertShaderFile(vertexShaderFile.c_str());
	std::stringstream vertShaderBuffer;
	vertShaderBuffer << vertShaderFile.rdbuf();
	vertShaderFile.close();
	
	VertexFromString(vertShaderBuffer.str()); // load the shader from the string
}

void Shader::VertexFromString(const std::string& vertexShaderString) {
	if (!vertexShaderString.empty()) { // if the string isn't empty
		mVertShaderString = vertexShaderString;
	}
	else {
		std::cout << "Unable to set Vertex Shader: shader string is empty." << std::endl; // error
	}
}

void Shader::FragmentFromFile(const std::string& fragmentShaderFile) {
	// read the shader from the file into the buffer
	std::ifstream fragShaderFile(fragmentShaderFile.c_str());
	std::stringstream fragShaderBuffer;
	fragShaderBuffer << fragShaderFile.rdbuf();
	fragShaderFile.close();
	
	FragmentFromString(fragShaderBuffer.str()); // load the shader from the string
}

void Shader::FragmentFromString(const std::string& fragmentShaderString) {
	if (!fragmentShaderString.empty()) { // if the string isn't empty
		mFragShaderString = fragmentShaderString;
	}
	else {
		std::cout << "Unable to set Fragment Shader: shader string is empty." << std::endl; // error
	}
}

void Shader::InitCallback() {
	Initialise();
	UseProgram();
	
	mProjectionMatrixLocation = glGetUniformLocation(mProgramID, "vertProj");
	mViewMatrixLocation = glGetUniformLocation(mProgramID, "vertView");
	mModelMatrixLocation = glGetUniformLocation(mProgramID, "vertModel");
	
	mTextureLocation = glGetUniformLocation(mProgramID, "fragBaseTex");
	glUniform1i(mTextureLocation, 0);
}

void Shader::VAOCallback() {
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glEnableVertexAttribArray(6);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VBOVertex), (void*)(offsetof(VBOVertex, mX)));
	glVertexAttribPointer(1, GL_BGRA, GL_UNSIGNED_INT_2_10_10_10_REV, GL_TRUE, sizeof(VBOVertex), (void*)(offsetof(VBOVertex, mNormal)));
	glVertexAttribPointer(2, 2, GL_UNSIGNED_SHORT, GL_TRUE, sizeof(VBOVertex), (void*)(offsetof(VBOVertex, mS)));
	glVertexAttribPointer(3, 4, GL_UNSIGNED_INT_2_10_10_10_REV, GL_FALSE, sizeof(VBOVertex), (void*)(offsetof(VBOVertex, mLWTT)));
	glVertexAttribPointer(4, 4, GL_UNSIGNED_SHORT, GL_TRUE, sizeof(VBOVertex), (void*)(offsetof(VBOVertex, mMinS)));
	glVertexAttribPointer(5, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(VBOVertex), (void*)(offsetof(VBOVertex, mR)));
	glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, sizeof(VBOVertex), (void*)(offsetof(VBOVertex, mScaleS)));
}

void Shader::RenderCallback() {
	glUniformMatrix4fv(mViewMatrixLocation, 1, GL_FALSE, &OpenGLStates::mViewMatrix[0][0]);
	glUniformMatrix4fv(mModelMatrixLocation, 1, GL_FALSE, &OpenGLStates::mModelMatrix[0][0]);
	glUniformMatrix4fv(mProjectionMatrixLocation, 1, GL_FALSE, &OpenGLStates::mProjectionMatrix[0][0]);
}

void Shader::Initialise() {
	if (mProgramID == 0) {
		mProgramID = glCreateProgram(); // create a glsl shader program and store the id
	}
}

void Shader::Clear() {
	if (mProgramID != 0) {
		if (OpenGLStates::mCurrentProgram == mProgramID) { // if our program is in use
			glUseProgram(0);
			OpenGLStates::mCurrentProgram = 0;
		}
		
		glDeleteProgram(mProgramID); // destroy our glsl shader program
		mProgramID = 0;
	}
}

GLuint Shader::GetProgramID() const {
	return mProgramID;
}

unsigned int Shader::GetTypeID() {
	return static_cast<unsigned int>(Resources::Shader);
}
}
