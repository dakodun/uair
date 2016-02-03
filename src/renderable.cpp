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

#include "renderable.hpp"

namespace uair {
std::string Renderable::GetName() const {
	return mName;
}

float Renderable::GetDepth() const {
	return mDepth;
}

void Renderable::SetDepth(const float& newDepth) {
	mDepth = std::min(std::max(newDepth, -1000.0f), 999.0f); // cap the depth between -1000 and 999 inclusive
}

glm::vec3 Renderable::GetColour() const {
	return mColour;
}

void Renderable::SetColour(const glm::vec3& newColour) {
	mColour = newColour;
}

float Renderable::GetAlpha() const {
	return mAlpha;
}

void Renderable::SetAlpha(const float& newAlpha) {
	mAlpha = newAlpha;
}
}
