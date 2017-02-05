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

#ifndef UAIRMESSAGEQUEUE_HPP
#define UAIRMESSAGEQUEUE_HPP

#include <sstream>
#include <queue>

#define CEREAL_SERIALIZE_FUNCTION_NAME Serialise
#include "cereal/archives/binary.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/string.hpp"

#include "init.hpp" // on windows, macro will rename GetMessage to GetMessageW so just go with it

namespace uair {
// base class for all messages allowing interaction with a message system
// derived classes should implement a static function that returns an unsigned integer (representing a unique typeid)
// (static unsigned int GetTypeID()) user implemented typeids should start at 1000
class EXPORTDLL Message {
	public :
		virtual ~Message();
		
		// allow derived classes to be serialised
		virtual void Serialise(cereal::BinaryOutputArchive& archive) const = 0;
		
		// allow derived classes to be deserialised
		virtual void Serialise(cereal::BinaryInputArchive& archive) = 0;
};


// 
class EXPORTDLL MessageQueue {
	public :
		// an entry into the message queue containing the unique id for the message type and the serialised message data
		// as well as a helper function to allow easier deserialistion
		class Entry {
			public :
				Entry(const unsigned int& messageTypeID, const std::string& messageString) :
						mMessageTypeID(messageTypeID), mMessageString(messageString) {
					
				}
				
				// deserialise this entry into a message of the requested type
				template <typename T, typename ...Ps>
				T Deserialise(const Ps&... params) const {
					if (T::GetTypeID() != mMessageTypeID) { // if the type requested doesn't match the type stored in this entry...
						throw std::runtime_error("message type mismatch"); // stop the deserialistion process
					}
					
					T messageOut(params...); // create a new message object
					std::stringstream strStream(mMessageString); // create a new stream containing the serialised message data
					
					{
						// create an input archive using cereal and our stringstream and deserialise the message
						cereal::BinaryInputArchive archiveInBinary(strStream);
						archiveInBinary(messageOut);
					} // flush the archive on destruction
					
					return messageOut; // return the new message deserialised object
				}
				
				// return the unique type id of the message stored in this entry
				unsigned int GetTypeID() const {
					return mMessageTypeID;
				}
				
				// return the serialised string stored in this entry
				std::string GetString() const {
					return mMessageString;
				}
			
			private :
				unsigned int mMessageTypeID; // the unique type id of the message stored in this entry
				std::string mMessageString; // the serialised string stored in this entry
		};
	public :
		// allow other messagequeues to be appended to the end of this one
		MessageQueue& operator+=(MessageQueue other);
		
		// serialise and push a message into the end of the queue
		template <class T>
		void PushMessage(const T& messageIn);
		
		// push an already serialised message into the end of the queue
		void PushMessageString(const unsigned int& messageTypeID, const std::string& messageString);
		
		// return the entry at the front of the queue
		Entry GetMessage() const;
		
		// remove the entry at the front of the queue
		void PopMessage();
		
		// remove all entries in the queue
		void ClearQueue();
		
		// return true if the queue has no entries
		bool IsEmpty() const;
		
		// return number of entries in the queue
		unsigned int GetMessageCount() const;
	
	private :
		std::queue<Entry> mQueue;
};

template <class T>
void MessageQueue::PushMessage(const T& messageIn) {
	if (!std::is_base_of<Message, T>::value) { // if the message being pushed into the queue doesn't inherit from base class...
		throw std::runtime_error("not of base type"); // an error has occurred, don't push the message
	}
	
	std::stringstream strStream;
	
	{
		// create an output archive using cereal and our stringstream and serialise the message
		cereal::BinaryOutputArchive archiveOutBinary(strStream);
		archiveOutBinary(messageIn);
	} // flush the archive on destruction
	
	mQueue.emplace(messageIn.GetTypeID(), strStream.str()); // add a new entry into the queue
}
}

#endif
