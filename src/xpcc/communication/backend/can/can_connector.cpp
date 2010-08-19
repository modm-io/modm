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

#include "can_connector.hpp"

uint8_t xpcc::CanConnectorBase::messageCounter = 0;

// ----------------------------------------------------------------------------
uint32_t
xpcc::CanConnectorBase::convertToIdentifier(const Header & header,
		bool fragmentated)
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

// ----------------------------------------------------------------------------
bool
xpcc::CanConnectorBase::convertToHeader(const uint32_t & identifier,
		xpcc::Header & header)
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

// ----------------------------------------------------------------------------
uint8_t
xpcc::CanConnectorBase::getNumberOfFragments(uint8_t messageSize)
{
	div_t n = div(messageSize, 6);
	return (n.rem > 0) ? n.quot + 1 : n.quot;
}
