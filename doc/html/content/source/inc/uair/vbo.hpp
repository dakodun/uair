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

#ifndef UAIRVBO_HPP
#define UAIRVBO_HPP

#include <map>
#include <vector>
#include <memory>

#include "init.hpp"
#include "resourceptr.hpp"
#include "shader.hpp"

namespace uair {
class Segment;
class FBO;

typedef unsigned int VBOIndex;

struct VBOVertex {
	float mX, mY, mZ;
	float mNX, mNY, mNZ;
	float mS, mT, mLayer;
	float mR, mG, mB, mA;
	float mType;
	float mExtra[2];
};

class VBO {
	public :
		VBO() = default;
		VBO(const VBO& other) = delete;
		VBO(VBO&& other);
		~VBO();
		
		VBO& operator=(VBO other);
		
		friend void swap(VBO& first, VBO& second);
		
		void Initialise();
		
		void AddData(const std::vector<VBOVertex>& vertData, const std::vector<VBOIndex>& indData);
		void AddData(const std::vector<VBOVertex>& vertData, const std::vector<VBOIndex>& indData, const std::vector<Segment>& segments);
		void Draw(const unsigned int& pass = 0u);
		void Draw(const FBO& fbo, const unsigned int& pass = 0u);
	private :
		void Draw(const unsigned int& targetID, const unsigned int& pass);
	
	public :
		static ResourcePtr<Shader> mDefaultShader;
		
		GLenum mType = GL_DYNAMIC_DRAW;
		std::size_t mMinimumSize = 0;
	private :
		GLuint mVertVBOID = 0;
		GLuint mIndVBOID = 0;
		
		std::map< unsigned int, std::vector<Segment> > mSegmentInfo;
		std::map<GLuint, GLuint> mVAOStore;
};
}

#endif
