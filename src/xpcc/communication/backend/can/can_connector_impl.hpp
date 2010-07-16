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

#ifndef XPCC__CAN_CONNECTOR_HPP
	#error	"Don't include this file directly, use 'can.hpp' instead!"
#endif

#include <xpcc/math/utils/bit_operation.hpp>
#include <xpcc/driver/can/can.hpp>

// ----------------------------------------------------------------------------
template<typename Driver>
xpcc::CanConnector<Driver>::~CanConnector()
{
}

// ----------------------------------------------------------------------------
template<typename Driver>
bool
xpcc::CanConnector<Driver>::isPacketAvailable() const
{
	return !this->receivedMessages.isEmpty();
}

template<typename Driver>
const xpcc::Header&
xpcc::CanConnector<Driver>::getPacketHeader() const
{
	return this->receivedMessages.getFront().header;
}

template<typename Driver>
const xpcc::SmartPointer
xpcc::CanConnector<Driver>::getPacketPayload() const
{
	return this->receivedMessages.getFront().payload;
}

template<typename Driver>
uint8_t
xpcc::CanConnector<Driver>::getPacketPayloadSize() const
{
	return this->receivedMessages.getFront().payload.getSize();
}

// ----------------------------------------------------------------------------
template<typename Driver>
void
xpcc::CanConnector<Driver>::sendPacket(const Header &header, SmartPointer payload)
{
	bool successfull = false;
	if (payload.getSize() <= 8 && Driver::isReadyToSend()) {
		// try to send the message directly
		successfull = this->sendMessage(
				header, payload.getPointer(), payload.getSize(), false);
	}

	if (!successfull)
	{
		// append the message to the list of waiting messages
		this->sendList.append(SendListItem(header, payload));
	}
}

// ----------------------------------------------------------------------------
template<typename Driver>
void
xpcc::CanConnector<Driver>::dropPacket()
{
	this->receivedMessages.removeFront();
}

// ----------------------------------------------------------------------------
template<typename Driver>
void
xpcc::CanConnector<Driver>::update()
{
	this->checkAndReceiveMessages();
	this->sendWaitingMessages();
}

// ----------------------------------------------------------------------------
// protected
// ----------------------------------------------------------------------------

template<typename Driver>
uint32_t
xpcc::CanConnector<Driver>::convertToIdentifier(const Header & header, bool fragmentated)
{
	uint32_t identifier;
	
	switch (header.type)
	{
		case xpcc::Header::REQUEST:
			identifier = 0;
			break;
		case xpcc::Header::RESPONSE:
			identifier = 1;
			break;
		case xpcc::Header::NEGATIVE_RESPONSE:
			identifier = 2;
			break;
		default:
			identifier = 0;
	}
	
	identifier = identifier << 1;
	if (header.isAcknowledge){
		identifier |= 1;
	}
	identifier = identifier << 1;
	// Message counter
	identifier = identifier << 1;
	
	if (fragmentated){
		identifier |= 1;
	}
	identifier = identifier << 8;
	identifier |= header.destination;
	identifier = identifier << 8;
	identifier |= header.source;
	identifier = identifier << 8;
	identifier |= header.packetIdentifier;
	
	return identifier;
}

template<typename Driver>
bool
xpcc::CanConnector<Driver>::convertToHeader(
		const uint32_t & identifier, xpcc::Header & header)
{
	const uint8_t *ptr = reinterpret_cast<const uint8_t *>(&identifier);
	
	header.packetIdentifier = ptr[0];
	header.source 			= ptr[1];
	header.destination		= ptr[2];
	
	uint8_t flags = ptr[3];
	
	if (flags & 0x04) {
		header.isAcknowledge = true;
	}
	else {
		header.isAcknowledge = false;
	}
	
	switch (flags & 0x18)
	{
		case 0x00:
			header.type = xpcc::Header::REQUEST;
			break;
		case 0x08:
			header.type = xpcc::Header::RESPONSE;
			break;
		case 0x10:
			header.type = xpcc::Header::NEGATIVE_RESPONSE;
			break;
		default:
			// unknown type
			//XPCC_LOG_ERROR << "Unknown Type" << xpcc::flush;
			header.type = xpcc::Header::REQUEST;
	}
	
	// check if the message is a fragment
	return ((flags & 0x01) == 0x01);
}

template<typename Driver>
inline uint8_t
xpcc::CanConnector<Driver>::getNumberOfFragments(uint8_t messageSize)
{
	div_t n = div(messageSize, 6);
	return (n.rem > 0) ? n.quot + 1 : n.quot;
}

template<typename Driver>
bool
xpcc::CanConnector<Driver>::sendMessage(const Header &header,
		const uint8_t *data, uint8_t size, bool fragmentated)
{
	xpcc::can::Message message(
			convertToIdentifier(header, fragmentated),
			size);
	
	memcpy(message.data, data, size);
	
	return Driver::sendMessage( message );
}

template<typename Driver>
void
xpcc::CanConnector<Driver>::sendWaitingMessages()
{
	if (this->sendList.isEmpty()) {
		// no message in the queue
		return;
	}
	
	SendListItem& message = this->sendList.getFront();
	uint8_t messageSize = message.payload.getSize();
	
	bool sendFinished = true;
	if (messageSize > 8)
	{
		// fragmented message
		uint8_t data[8];
		
		data[0] = message.fragmentIndex;
		data[1] = messageSize; 	// size of the complete message
		
		uint8_t offset = message.fragmentIndex * 6;
		uint8_t fragmentSize = messageSize - offset;
		if (fragmentSize > 6)
		{
			fragmentSize = 6;
			sendFinished = false;
		}
		// otherwise fragmentSize is smaller or equal to six, so the last
		// fragment is about to be sent.

		memcpy(data + 2, message.payload.getPointer() + offset, fragmentSize);
		
		if (sendMessage(message.header, data, fragmentSize + 2, true)) {
			message.fragmentIndex++;
		}
		else {
			sendFinished = false;
		}
	}
	else
	{
		if (!this->sendMessage(
				message.header,
				message.payload.getPointer(),
				messageSize,
				false)) {
			sendFinished = false;
		}
	}
	
	if (sendFinished)
	{
		// message was the last fragment
		// => remove it from the list
		this->sendList.removeFront();
	}
}

template<typename Driver>
bool
xpcc::CanConnector<Driver>::retrieveMessage()
{
	can::Message message;
	if (Driver::getMessage(message))
	{
		xpcc::Header header;
		bool isFragment = convertToHeader(message.identifier, header);
		
		if (!isFragment)
		{
			this->receivedMessages.append(ReceiveListItem(message.length, header));
			std::memcpy(this->receivedMessages.getBack().payload.getPointer(),
					message.data, message.length);
		}
		else
		{
			// find existing container otherwise create a new one
			const uint8_t fragmentIndex = message.data[0];
			const uint8_t messageSize = message.data[1];
			
			// calculate the number of messages need to send messageSize-bytes
			uint8_t numberOfFragments = this->getNumberOfFragments(messageSize);
			
			if (message.length < 4 || messageSize > 48 || fragmentIndex >= numberOfFragments)
			{
				// illegal format: fragmented messages need to have at least 3 byte payload,
				// 				   the maximum size is 48 Bytes and the fragment number should
				//				   not be higher than the number of fragments.
				return false;
			}

			// check the length of the fragment (all fragments except the last one
			// need to have a payload-length of 6 bytes + 2 byte fragment information)
			uint8_t offset = fragmentIndex * 6;
			if (fragmentIndex + 1 == numberOfFragments)
			{
				// this one is the last fragment
				if (messageSize - offset != message.length - 3)
				{
					// illegal format
					return false;
				}
			}
			else if (message.length != 8)
			{
				// illegal format
				return false;
			}
			
			// Check if other parts of this message are already in the
			// list of receiving messages.
			typename ReceiveList::iterator packet = this->receivingMessages.begin();
			for ( ; packet != this->receivingMessages.end(); ++packet) {
				if (packet->header == header) {
					break;
				}
			}
			
			if (packet == this->receivingMessages.end()) {
				// message not found => first part of this message,
				// prepend it to the list
				this->receivingMessages.prepend(ReceiveListItem(messageSize, header));
				packet = this->receivingMessages.begin();
			}
			
			// create a marker for the currently received fragment and
			// test if the fragment was already received
			const uint8_t currentFragment = (1 << fragmentIndex);
			if (currentFragment & packet->receivedFragments)
			{
				// error: received fragment twice -> most likely a new message -> delete the old one
				//XPCC_LOG_WARNING << "lost fragment" << xpcc::flush;
				packet->receivedFragments = 0;
			}
			packet->receivedFragments |= currentFragment;
			
			std::memcpy(packet->payload.getPointer() + offset,
					message.data + 2,
					message.length - 2);
			
			// test if this was the last segment, otherwise we have to wait for more messages
			if (xpcc::math::bitCount(packet->receivedFragments) == numberOfFragments)
			{
				this->receivedMessages.append(*packet);
				this->receivingMessages.remove(packet);
			}
		}
		
		return true;
	}
	else {
		return false;
	}
}

template<typename Driver>
void
xpcc::CanConnector<Driver>::checkAndReceiveMessages()
{
	while (Driver::isMessageAvailable()) {
		this->retrieveMessage();
	}
}
