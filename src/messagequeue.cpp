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

#include "messagequeue.hpp"

namespace uair {
Message::~Message() {
	
}


MessageQueue& MessageQueue::operator+=(MessageQueue other) {
	while (!other.mQueue.empty()) { // while the copy of the other queue still has entries...
		mQueue.push(other.mQueue.front()); // add the entry to the end of this queue
		other.mQueue.pop(); // remove the entry from the copy of the other queue
	}
	
	return *this;
}

void MessageQueue::PushMessageString(const unsigned int& messageTypeID, const std::string& messageString) {
	// add an entry to the queue using the message's unique type id and the serialised string
	mQueue.emplace(messageTypeID, messageString);
}

MessageQueue::Entry MessageQueue::GetMessage() const {
	return mQueue.front(); // return the entry at the front of the queue
}

void MessageQueue::PopMessage() {
	mQueue.pop(); // remove the message from the front of the queue
}

void MessageQueue::ClearQueue() {
	std::queue<Entry> newQueue; // create a new empty queue
	mQueue = std::move(newQueue); // overwrite the existing queue
}

bool MessageQueue::IsEmpty() const {
	return mQueue.empty();
}

unsigned int MessageQueue::GetMessageCount() const {
	return mQueue.size();
}
}
