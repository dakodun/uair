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

#include "sound.hpp"

#include "util.hpp"

namespace uair {
Sound::Sound() : mLoops(0) {
	
}

Sound::Sound(const Sound& other) : mLoops(other.mLoops.load()), mSampleRate(other.mSampleRate), mBufferFrames(other.mBufferFrames), mNumChannels(other.mNumChannels),
		mSamplePosition(other.mSamplePosition), mSampleDataPtr(other.mSampleDataPtr), mSoundBuffer(other.mSoundBuffer) {
	
	if (other.mRtAudioSetup) {
		OpenRtAudioStream(mNumChannels);
	}
}

Sound::Sound(Sound&& other) : Sound() {
	swap(*this, other);
}

Sound::~Sound() {
	CloseRtAudioStream(); // close the rtaudio stream
}

Sound& Sound::operator=(Sound other) {
	swap(*this, other);
	
	return *this;
}

void swap(Sound& first, Sound& second) {
	using std::swap;
	
	second.mLoops.store(first.mLoops.exchange(second.mLoops.load())); // swap the atomic loop counts
	
	// store the state of the sound's rtaudio stream
	bool firstSetUp = first.mRtAudioSetup;
	bool secondSetUp = second.mRtAudioSetup;
	
	// ensure both sound's rtaudio streams are closed
	first.CloseRtAudioStream();
	second.CloseRtAudioStream();
	
	swap(first.mSampleRate, second.mSampleRate);
	swap(first.mBufferFrames, second.mBufferFrames);
	swap(first.mNumChannels, second.mNumChannels);
	
	swap(first.mSamplePosition, second.mSamplePosition);
	swap(first.mSampleDataPtr, second.mSampleDataPtr);
	swap(first.mSoundBuffer, second.mSoundBuffer);
	
	if (secondSetUp) { // if the second sound's rtaudio stream was initialised...
		first.OpenRtAudioStream(first.mNumChannels); // initialise the first sound's rtaudio stream
	}
	
	if (firstSetUp) {
		second.OpenRtAudioStream(second.mNumChannels);
	}
}

void Sound::FromBuffer(ResourcePtr<SoundBuffer> buffer) {
	Stop(); // stop the audio playback
	CloseRtAudioStream(); // close the old rtaudio stream
	mSampleDataPtr = nullptr; // unset the pointer to the samples array
	
	mSoundBuffer = buffer; // set the new buffer resource pointer
	mSampleDataPtr = &(mSoundBuffer->mSampleData); // set the pointer to the new samples array
	mNumChannels = mSoundBuffer->mNumChannels; // update the number of channels
	OpenRtAudioStream(mNumChannels); // open the new rtaudio stream [!] params
}

void Sound::FromBuffer(SoundBuffer* buffer) {
	FromBuffer(ResourcePtr<SoundBuffer>(buffer));
}

bool Sound::Play() {
	if (mRtAudio.isStreamRunning()) { // if sound is playing...
		Stop(); // stop the sound
	}
	
	if (mRtAudio.isStreamOpen()) { // if sound is open...
		mRtAudio.startStream(); // play the sound
		return true;
	}
	
	return false;
}

bool Sound::Pause() {
	if (mRtAudio.isStreamRunning()) { // if sound is playing...
		mRtAudio.stopStream(); // pause the sound
		return true;
	}
	else if (mRtAudio.isStreamOpen()) { // otherwise if sound is open...
		mRtAudio.startStream(); // play the sound
		return true;
	}
	
	return false;
}

bool Sound::Stop() {
	if (mRtAudio.isStreamRunning()) { // if sound is playing...
		mRtAudio.stopStream(); // stop the sound
		mSamplePosition = 0u; // seek to beginning
		
		return true;
	}
	
	return false;
}

bool Sound::IsPlaying() {
	return mRtAudio.isStreamRunning() || mRtAudio.isStreamStopping(); // return whether the stream is running or not
}

int Sound::GetPlaybackTime() {
	return (((mSamplePosition / static_cast<float>(mNumChannels)) / mSampleRate) * 1000.0f); // return the playback time in milliseconds
}

void Sound::SetPlaybackTime(const unsigned int& time) {
	bool wasPlaying = Stop(); // stop any current playback
	mSamplePosition = ((static_cast<float>(time) / 1000) * mSampleRate) * mNumChannels; // calculate the position in samples using the sample rate and channel count
	mSamplePosition -= mSamplePosition % mNumChannels; // offset the playback position to ensure channels are in the correct order
	
	if (wasPlaying) { // if the sound was playing before...
		Play(); // restart the sound from the new position
	}
}

void Sound::OpenRtAudioStream(const int& numChannels) {
	if (!mRtAudioSetup) { // if rtaudio isn't set up yet...
		if (mRtAudio.getDeviceCount() > 0) { // if there is at least one device capable of playback...
			mRtAudioParams.deviceId = mRtAudio.getDefaultOutputDevice(); // get the id of the default playback device
			mRtAudioParams.nChannels = numChannels; // get the number of channels
			mRtAudioParams.firstChannel = 0; // set the index of the first channel to 0
			mRtAudioSetup = true;
			
			try {
				using namespace std::placeholders;
				mRtAudio.openStream(&mRtAudioParams, 0, RTAUDIO_SINT32, mSampleRate, &mBufferFrames,
						std::bind(&Sound::AudioCallback, this, _1, _2, _3, _4, _5, _6),
						(void*)&mSamplePosition);
			} catch (RtAudioError& e) {
				e.printMessage();
				mRtAudioSetup = false;
			}
		}
	}
}

void Sound::CloseRtAudioStream() {
	if (mRtAudioSetup) { // if rtaudio is set up...
		Stop();
		
		if (mRtAudio.isStreamOpen()) { // if there is an open stream...
			mRtAudio.closeStream(); // close the stream
		}
		
		mRtAudioSetup = false;
	}
}

int Sound::AudioCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void* userData) {
	int* output = (int*) outputBuffer;
	unsigned int* samplePosition = (unsigned int*) userData;
	
	unsigned int samplesAdded = 0u; // the total count of samples added to the output buffer
	if (mSampleDataPtr && !mSampleDataPtr->empty()) { // if sample data exists...
		unsigned int sampleTotal = mSampleDataPtr->size(); // the size of the sample array
		unsigned int sampleCount = nBufferFrames * mNumChannels; // the number of samples required in the buffer
		
		while (samplesAdded != sampleCount) { // whilst we still haven't added enough samples...
			unsigned int samplesAddedLoop = 0; // the number of samples added in the current loop
			
			// for all required samples or all samples remaining in the array...
			for (unsigned int i = *samplePosition, j = 0u; i < sampleTotal && j < (sampleCount - samplesAdded); i += mNumChannels, j += mNumChannels) {
				for (unsigned int k = 0u; k < mNumChannels; ++k) { // for all channels...
					*output++ = mSampleDataPtr->at(i + k); // add the current sample to the output buffer
				}
				
				samplesAddedLoop += mNumChannels; // increment the count of samples added this loop
			}
			
			*samplePosition += samplesAddedLoop; // increment the sample offset position by the number of samples added this loop
			samplesAdded += samplesAddedLoop; // increment the total number of samples added
			if (samplesAdded != sampleCount && mLoops != 0) { // if we've not reached the required number of samples and should loop...
				*samplePosition = 0; // reset the playback position to the beginning
				
				if (mLoops > 0) { // if the loop count is positive...
					--mLoops; // decrement the loop counter
				}
			}
			else { // otherwise if we've reached our limit or have no loops left...
				break; // stop processing samples
			}
		}
	}
	
	if (samplesAdded == 0u) { // if no samples were added...
		mLoops = 0; // clear any remaining loops
		return 1; // playback is done
	}
	else {
		return 0;
	}
}
}
