// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
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
 * $Id: tipc.cpp 607 2011-09-13 19:51:03Z dergraaf $
 */
// ----------------------------------------------------------------------------

#include "tipc.hpp"
#include <xpcc/debug/logger.hpp>

#undef  XPCC_LOG_LEVEL
#define XPCC_LOG_LEVEL xpcc::log::WARNING

// ----------------------------------------------------------------------------
xpcc::TipcConnector::TipcConnector() :
	transmitter(),
	receiver(this->transmitter.getPortId())
{
}

// ----------------------------------------------------------------------------
xpcc::TipcConnector::~TipcConnector()
{
}

// ----------------------------------------------------------------------------
bool
xpcc::TipcConnector::isPacketAvailable() const
{
	return this->receiver.hasPacket();
}

// ----------------------------------------------------------------------------
const xpcc::Header&
xpcc::TipcConnector::getPacketHeader() const
{
	return *(xpcc::Header*) this->receiver.getPacket().getPointer();
}

// ----------------------------------------------------------------------------
const xpcc::SmartPointer
xpcc::TipcConnector::getPacketPayload() const
{
	SmartPointer payload( this->receiver.getPacket().getSize() - sizeof(xpcc::Header) );
	if( payload.getSize() > 0 ) {
		memcpy(
				payload.getPointer(),
				this->receiver.getPacket().getPointer() + sizeof(xpcc::Header),
				payload.getSize() );
	}
	return payload;
}

// ----------------------------------------------------------------------------
void
xpcc::TipcConnector::dropPacket()
{
	this->receiver.dropPacket();
}

// ----------------------------------------------------------------------------
void
xpcc::TipcConnector::sendPacket(const xpcc::Header &header, SmartPointer payload)
{
//	XPCC_LOG_DEBUG << XPCC_FILE_INFO
//			<< " payload size=" << payload.getSize()
//			<< " payload=" << payload
//			<< xpcc::flush;
	
	SmartPointer combinedPayload( sizeof(xpcc::Header) + payload.getSize() );

	memcpy(	combinedPayload.getPointer(), &header, sizeof(xpcc::Header) );

	// place payload behind the header
	if ( payload.getSize() > 0 ) {
		memcpy(	combinedPayload.getPointer() + sizeof(xpcc::Header),
				payload.getPointer(),
				payload.getSize());
	}

	if ( header.destination != 0 ) {
		// transmit a REQUENST
		this->transmitter.transmitRequest( header.destination, combinedPayload );;
	}
	else {
		// transmit an EVENT
		this->transmitter.transmitEvent( header.packetIdentifier, combinedPayload );
	}
}

// ----------------------------------------------------------------------------
void
xpcc::TipcConnector::update()
{
	// nothing to do, because TipcReceiver is using threads
}
