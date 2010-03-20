// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id$
 */
// ----------------------------------------------------------------------------
#ifndef XPCC_CAN_CONNECTOR_HPP
	#error	"Don't include this file directly, use 'can.hpp' instead!"
#endif

#include "can.hpp"
#include <xpcc/driver/can/can.hpp>

#include <xpcc/architecture/general/time/delay.hpp>
#include <xpcc/debug/logger/logger.hpp>

// set the Loglevel
#undef  XPCC_LOG_LEVEL
#define XPCC_LOG_LEVEL xpcc::log::DEBUG

template<typename C>
xpcc::CanConnector<C>::CanConnector() :
sendList(0), receivingMessages(0), receivedMessages(0) {
	
}

// -----------------------------------------------------------------------------
template<typename C>
xpcc::CanConnector<C>::~CanConnector()
{

}

// -----------------------------------------------------------------------------
template<typename C>
void
xpcc::CanConnector<C>::sendPacket(const Header &header, SmartPointer payload)
{
	if (0)
	{
		// TODO: check if the send buffer is empty, so that we can send the
		// message directly without putting it to the list
	}
	else
	{
		SendListItem *message = new SendListItem(header, payload);
		
		// append message to the list of pending messages
		if (this->sendList == 0) {
			// empty list => first element
			this->sendList = message;
		}
		else {
			// find the last element in the list and append
			SendListItem *node = this->sendList;
			while (node->next != 0) {
				node = node->next;
			}
			node->next = message;
		}
	}
}

// -----------------------------------------------------------------------------
template<typename C>
void
xpcc::CanConnector<C>::dropPacket()
{
	ReceiveListItem *temp = receivedMessages;
	receivedMessages = receivedMessages->next;
	delete temp;
}

// -----------------------------------------------------------------------------
template<typename C>
void
xpcc::CanConnector<C>::update() {
	this->checkAndReceiveMessages();
	this->sendWaitingMessages();
}

// -----------------------------------------------------------------------------
// protected
// -----------------------------------------------------------------------------
template<typename C>
bool
xpcc::CanConnector<C>::sendCanMessage(const Header &header, const uint8_t *data, uint8_t size)
{
	xpcc::Can::Message message(
			0,
			size);

	switch ( header.type ) {
		case xpcc::Header::REQUEST :
			message.identifier = 0;
			break;
		case xpcc::Header::RESPONSE :
			message.identifier = 1;
			break;
		case xpcc::Header::NEGATIVE_RESPONSE :
			message.identifier = 2;
			break;
	}
	message.identifier = message.identifier << 1;

	if( header.isAcknowledge ) {
		message.identifier += 1;
	}
	message.identifier = message.identifier << 1;

	// todo counter
	message.identifier = message.identifier << 8;

	message.identifier += header.destination;
	message.identifier = message.identifier << 8;
	message.identifier += header.source;
	message.identifier = message.identifier << 8;
	message.identifier += header.packetIdentifier;

	memcpy(message.data, data, size);

	return C::sendMessage( message );
}

template<typename C>
void
xpcc::CanConnector<C>::sendWaitingMessages()
{
	if (this->sendList == 0) {
		// no message in the queue
		return;
	}
	
	uint8_t size = this->sendList->payload.getSize();
	bool sendFinished = true;
	
	if (size > 8)
	{
		// fragmented message
		uint8_t data[8];
		
		data[0] = this->sendList->fragmentIndex;
		data[1] = size; // complete message
		
		uint8_t offset = this->sendList->fragmentIndex * 6;
		uint8_t sizeFragment = size - offset;
		if (sizeFragment > 6) {
			sizeFragment = 6;
			sendFinished = false;
		}

		memcpy(data + 2, this->sendList->payload.getPointer()+offset, sizeFragment);

		if (sendCanMessage(this->sendList->header, data, sizeFragment)) {
			this->sendList->fragmentIndex++;
		}
		else {
			sendFinished = false;
		}
	}
	else
	{
		if (!this->sendCanMessage(this->sendList->header, this->sendList->payload.getPointer(), size)) {
			sendFinished = false;
		}
	}
	
	if (sendFinished)
	{
		// message was the last fragment
		// => remove it from the list
		SendListItem *node = sendList;
		this->sendList = this->sendList->next;
		delete node;
	}
}

template<typename C>
bool
xpcc::CanConnector<C>::isCanMessageAvailable() const
{
	return false;
}

template<typename C>
bool
xpcc::CanConnector<C>::retrieveCanMessage()
{
	return false;
}

template<typename C>
uint32_t
xpcc::CanConnector<C>::getCanIdentifier()
{
	return 0;
}

template<typename C>
uint8_t
xpcc::CanConnector<C>::getCanData(uint8_t *data)
{
	(void) data;

	return 0;
}

template<typename C>
void
xpcc::CanConnector<C>::checkAndReceiveMessages()
{
	while (isCanMessageAvailable())
	{
		if (retrieveCanMessage()) {
			readMessage();
		}
	}
}

template<typename C>
void
xpcc::CanConnector<C>::readMessage()

{
	//uint32_t identifier = getCanIdentifier();
	//uint8_t size = getCanSize();
	
	if (receivingMessages == 0)
	{
		//receivingMessages = 
	}
	else {
		/*ReceiveListItem *node = receivingMessages;
		do {
			if (header == node->header &&
				size == node->size)
			{
				
			}
			node = node->next;
		} while (node);
		*/
	}
}
