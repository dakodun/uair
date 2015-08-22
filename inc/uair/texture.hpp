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

#ifndef UAIRTEXTURE_HPP
#define UAIRTEXTURE_HPP

#include <string>
#include <vector>

#include "init.hpp"
#include "resource.hpp"

namespace uair {
class FBO;

class Texture : public Resource<Texture> {
	public :
		struct LayerData { // data relating to an individual layer in a texture array
			unsigned int mWidth; // the width of the layer without padding
			unsigned int mHeight; // the height of the layer without padding
			
			float mSMax; // the max s coordinate without padding
			float mTMax; // the max t coordinate without padding
		};
		
		Texture() = default;
		Texture(const Texture& other) = delete;
		Texture(Texture&& other);
		~Texture();
		
		Texture& operator=(Texture other);
		
		friend void swap(Texture& first, Texture& second);
		
		bool AddFromFile(const std::string& filename, const bool& flip = true); // add a layer to the texture from an image file
		bool AddFromMemory(std::vector<unsigned char> data, const unsigned int& width, const unsigned int& height, const bool& flip = true); // add a layer to the texture from memory
		
		bool CreateTexture(); // create the texture with the added layers
		
		void EnsureInitialised(); // ensure the texture has been properly set up before use
		void Clear(); // remove resources used by the texture
		
		unsigned int GetTextureID() const; // return the assigned id
		unsigned int GetWidth() const; // return the width
		unsigned int GetHeight() const; // return the height
		unsigned int GetDepth() const; // return the number of layers
		LayerData GetData(const std::size_t& index) const; // return a copy of the texture data
	private :
		GLuint mTextureID = 0; // the assigned id
		
		typedef std::pair< std::vector<unsigned char>, LayerData> TexelLayerPair;
		std::vector<TexelLayerPair> mData; // texture (texel) data and associated layer data (temporary)
		std::vector<LayerData> mLayerData; // the texture layer store
		unsigned int mWidth; // the width of the texture
		unsigned int mHeight; // the height of the texture
		
		bool mIsTextureNull = true; // indicates if the texture has actual data
};
}

#endif
