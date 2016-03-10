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

#ifndef UAIRMESSAGESYSTEM_HPP
#define UAIRMESSAGESYSTEM_HPP

#include <sstream>

#define CEREAL_SERIALIZE_FUNCTION_NAME Serialise
#include "cereal/archives/binary.hpp"

namespace uair {
// base class for all messages allowing interaction with a message system
class Message {
	public :
		virtual ~Message();
		
		// derived message must implement this function and return a unique id (unique amongst other derived message)
		virtual unsigned int GetTypeID() const = 0;
		
		// allow derived classes to be serialised
		virtual void Serialise(cereal::BinaryOutputArchive& archive) = 0;
		
		// allow derived classes to be deserialised
		virtual void Serialise(cereal::BinaryInputArchive& archive) = 0;
};


// class to allow messages to be passed between systems
class MessageSystem {
	friend class Game;
	
	public :
		enum MessageState : int {
			Read = 1,
			Input = 2,
			Process = 4,
			PostProcess = 8,
			Render = 16,
			All = 30
		};
		
		// an entry into the message queue containing the unique system id that added it, a unique id for the message type and the serialised message data
		struct QueueEntry {
			QueueEntry(const unsigned int& systemTypeID, const unsigned int& messageTypeID, const std::string& messageString) :
					mSystemTypeID(systemTypeID), mMessageTypeID(messageTypeID), mMessageString(messageString) {
				
				
			}
			
			unsigned int mSystemTypeID;
			unsigned int mMessageTypeID;
			std::string mMessageString;
			int mMessageStatus = 0;
		};
	public :
		// serialise and push a message into the end of the queue
		template <class T>
		void PushMessage(const unsigned int& systemTypeID, const T& messageIn);
		
		// push an already serialised message into the end of the queue
		void PushMessageString(const unsigned int& systemTypeID, const unsigned int& messageTypeID, const std::string& messageString);
		
		// return number of messages in the queue
		unsigned int GetMessageCount();
		
		// return the unique id of system that added the message at position index
		unsigned int GetSystemType(const unsigned int& index);
		
		// return the unique id of message at position index
		unsigned int GetMessageType(const unsigned int& index);
		
		// return the current state of message at position index
		int GetMessageState(const unsigned int& index);
		
		// deserialise and return the message object at position index (without modifying its read flag)
		template <class T>
		T PeekMessage(const unsigned int& index);
		
		// deserialise and return the message object at position index
		template <class T>
		T GetMessage(const unsigned int& index);
		
		// remove the message at position index
		void PopMessage(const unsigned int& index);
	private :
		std::vector<QueueEntry> mMessageQueue;
};

template <class T>
void MessageSystem::PushMessage(const unsigned int& systemTypeID, const T& messageIn) {
	if (!std::is_base_of<Message, T>::value) { // if the message being pushed into the queue doesn't inherit from base class...
		throw std::runtime_error("not of base type"); // an error has occurred, don't push the message
	}
	
	std::stringstream strStream; // the stringstream used to serialise the message being pushed
	
	{
		// create an output archive using cereal and our stringstream and serialise the message
		cereal::BinaryOutputArchive archiveOutBinary(strStream);
		archiveOutBinary(messageIn);
	} // flush the archive on destruction
	
	// add an entry to the queue using the message's unique type id and the serialised string
	mMessageQueue.emplace_back(systemTypeID, messageIn.GetTypeID(), strStream.str());
}

template <class T>
T MessageSystem::PeekMessage(const unsigned int& index) {
	const QueueEntry* queueEntryPtr;
	try {
		queueEntryPtr = &(mMessageQueue.at(index));
	} catch (std::exception& e) {
		throw;
	}
	
	T messageOut;
	if (messageOut.GetTypeID() != queueEntryPtr->mMessageTypeID) {
		throw std::runtime_error("message type mismatch");
	}
	
	// the stringstream used to deserialise the message being pushed, initialised with the stored serialised string
	std::stringstream strStream(queueEntryPtr->mMessageString);
	
	{
		// create an input archive using cereal and our stringstream and deserialise the message
		cereal::BinaryInputArchive archiveInBinary(strStream);
		archiveInBinary(messageOut);
	} // flush the archive on destruction
	
	return messageOut;
}

template <class T>
T MessageSystem::GetMessage(const unsigned int& index) {
	try {
		T retVal = PeekMessage<T>(index);
		
		QueueEntry* queueEntryPtr = &(mMessageQueue.at(index));
		queueEntryPtr->mMessageStatus |= MessageState::Read;
		
		return retVal;
	} catch (std::exception& e) {
		throw;
	}
}


}

#endif
