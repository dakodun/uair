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

#include "texture.hpp"

#include <iostream>
#include "lodepng.h"

#include "openglstates.hpp"
#include "util.hpp"

namespace uair {
Texture::Texture(Texture&& other) : Texture() {
	swap(*this, other);
}

Texture::~Texture() {
	Clear();
}

Texture& Texture::operator=(Texture other) {
	swap(*this, other);
	
	return *this;
}

void swap(Texture& first, Texture& second) {
	std::swap(first.mTextureID, second.mTextureID);
	
	std::swap(first.mData, second.mData);
	std::swap(first.mWidth, second.mWidth);
	std::swap(first.mHeight, second.mHeight);
	std::swap(first.mIsTextureNull, second.mIsTextureNull);
}

bool Texture::AddFromFile(const std::string & fileName) {
	TextureData tempData; // create temporary texture data
	std::vector<unsigned char> buffer; // create a buffer for the texture data from the image file
	
	lodepng::load_file(buffer, fileName.c_str()); // load the image file
	unsigned int error = lodepng::decode(tempData.mData, tempData.mWidth, tempData.mHeight, buffer); // parse the texture data from the file and store it in the buffer
	if (error != 0) { // if an error occured...
		std::cout << error << ": " << lodepng_error_text(error) << std::endl; // output the error message
		return false; // return failure
	}
	
	if (!tempData.mData.empty() && mIsTextureNull) { // if there is data in the temporary and the texture is currently null...
		mIsTextureNull = false; // indicate texture is no longer null
	}
	
	mData.push_back(std::move(tempData)); // move the temporary into the texture data store
	return true;
}

bool Texture::AddFromMemory(std::vector<unsigned char> data, const unsigned int& width, const unsigned int& height) {
	TextureData tempData; // create temporary texture data
	
	tempData.mData = std::move(data); // move the (copy of) supplied texture data into temporary
	tempData.mWidth = width; tempData.mHeight = height; // set width and height of temporary
	
	if (!tempData.mData.empty() && mIsTextureNull) { // if there is data in the temporary and the texture is currently null...
		mIsTextureNull = false; // indicate texture is no longer null
	}
	
	mData.push_back(std::move(tempData)); // move the temporary into the texture data store
	return true;
}

bool Texture::CreateTexture() {
	mWidth = 0; // reset the width
	mHeight = 0; // reset the height
	
	for (auto iter = mData.begin(); iter != mData.end(); ++iter) { // for all texture layers...
		if (iter->mWidth > mWidth) { // if the width of the current layer is longer than the stored width...
			mWidth = iter->mWidth; // update the stored width
		}
		
		if (iter->mHeight > mHeight) { // if the height of the current layer is longer than the stored height...
			mHeight = iter->mHeight; // update the stored height
		}
	}
	
	mWidth = util::NextPowerOf2(mWidth); // upscale the width to the nearest power of 2
	mHeight = util::NextPowerOf2(mHeight); // upscale the height to the nearest power of 2
	
	std::vector<unsigned char> data; // array of texture data for all layers
	for (auto iter = mData.begin(); iter != mData.end(); ++iter) { // for all texture layers...
		std::size_t count = 0; // the position of the start of the current row
		unsigned int padWidth = mWidth - iter->mWidth; // the amount of width padding required for the current layer
		unsigned int padHeight = mHeight - iter->mHeight; // the amount of height padding required for the current layer
		
		iter->mSMax = static_cast<float>(iter->mWidth) / mWidth; // calculate the max s coordinate without padding
		iter->mTMax = static_cast<float>(iter->mHeight) / mHeight; // calculate the max t coordinate without padding
		
		if (!mIsTextureNull) { // if the texture isn't null...
			for (unsigned int i = 0; i < iter->mHeight; ++i) { // for all rows
				std::size_t max = (count + (iter->mWidth * 4)); // calculate the position of the end of the row
				data.insert(data.end(), iter->mData.begin() + count, iter->mData.begin() + max); // add the row data for the current layer
				
				for (unsigned int j = 0; j < padWidth; ++j) { // for all width padding...
					data.emplace_back(255); data.emplace_back(255); // add a white, transparent texel
					data.emplace_back(255); data.emplace_back(0);
				}
				
				count += iter->mWidth * 4; // update the row start position to the next row
			}
			
			for (unsigned int i = 0; i < padHeight; ++i) { // for all height padding...
				for (unsigned int j = 0; j < iter->mWidth + padWidth; ++j) { // for the width of a row...
					data.emplace_back(255); data.emplace_back(255); // add a white, transparent texel
					data.emplace_back(255); data.emplace_back(0);
				}
			}
		}
	}
	
	EnsureInitialised(); // ensure the texture is properly set up
	
	{
		glBindTexture(GL_TEXTURE_2D_ARRAY, mTextureID); // bind the texture
		OpenGLStates::mCurrentTexture = mTextureID; // set the texture as the currently bound texture
		
		// set the texture's default properties
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		
		if (!mIsTextureNull) { // if the texture isn't a null texture...
			glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, mWidth, mHeight, mData.size(), 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data()); // create the texture using the texture data
		}
		else { // otherwise it is a null texture...
			glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, mWidth, mHeight, mData.size(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL); // create a null texture
		}
	}
	
	return true;
}

void Texture::EnsureInitialised() {
	if (mTextureID == 0) { // if texture has no assigned id...
		glGenTextures(1, &mTextureID); // create texture and store assigned id
	}
}

void Texture::Clear() {
	if (mTextureID != 0) { // if texture has an assigned id...
		if (OpenGLStates::mCurrentTexture == mTextureID) { // if texture is currently bound...
			glBindTexture(GL_TEXTURE_2D, 0); // unbind texture
			OpenGLStates::mCurrentTexture = 0; // set currently bound texture to none
		}
		
		glDeleteTextures(1, &mTextureID); // delete texture
	}
	
	mTextureID = 0; // set assigned id to none
	
	mData.clear(); // clear texture data
	mWidth = 0u; // reset width
	mHeight = 0u; // reset height
	mIsTextureNull = true; // reset texture status
}

unsigned int Texture::GetTextureID() const {
	return mTextureID; // return the assigned id
}

unsigned int Texture::GetWidth() const {
	return mWidth; // return the width
}

unsigned int Texture::GetHeight() const {
	return mHeight; // return the height
}

unsigned int Texture::GetDepth() const {
	return mData.size(); // return the count of stored texture data
}

TextureData Texture::GetData(const std::size_t& index) const {
	return mData.at(index); // return the texture data at the requested index
}
}
