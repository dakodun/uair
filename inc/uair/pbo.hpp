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

#ifndef UAIRPBO_HPP
#define UAIRPBO_HPP

#include "init.hpp"

namespace uair {
class PBO {
	public :
		PBO() = default;
		PBO(const PBO& other) = delete;
		PBO(PBO&& other);
		~PBO();
		
		PBO& operator=(PBO other);
		
		friend void swap(PBO& first, PBO& second);
		
		// set up the pbo to have enough memory and performance hints
		void BufferData(const GLenum& target, const GLsizeiptr& size, const GLenum& usage);
		
		// map the buffer to read from
		void MapBuffer(const GLuint& attachmentPoint);
		
		// ensure the pbo is properly initialised before use
		void EnsureInitialised();
		
		// clear the resources used by the pbo
		void Clear();
		
		// return the id assigned to the pbo
		unsigned int GetPBOID() const; 
	private :
		GLuint mPBOID = 0; // the id assigned to the pbo
};
}

#endif
