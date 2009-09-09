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

#ifndef	XPCC_BACKEND_INTERFACE_H
#define	XPCC_BACKEND_INTERFACE_H

#include <stdint.h>

namespace xpcc
{
	struct Packet
	{
		typedef enum {
			ACTION,
			RESPONSE,
			NEGATIVE_RESPONSE,
		} Type;
		
		typedef enum {
			REQUEST,
			ACKNOWLEDGE
		} Direction;
		
		struct Header
		{
			Type type;
			Direction direction;
			
			bool isMessageIdentifierActive;
			bool isFragmented;
			
			uint8_t destination;
			uint8_t source;
			uint8_t actionIdentifier;
		};
		
		Header header;
		
		uint8_t data[8];
		uint8_t size;
	};
	
	class BackendInterface
	{
	public:
		virtual bool
		sendPacket(Packet &packet) = 0;
		
		
		virtual bool
		isPacketAvailable() const = 0;
		
		virtual void
		retrievePacket() = 0;
		
		virtual const Packet&
		getPacket() const = 0;
	};
}

#endif	// XPCC_BACKEND_INTERFACE_H
