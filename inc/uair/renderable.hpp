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

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "polygon.hpp"

namespace uair {
class RenderBatchData;

class Renderable {
	friend class RenderBatch;
	
	public :
		virtual ~Renderable() = default;
		
		glm::vec2 GetPosition() const;
		void SetPosition(const glm::vec2 & newPos);
		glm::vec2 GetOrigin() const;
		void SetOrigin(const glm::vec2 & newOrigin);
		
		float GetDepth() const;
		void SetDepth(const float & newDepth);
		glm::vec3 GetColour() const;
		void SetColour(const glm::vec3 & newColour);
		float GetAlpha() const;
		void SetAlpha(const float & newAlpha);
		
		glm::vec2 GetScale() const;
		void SetScale(const glm::vec2 & newScale);
		float GetRotation() const;
		void SetRotation(const float & newRotation);
		glm::vec2 GetSkew() const;
		void SetSkew(const glm::vec2 & newSkew);
		
		Polygon GetLocalBoundingBox() const;
		Polygon GetGlobalBoundingBox() const;
		
		Polygon GetLocalMask() const;
		void SetLocalMask();
		void SetLocalMask(const Polygon & newMask);
		Polygon GetGlobalMask() const;
		
		virtual std::string GetTag() const = 0;
		std::string GetName() const;
	protected :
		virtual void UpdateGlobalBoundingBox() = 0;
		virtual void UpdateGlobalMask() = 0;
		virtual void CreateLocalMask() = 0;
		virtual RenderBatchData Upload() = 0;
	protected :
		std::string mName = "";
		
		glm::vec2 mPosition = glm::vec2();
		glm::vec2 mOrigin = glm::vec2();
		
		float mDepth = 0.0f;
		glm::vec3 mColour = glm::vec3(1.0f, 1.0f, 1.0f);
		float mAlpha = 1.0f;
		
		glm::mat3 mTransformation = glm::mat3();
		glm::vec2 mScale = glm::vec2(1.0f, 1.0f);
		float mRotation = 0;
		glm::vec2 mSkew = glm::vec2();
		
		Polygon mLocalBoundingBox;
		Polygon mGlobalBoundingBox;
		
		Polygon mLocalMask;
		Polygon mGlobalMask;
};
}

#endif
