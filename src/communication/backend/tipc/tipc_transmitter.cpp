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
 * $Id$
 */
// ----------------------------------------------------------------------------
#include "./tipc_transmitter.h"
#include "./tipc_header_definition.h"

#include <iostream>
#include <boost/shared_array.hpp>

#include <modules/backplane/logging/logger_message_forwarder.hpp>
SUB_LOGGER_CREATE(logger, Logger::WARNING, "icc::tipc::transmitter");

// TODO: IMPORTANT! If the TIPC-Module is not loaded terminate with appropriate
// error message!!!!

// -------------------------------------------------------------------------
rca::tipc::Transmitter::Transmitter(	icc::TChannelId 	channelId, 
										icc::TComponentId	componentId) :
	channelId_(channelId),
	componentId_(componentId)
{
}
// -------------------------------------------------------------------------
rca::tipc::Transmitter::~Transmitter() 
{
}
// -------------------------------------------------------------------------
// Attention: This method modifies the header information of the packet!!
void 
rca::tipc::Transmitter::transmitPacket(	icc::TChannelId 		receiverChannelId,
										icc::TComponentId		receiverComponentId,
										icc::TPayloadId			payloadId,
										icc::TSize				payloadSize,
										const void *			payload,
										icc::TServiceSpecifier	serviceSpecifier,
										icc::TError				error)
{
	// Speicherplatz fuers ICC-Packet erstellen
	boost::shared_array<char> iccPacket(new char[sizeof(icc::THeader) + payloadSize]);
	
	// Payload an die richtige Stelle im Packet kopieren (hinter den Header)
	// Es könnte auch Packete ohne Payload geben.
	if ( payloadSize > 0 ) {
		memcpy(	iccPacket.get() + sizeof(icc::THeader),
				payload,
				payloadSize);
	}
	
	// Header setzen vor die payload
	( *(icc::THeader*) iccPacket.get() ).transmitterChannelId	= this->channelId_;
	( *(icc::THeader*) iccPacket.get() ).receiverChannelId		= receiverChannelId;
	( *(icc::THeader*) iccPacket.get() ).transmitterComponentId	= this->componentId_;
	( *(icc::THeader*) iccPacket.get() ).receiverComponentId	= receiverComponentId;
	( *(icc::THeader*) iccPacket.get() ).size					= payloadSize;
	( *(icc::THeader*) iccPacket.get() ).payloadId				= payloadId;
	( *(icc::THeader*) iccPacket.get() ).error					= error;
	( *(icc::THeader*) iccPacket.get() ).serviceSpecifier		= serviceSpecifier;
	

	// Send packet using TIPC
	this->tipcTransmitterSocket_.transmitPayload(	payloadId + TYPE_ID_OFFSET,
													this->channelId_,
													iccPacket.get(),
													sizeof(icc::THeader) + payloadSize );
}
// -------------------------------------------------------------------------

