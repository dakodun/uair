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

#include "init.hpp"

namespace uair {
class SegmentInfo;
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
		VBO(const VBO& copyFrom) = delete;
		~VBO();
		
		VBO & operator=(const VBO& other) = delete;
		
		void AddData(const std::vector<VBOVertex>& vertData, const std::vector<VBOIndex>& indData);
		void AddData(const std::vector<VBOVertex>& vertData, const std::vector<VBOIndex>& indData, const std::map< unsigned int, std::vector<SegmentInfo> >& segmentInfo);
		void Draw(const unsigned int& pass = 0u);
		void Draw(const FBO& fbo, const unsigned int& pass = 0u);
		
		void EnsureInitialised();
	private :
		void Draw(const unsigned int& targetID, const unsigned int& pass);
	public :
		GLenum mType = GL_DYNAMIC_DRAW;
		std::size_t mMinimumSize = 0;
	private :
		typedef std::pair< unsigned int, std::vector<SegmentInfo> > IndexedSegmentInfo; // unsigned int/segment info vector pair
		
		GLuint mVertVBOID = 0;
		GLuint mIndVBOID = 0;
		
		std::map< unsigned int, std::vector<SegmentInfo> > mSegmentInfo;
};
}

#endif
