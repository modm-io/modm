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
 * $Id: tipc.cpp 92 2009-10-17 15:54:22Z thundernail $
 */
// ----------------------------------------------------------------------------

#include "tipc.hpp"

xpcc::tipc::Tipc::Tipc( ) :
	receiver( ),
	transmitter( )
{

}

// ----------------------------------------------------------------------------

xpcc::tipc::Tipc::~Tipc()
{

}

// ----------------------------------------------------------------------------

bool
xpcc::tipc::Tipc::isPacketAvailable() const
{
	return this->receiver.hasPacket();
}

// ----------------------------------------------------------------------------

const xpcc::Header&
xpcc::tipc::Tipc::getPacketHeader() const
{
	return *(xpcc::Header*)this->receiver.frontPayload().getPointer();
}

// ----------------------------------------------------------------------------

xpcc::SmartPointer
xpcc::tipc::Tipc::getPacketPayload() const
{
	SmartPointerVolatile payload( this->receiver.frontPayload().getSize() );
	if( payload.getSize() > 0 ) {
		memcpy(
				payload.getPointer(),
				this->receiver.frontPayload().getPointer() + sizeof(xpcc::Header),
				payload.getSize() );
	}
	return payload;
}

// ----------------------------------------------------------------------------

uint8_t
xpcc::tipc::Tipc::getPacketPayloadSize() const
{
	return this->receiver.frontPayload().getSize();
}

// ----------------------------------------------------------------------------

void
xpcc::tipc::Tipc::dropPacket()
{
	this->receiver.popFront();
}

// ----------------------------------------------------------------------------

void
xpcc::tipc::Tipc::sendPacket(const xpcc::Header &header, SmartPointer payload)
{
	SmartPointerVolatile combinedPayload( sizeof(xpcc::Header) + payload.getSize() );

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
xpcc::tipc::Tipc::update()
{
	// nothing to do, because TipcReceiver is using threads
}

// ----------------------------------------------------------------------------
