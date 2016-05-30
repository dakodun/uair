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

#ifndef UAIRSOUNDBUFFER_HPP
#define UAIRSOUNDBUFFER_HPP

#include <atomic>

#include "RtAudio.h"
#include "resource.hpp"
#include "soundloaderogg.hpp"

namespace uair {
class SoundBuffer : public Resource {
	public :
		SoundBuffer() = default;
		SoundBuffer(const SoundBuffer& other) = delete;
		SoundBuffer(SoundBuffer&& other);
		~SoundBuffer();
		
		SoundBuffer& operator=(SoundBuffer other);
		
		friend void swap(SoundBuffer& first, SoundBuffer& second);
		
		void LoadFromFile(const std::string& filename);
		
		static unsigned int GetTypeID();
	private :
		int DecoderCallback(const int& serial, const int& numChannels, const int& numSamples, std::vector<int>& samples);
	public :
		std::vector<int> mSampleData; // array holding the sample data
		unsigned int mNumChannels = 1u; // the number of playback channels
};
}

#endif
