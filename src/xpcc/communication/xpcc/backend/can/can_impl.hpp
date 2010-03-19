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
#ifndef XPCC_CAN_HPP
	#error	"Don't include this file directly, use 'can.hpp' instead!"
#endif

#ifndef	XPCC_CAN_IMPL_HPP
#define	XPCC_CAN_IMPL_HPP

#include "can.hpp"
template<typename C>
xpcc::CanInterface::CanInterface() :
sendList(0), receivingMessages(0), receivedMessages(0) {
	
}
template<typename C>
xpcc::CanInterface::~CanInterface()
{

}

template<typename C>
void
xpcc::CanInterface::sendPacket(const Header &header, SmartPointer payload)
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
		if (sendList == 0) {
			sendList = message;
		}
		else {
			SendListItem *node = sendList;
			while (node->next != 0) {
				node = node->next;
			}
			node->next = message;
		}
	}
}

template<typename C>
void
xpcc::CanInterface::dropPacket()
{
	ReceiveListItem *temp = receivedMessages;
	receivedMessages = receivedMessages->next;
	delete temp;
}

template<typename C>
void
xpcc::CanInterface::update() {
	checkAndReceiveMessages();
	sendWaitingMessages();
}

template<typename C>
void
xpcc::CanInterface::sendWaitingMessages()
{
	if (sendList == 0) {
		// no message in the queue
		return;
	}
	
	uint8_t size = sendList->payload.getSize();
	bool sendFinished = true;
	
	if (size > 8)
	{
		// fragmented message
		uint8_t data[8];
		
		data[0] = sendList->fragmentIndex;
		data[1] = size;
		
		if (size > 6) {
			size = 6;
			sendFinished = false;
		}
		
		memcpy(data + 2, sendList->payload.getPointer(), size);
		
		if (sendCanMessage(sendList->header, data, size)) {
			sendList->fragmentIndex++;
		}
		else {
			sendFinished = false;
		}
	}
	else
	{
		if (!sendCanMessage(sendList->header, sendList->payload.getPointer(), size)) {
			sendFinished = false;
		}
	}
	
	if (sendFinished)
	{
		// message was the last fragment
		// => remove it from the list
		SendListItem *node = sendList;
		sendList = sendList->next;
		delete node;
	}
}

template<typename C>
void
xpcc::CanInterface::checkAndReceiveMessages()
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
xpcc::CanInterface::readMessage()

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

#endif /* XPCC_CAN_IMPL_HPP */
