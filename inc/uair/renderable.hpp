/* **************************************************************** **
**	Uair Engine
**	Copyright (c) 2014 Iain M. Crawford
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

#ifndef UAIRRENDERABLE_HPP
#define UAIRRENDERABLE_HPP

#include <vector>
#include <string>

#include <glm/glm.hpp>

#include "init.hpp"

namespace uair {
class RenderBatchData;

class Renderable {
	friend class RenderBatch;
	
	public :
		virtual ~Renderable() = default;
		
		virtual std::string GetTag() const = 0;
		std::string GetName() const;
		
		float GetDepth() const;
		void SetDepth(const float& newDepth);
		glm::vec3 GetColour() const;
		void SetColour(const glm::vec3& newColour);
		float GetAlpha() const;
		void SetAlpha(const float& newAlpha);
	protected :
		virtual RenderBatchData Upload() = 0;
	public :
		GLenum mRenderMode = GL_TRIANGLES;
	protected :
		std::string mName = "";
		
		float mDepth = 0.0f;
		glm::vec3 mColour = glm::vec3(1.0f, 1.0f, 1.0f);
		float mAlpha = 1.0f;
};
}

#endif
