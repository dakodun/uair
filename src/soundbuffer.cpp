/* **************************************************************** **
**	Uair Engine
**	Copyright (c) 2010 - 2017, Iain M. Crawford
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

#include "soundbuffer.hpp"

namespace uair {
SoundBuffer::SoundBuffer(SoundBuffer&& other) : SoundBuffer() {
	swap(*this, other);
}

SoundBuffer::~SoundBuffer() {
	
}

SoundBuffer& SoundBuffer::operator=(SoundBuffer other) {
	swap(*this, other);
	
	return *this;
}

void swap(SoundBuffer& first, SoundBuffer& second) {
	std::swap(first.mSampleData, second.mSampleData);
	std::swap(first.mNumChannels, second.mNumChannels);
}

void SoundBuffer::LoadFromFile(const std::string& filename) {
	using namespace std::placeholders;
	
	std::ifstream oggFile(filename.c_str(), std::ios::in | std::ios::binary); // open the ogg file on disk
	
	SoundLoaderOgg slo;
	slo.Decode(oggFile, std::bind(&SoundBuffer::DecoderCallback, this, _1, _2, _3, _4)); // decode the entire ogg file
}

unsigned int SoundBuffer::GetTypeID() {
	return static_cast<unsigned int>(Resources::SoundBuffer);
}

int SoundBuffer::DecoderCallback(const int& serial, const int& numChannels, const int& numSamples, std::vector<int>& samples) {
	mSampleData.insert(mSampleData.end(), samples.begin(), samples.end()); // add the samples into the array
	mNumChannels = numChannels; // set the number of channels
	
	return numSamples; // return the number of samples used (all of them)
}
}
