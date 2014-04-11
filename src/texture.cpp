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
	Clear();
	
	swap(*this, other);
	
	return *this;
}

void swap(Texture& first, Texture& second) {
	using std::swap; 
	
	swap(first.mTextureID, second.mTextureID);
	
	swap(first.mData, second.mData);
	swap(first.mWidth, second.mWidth);
	swap(first.mHeight, second.mHeight);
}

bool Texture::AddFromFile(const std::string & fileName) {
	TextureData tempData;
	std::vector<unsigned char> buffer;
	
	lodepng::load_file(buffer, fileName.c_str());
	unsigned int error = lodepng::decode(tempData.mData, tempData.mWidth, tempData.mHeight, buffer);
	if (error != 0) {
		std::cout << error << ": " << lodepng_error_text(error) << std::endl;
		return false;
	}
	
	mData.push_back(std::move(tempData));
	return true;
}

bool Texture::CreateTexture() {
	mWidth = 0;
	mHeight = 0;
	
	for (auto iter = mData.begin(); iter != mData.end(); ++iter) {
		if (iter->mWidth > mWidth) {
			mWidth = iter->mWidth;
		}
		
		if (iter->mHeight > mHeight) {
			mHeight = iter->mHeight;
		}
	}
	
	mWidth = util::NextPowerOf2(mWidth);
	mHeight = util::NextPowerOf2(mHeight);
	
	std::vector<unsigned char> data;
	for (auto iter = mData.begin(); iter != mData.end(); ++iter) {
		std::size_t count = 0;
		unsigned int padWidth = mWidth - iter->mWidth;
		unsigned int padHeight = mHeight - iter->mHeight;
		
		iter->mSMax = static_cast<float>(iter->mWidth) / mWidth;
		iter->mTMax = static_cast<float>(iter->mHeight) / mHeight;
		
		for (unsigned int i = 0; i < iter->mHeight; ++i) {
			std::size_t max = (count + (iter->mWidth * 4));
			data.insert(data.end(), iter->mData.begin() + count, iter->mData.begin() + max);
			
			for (unsigned int j = 0; j < padWidth; ++j) {
				data.emplace_back(255); data.emplace_back(255);
				data.emplace_back(255); data.emplace_back(0);
			}
			
			count += iter->mWidth * 4;
		}
		
		for (unsigned int i = 0; i < padHeight; ++i) {
			for (unsigned int j = 0; j < iter->mWidth + padWidth; ++j) {
				data.emplace_back(255); data.emplace_back(255);
				data.emplace_back(255); data.emplace_back(0);
			}
		}
	}
	
	EnsureInitialised();
	
	{
		glBindTexture(GL_TEXTURE_2D_ARRAY, mTextureID);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		
		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, mWidth, mHeight, mData.size(), 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
		
		OpenGLStates::mCurrentTexture = mTextureID;
	}
	
	return true;
}

void Texture::EnsureInitialised() {
	if (mTextureID == 0) {
		glGenTextures(1, &mTextureID);
	}
}

void Texture::Clear() {
	if (mTextureID != 0) {
		if (OpenGLStates::mCurrentTexture == mTextureID) {
			glBindTexture(GL_TEXTURE_2D, 0);
			OpenGLStates::mCurrentTexture = 0;
		}
		
		glDeleteTextures(1, &mTextureID);
	}
}

unsigned int Texture::GetTextureID() const {
	return mTextureID;
}

unsigned int Texture::GetWidth() const {
	return mWidth;
}

unsigned int Texture::GetHeight() const {
	return mHeight;
}

unsigned int Texture::GetDepth() const {
	return mData.size();
}

TextureData Texture::GetData(const std::size_t & index) const {
	return mData.at(index);
}
}
