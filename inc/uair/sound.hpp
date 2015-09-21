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

#ifndef UAIRSOUND_HPP
#define UAIRSOUND_HPP

#include "soundbuffer.hpp"

namespace uair {
class Sound {
	public :
		Sound();
		Sound(const Sound& other);
		Sound(Sound&& other);
		~Sound();
		
		Sound& operator=(Sound other);
		
		friend void swap(Sound& first, Sound& second);
		
		void FromBuffer(ResourcePtr<SoundBuffer> buffer);
		void FromBuffer(SoundBuffer* buffer);
		
		bool Play();
		bool Pause();
		bool Stop();
		
		bool IsPlaying();
		int GetPlaybackTime();
		void SetPlaybackTime(const unsigned int& time);
	private :
		void OpenRtAudioStream(const int& numChannels);
		void CloseRtAudioStream();
		
		int AudioCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void* userData);
	public :
		std::atomic<int> mLoops; // the number of times the sound should loop (negative values imply infinite loop)
	private :
		bool mRtAudioSetup = false; // indication whether rtaudio has been initialised or not
		RtAudio mRtAudio; // an rtaudio instance used to interface with the sound hardware
		RtAudio::StreamParameters mRtAudioParams; // paramaters to set up an rtaudio instance
		unsigned int mSampleRate = 44100u; // the sample rate of playback
		unsigned int mBufferFrames = 256u; // the number of frames to buffer for playback
		unsigned int mNumChannels = 1u; // the number of playback channels
		
		unsigned int mSamplePosition = 0u; // the current playback offset in the samples array
		std::vector<int>* mSampleDataPtr = nullptr; // a pointer to the samples in the current resource
		uair::ResourcePtr<SoundBuffer> mSoundBuffer; // a resource pointer to a sound buffer where sample data comes from
};
}

#endif
