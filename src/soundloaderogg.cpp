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

#include "soundloaderogg.hpp"

#include <iostream>
#include <map>
#include <limits>

namespace uair {
void SoundLoaderOgg::Decode(std::ifstream& oggFile, std::function<int (const int& serial, const int& numChannels, const int& numSamples,
		std::vector<int>& samples)> audioCallback) {
	
	std::map<int, OggStream> streamMap; // create a map to hold all logical streams in the ogg file and their associated serial number
	long bufferSize = 8192; // the buffer size used when reading pages (4096 extra bytes padding is added by libogg)
	ogg_sync_state oggSyncState;
	ogg_page oggPage;
	ogg_packet oggPacket;
	
	ogg_sync_init(&oggSyncState); // initialise the synchronisation state for ogg decoding
	
	while (!oggFile.eof()) { // whilst the ogg file stream still has data remaining...
		while (ogg_sync_pageout(&oggSyncState, &oggPage) != 1) { // whilst current page still isn't fully synced
			char* buffer = ogg_sync_buffer(&oggSyncState, bufferSize); // return a buffer to hold page data
			if (!buffer) { // if buffer isn't valid...
				std::cout << "an error occurred, unable to get a valid buffer" << std::endl;
			}
			
			oggFile.read(buffer, bufferSize); // read from the ogg file stream into the buffer
			long extracted = static_cast<long>(oggFile.gcount()); // store the number of bytes extracted from the file stream
			if (extracted == 0) { // if no bytes were extracted...
				std::cout << "an error occurred, end of file and still incomplete page" << std::endl;
				break;
			}
			
			if (ogg_sync_wrote(&oggSyncState, extracted) != 0) { // inform libogg how many bytes were read from the ogg file stream
				std::cout << "an error occurred, possible overflow" << std::endl;
			}
		}
		
		int serial = ogg_page_serialno(&oggPage); // get the serial number of the stream the page belongs to
		if (ogg_page_bos(&oggPage) != 0) { // if the page is at the beginning of the stream...
			OggStream oggStream; // create a new stream container object
			auto result = streamMap.emplace(serial, std::move(oggStream)); // attempt to add the container object to the map
			if (result.second) { // if the container object was added successfully...
				if (ogg_stream_init(&(result.first->second.mState), serial) != 0) { // if the stream state fails to initialise properly...
					std::cout << "an error occurred, failed to initialise stream object" << std::endl;
				}
			}
		}
		
		auto stream = streamMap.find(serial); // return the stream associated with the page
		if (stream == streamMap.end()) { // if the stream wasn't found...
			std::cout << "an error occurred, stream with serial number doesn't exist" << std::endl;
			break;
		}
		
		if (ogg_stream_pagein(&(stream->second.mState), &oggPage) != 0) { // if the page failed to be added to the bitstream...
			std::cout << "an error occurred, potential serial number or version mismatch" << std::endl;
		}
		
		int packetStatus = -1; // the status of the current packet being decoded
		while ((packetStatus = ogg_stream_packetout(&(stream->second.mState), &oggPacket)) != 0) { // whilst we still have full packets to decode...
			if (packetStatus == -1) { // if the packet is out of sync...
				continue;
			}
			
			if (stream->second.mVorbis == 0) { // if the current stream type is currently undefined...
				if (vorbis_synthesis_idheader(&oggPacket) == 1) { // if the packet is a vorbis header packet...
					stream->second.mVorbis = 1; // the stream is a valid vorbis stream
					vorbis_info_init(&(stream->second.mInfo));
					vorbis_comment_init(&(stream->second.mComment));
				}
				else {
					stream->second.mVorbis = -1; // the stream is not a valid vorbis stream
				}
			}
			
			if (stream->second.mVorbis == 1) { // if the stream is a vlaid vorbis stream...
				if (stream->second.mPacketsProcessed < 3u) { // if the total packets processed is less than 3 (first 3 packets are header packets)...
					vorbis_synthesis_headerin(&(stream->second.mInfo), &(stream->second.mComment), &oggPacket); // process header packet
					++stream->second.mPacketsProcessed; // update the processed count
					
					if (stream->second.mPacketsProcessed == 3u) { // if we have now processed our 3rd packet (last of the header packets)...
						vorbis_synthesis_init(&(stream->second.mDSPState), &(stream->second.mInfo));
						vorbis_block_init(&(stream->second.mDSPState), &(stream->second.mBlock));
					}
				}
				else { // otherwise if we have already processed all header packets...
					vorbis_synthesis(&(stream->second.mBlock), &oggPacket);
					vorbis_synthesis_blockin(&(stream->second.mDSPState), &(stream->second.mBlock));
					
					float** pcm = 0;
					int numChannels = stream->second.mInfo.channels;
					int numSamples = vorbis_synthesis_pcmout(&(stream->second.mDSPState), &pcm); // generate sample data from the packet
					std::vector<int> samples;
					
					for (int sample = 0u; sample < numSamples; ++sample) {
						for (int channel = 0u; channel < numChannels; ++channel) {
							int samp = pcm[channel][sample] * std::numeric_limits<int>::max(); // convert the sample data to an int
							samples.push_back(std::move(samp));
						}
					}
					
					// pass the number of channels, samples and the pointer to the samples to our callback function for processing, and return the number of samples processed
					int samplesUsed = audioCallback(stream->first, stream->second.mInfo.channels, numSamples, samples);
					
					vorbis_synthesis_read(&(stream->second.mDSPState), samplesUsed); // indicate how many samples were processed
				}
			}
		}
	}
	
	for (auto iter = streamMap.begin(); iter != streamMap.end(); ++iter) { // for all stream container objects...
		// clean up
		vorbis_block_clear(&(iter->second.mBlock));
		vorbis_dsp_clear(&(iter->second.mDSPState));
		vorbis_comment_clear(&(iter->second.mComment));
		vorbis_info_clear(&(iter->second.mInfo));
		ogg_stream_clear(&(iter->second.mState));
	}
	
	ogg_sync_clear(&oggSyncState); // clean up the synchronisation state
}
}
