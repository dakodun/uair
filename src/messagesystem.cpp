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

#include "messagesystem.hpp"

namespace uair {
MessageS::~MessageS() {
	
}


void MessageSystem::PushMessageString(const unsigned int& systemTypeID, const unsigned int& messageTypeID, const std::string& messageString) {
	// add an entry to the queue using the message's unique type id and the serialised string
	mMessageQueue.emplace_back(systemTypeID, messageTypeID, messageString);
}

unsigned int MessageSystem::GetMessageCount() {
	return mMessageQueue.size();
}

unsigned int MessageSystem::GetSystemType(const unsigned int& index) {
	try {
		return mMessageQueue.at(index).mSystemTypeID;
	} catch (std::exception& e) {
		throw;
	}
}

unsigned int MessageSystem::GetMessageType(const unsigned int& index) {
	try {
		return mMessageQueue.at(index).mMessageTypeID;
	} catch (std::exception& e) {
		throw;
	}
}

int MessageSystem::GetMessageState(const unsigned int& index) {
	try {
		return mMessageQueue.at(index).mMessageStatus;
	} catch (std::exception& e) {
		throw;
	}
}

void MessageSystem::PopMessage(const unsigned int& index) {
	if (index < mMessageQueue.size()) { // if index supplied is valid...
		mMessageQueue.erase(mMessageQueue.begin() + index); // remove the message from the queue
	}
}
}
