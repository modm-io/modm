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

#include "can.hpp"
#include <xpcc/driver/can/can.hpp>

//#include <xpcc/debug/logger/logger.hpp>
#include <xpcc/utils/misc.hpp>

#define XPCC_CAN_USE_COUNTER 0

// set the Loglevel
//#undef  XPCC_LOG_LEVEL
//#define XPCC_LOG_LEVEL xpcc::log::INFO

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
	bool sendSuccessfull = false;

#if XPCC_CAN_USE_COUNTER
	if ( payload.getSize() <= 7 && C::canSend()  )
#else
	if ( payload.getSize() <= 7 && C::canSend()  )
//	if ( payload.getSize() <= 8 && C::canSend()  )
#endif
	{
		sendSuccessfull = this->sendCanMessage(	header, payload.getPointer(), payload.getSize(), false );
	}

	if( !sendSuccessfull )
	{
		SendListItem* message = new SendListItem(header, payload);

		this->sendList.append( new typename SendList::Node( message ) );
	}
}

// -----------------------------------------------------------------------------
template<typename C>
void
xpcc::CanConnector<C>::dropPacket()
{
	typename ReceiveList::Node *temp = this->receivedMessages.front();
	this->receivedMessages.remove( temp );
	delete temp->getValue();
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
xpcc::CanConnector<C>::sendCanMessage(const Header &header, const uint8_t *data, uint8_t size, bool fragmentated)
{
	static uint8_t messageCounter(0);
	messageCounter++;

	xpcc::Can::Message message(
			0,
			size+1);
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
		message.identifier |= 1;
	}
	message.identifier = message.identifier << 1;

	// todo counter
#if XPCC_CAN_USE_COUNTER
#error "use of counter is not implemented yet"
#endif
	message.identifier = message.identifier << 1;

	if( fragmentated ) {
		message.identifier |= 1;
	}
	message.identifier = message.identifier << 8;

	message.identifier |= header.destination;
	message.identifier = message.identifier << 8;
	message.identifier |= header.source;
	message.identifier = message.identifier << 8;
	message.identifier |= header.packetIdentifier;

	message.data[0] = messageCounter;
	memcpy(message.data + 1, data, size);

	message.flags.extended = true;
//	message.flags.rtr = false;
	return C::sendMessage( message );
}

template<typename C>
void
xpcc::CanConnector<C>::sendWaitingMessages()
{
	if ( this->sendList.isEmpty() ) {
		// no message in the queue
		return;
	}
	
	SendListItem* message = this->sendList.front()->getValue();
	uint8_t size = message->payload.getSize();
	bool sendFinished = true;
	
	if (size > 7)
	{
		// fragmented message
		uint8_t data[7];
		
		data[0] = message->fragmentIndex;
		data[1] = size; // complete message
		
		uint8_t offset = message->fragmentIndex * 5;
		uint8_t sizeFragment = size - offset;
		if (sizeFragment > 5) {
			sizeFragment = 5;
			sendFinished = false;
		}
		// else sizeFragment is 5 or smaller, i.a. the last fragment is to be
		// sent.
		// So sendFinished

		memcpy(data + 2, message->payload.getPointer() + offset, sizeFragment);

		if (sendCanMessage(message->header, data, sizeFragment+2, true)) {
			message->fragmentIndex++;
		}
		else {
			sendFinished = false;
		}
	}
	else
	{
		if (!this->sendCanMessage(message->header, message->payload.getPointer(), size, false)) {
			sendFinished = false;
		}
	}
	
	if (sendFinished)
	{
		// message was the last fragment
		// => remove it from the list
		typename SendList::Node *temp = this->sendList.front();
		this->sendList.remove( temp );
		delete temp->getValue();
		delete temp;
	}
}

template<typename C>
bool
xpcc::CanConnector<C>::isCanMessageAvailable() const
{
	return C::isMessageAvailable();
}

template<typename C>
bool
xpcc::CanConnector<C>::retrieveCanMessage()
{
	Can::Message canMessage;

	if( C::getMessage(canMessage) ) {

		xpcc::Header header;
		header.packetIdentifier = 0x000000FF & canMessage.identifier;
		canMessage.identifier	= canMessage.identifier >> 8;
		header.source 			= 0x000000FF & canMessage.identifier;
		canMessage.identifier	= canMessage.identifier >> 8;
		header.destination 		= 0x000000FF & canMessage.identifier;
		canMessage.identifier 	= canMessage.identifier >> 8;

		bool fragmentated( false );
		if ( canMessage.identifier & 0x01 ) {
			fragmentated = true;
		}
		canMessage.identifier = canMessage.identifier >> 1;

		// todo counter
#if XPCC_CAN_USE_COUNTER
#error "use of counter is not implemented yet"
#endif
		canMessage.identifier = canMessage.identifier >> 1;

		if( canMessage.identifier & 0x01 ) {
			header.isAcknowledge = true;
		}
		canMessage.identifier = canMessage.identifier >> 1;

		switch (canMessage.identifier & 0x03 ) {
			case 0:
				header.type = xpcc::Header::REQUEST;
				break;
			case 1:
				header.type = xpcc::Header::RESPONSE;
				break;
			case 2:
				header.type = xpcc::Header::NEGATIVE_RESPONSE;
				break;
			default:
				// unknown type
				return false;
		}
		
//		if (header.destination == 0x12)
//			XPCC_LOG_INFO << "H" << xpcc::endl;
//
		if( fragmentated ) {
			// find existing container otherwise create a new one
			const uint8_t& messageSize = canMessage.data[2];
			const uint8_t& fragmentId = canMessage.data[1];

			// calculate the number of messages need to send message_size-bytes
			div_t n = div(messageSize, 5);
			uint8_t numberOfFragments = (n.rem > 0) ? n.quot + 1 : n.quot;
			// TODO: number_of_message = (current_message.size - 1) / 5 + 1;

			if (canMessage.length < 4 || messageSize > 40 || fragmentId >= numberOfFragments)
			{
				// illegal format: fragmented messages need to have at least 3 byte payload,
				// 				   the maxium size is 48 Bytes and the fragment number should
				//				   not be heigher than the number of fragments.
				return false;
			}

			// check the length of the fragment (all fragments except the last one
			// need to have a payload-length of 6 bytes + 2 byte fragment information)
			uint8_t offset = fragmentId * 5;
			if (fragmentId + 1 == numberOfFragments)
			{
				// this one is the last fragment
				if (messageSize - offset != canMessage.length - 3)
				{
					// illegal format
					return false;
				}
			}
			else if (canMessage.length != 8)
			{
				// illegal format
				return false;
			}

			// create a marker for the currently received fragment
			uint8_t currentFragment = (1 << fragmentId);

			typename ReceiveList::Node* node(0);
			if ( !this->receivingMessages.isEmpty() ) {
				// find the last element in the list and append
				node = this->receivingMessages.front();
				while (node != 0) {
					if( node->getValue()->header == header ) {
						break;
					}

					node = ++(*node);
				}
			}
			if( node == 0 ) {
				//xpcc::log::debug << "new container" << xpcc::flush;
				ReceiveListItem* message = new ReceiveListItem( messageSize, header );
				node = new typename ReceiveList::Node( message );
				this->receivingMessages.append( node );
			}

			// test if the fragment was already received
			if (currentFragment & node->getValue()->receivedFragments)
			{
				// error: received fragment twice -> most likely a new message -> delete the old one
				xpcc::log::warning << "lost fragment" << xpcc::flush;
				node->getValue()->receivedFragments = 0;
			}

			memcpy( node->getValue()->data.getPointer()+offset, canMessage.data+3, canMessage.length-3 );
			node->getValue()->receivedFragments |= currentFragment;

			// test if this was the last segment, otherwise we have to wait for more messages
			if ( xpcc::utils::bitCount( node->getValue()->receivedFragments ) == numberOfFragments )
			{
				this->receivedMessages.append( node );
				this->receivingMessages.remove( node );
			}
		}
		else {
			ReceiveListItem* message = new ReceiveListItem( canMessage.length-1, header );
			memcpy( message->data.getPointer(), canMessage.data+1, canMessage.length-1 );
			this->receivedMessages.append( new typename ReceiveList::Node( message ) );
		}

		return true;
	}
	// todo Save values
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
	while ( this->isCanMessageAvailable() )
	{
		this->retrieveCanMessage();
	}
}

