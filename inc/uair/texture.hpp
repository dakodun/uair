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

#ifndef UAIRTEXTURE_HPP
#define UAIRTEXTURE_HPP

#include <string>
#include <vector>

#include "init.hpp"
#include "resource.hpp"

namespace uair {
struct TextureData {
	std::vector<unsigned char> mData;
	unsigned int mWidth;
	unsigned int mHeight;
	
	float mSMax;
	float mTMax;
};

class Texture : public Resource<Texture> {
	public :
		Texture() = default;
		Texture(const Texture& other) = delete;
		Texture(Texture&& other);
		~Texture();
		
		Texture& operator=(Texture other);
		
		friend void swap(Texture& first, Texture& second);
		
		bool AddFromFile(const std::string& filename);
		
		bool CreateTexture();
		
		void EnsureInitialised();
		void Clear();
		
		unsigned int GetTextureID() const;
		unsigned int GetWidth() const;
		unsigned int GetHeight() const;
		unsigned int GetDepth() const;
		TextureData GetData(const std::size_t& index) const;
	private :
		GLuint mTextureID = 0;
		
		std::vector<TextureData> mData;
		unsigned int mWidth;
		unsigned int mHeight;
};
}

#endif
