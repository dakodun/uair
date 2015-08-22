/* **************************************************************** **
**	Uair Engine
**	Copyright (c) 20XX Iain M. Crawford
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

#ifndef UAIRSIGNEDDISTANCEFIELD_HPP
#define UAIRSIGNEDDISTANCEFIELD_HPP

#include <vector>
#include <string>
#include <glm/glm.hpp>

#include "texture.hpp"

namespace uair {
class SignedDistanceField {
	public :
		std::pair<std::vector<unsigned char>, glm::ivec2> GenerateFromFile(const std::string& textureFile);
		std::pair<std::vector<unsigned char>, glm::ivec2> GenerateFromTexture(Texture* texture, const unsigned int& layer);
	private :
		
};
}

#endif