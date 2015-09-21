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

#ifndef UAIRSOUNDLOADEROGG_HPP
#define UAIRSOUNDLOADEROGG_HPP

#include <string>
#include <fstream>
#include <functional>
#include <vector>
#include "ogg/ogg.h"
#include "vorbis/codec.h"

namespace uair {
class SoundLoaderOgg {
	// container struct that holds objects relating to vorbis decoding
	struct OggStream {
		int mVorbis = 0; // is this stream a vorbis stream? (0: undefined, -1: no, 1: yes)
		unsigned int mPacketsProcessed = 0u; // how many packets have been processed for this stream
		
		ogg_stream_state mState;
		vorbis_info mInfo;
		vorbis_comment mComment;
		vorbis_block mBlock;
		vorbis_dsp_state mDSPState;
	};
	
	public :
		void Decode(std::ifstream& oggFile, std::function<int (const int& serial, const int& numChannels, const int& numSamples,
				std::vector<int>& samples)> audioCallback); // decode an ogg file stream and handle data using callback function
	private :
};
}

#endif
