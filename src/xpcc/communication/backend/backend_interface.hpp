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
 * $Id: backend_interface.hpp 90 2009-10-17 15:51:00Z thundernail $
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_BACKEND_INTERFACE_H
#define	XPCC_BACKEND_INTERFACE_H

#include <stdint.h>

#include "../smart_payload.hpp"

namespace xpcc
{
	/**
	 * @ingroup		communication
	 * @brief 		The header of the communication.
	 *
	 *
	 * @version		$Id: backend_interface.hpp 90 2009-10-17 15:51:00Z thundernail $
	 * @author		Martin Rosekeit, Fabian Greif
	 */
	struct Header
	{
		typedef enum {
			REQUEST,
			RESPONSE,
			NEGATIVE_RESPONSE,
		} Type;

		Header() :
			type( REQUEST ),
			isAcknowledge ( false ),
			destination ( 0 ),
			source ( 0 ),
			packetIdentifier ( 0 )
		{};

		Header(Type type, bool isAck, uint8_t dest, uint8_t source, uint8_t id) :
			type( type ),
			isAcknowledge ( isAck ),
			destination ( dest ),
			source ( source ),
			packetIdentifier ( id )
		{};
		
		Type type;
		bool isAcknowledge;
		
		uint8_t destination;
		uint8_t source;
		uint8_t packetIdentifier;
		
		bool
		operator==(const Header& other) {
			return ((type == other.type) &&
					(isAcknowledge == other.isAcknowledge) &&
					(destination == other.destination) &&
					(source == other.source) &&
					(packetIdentifier == other.packetIdentifier));
		}
	};
	
	/**
	 * @ingroup		communication
	 * @brief 		The BackendInterface provides a common interface for using
	 * 				different hardware modules to transmit messages.
	 *
	 * All backends have to implement the this interface.
	 *
	 * @version		$Id: backend_interface.hpp 90 2009-10-17 15:51:00Z thundernail $
	 * @author		Martin Rosekeit, Fabian Greif
	 */
	class BackendInterface
	{
	public:
		virtual
		~BackendInterface() {};

		virtual void
		update() = 0;
		
		//! Send a Message. // soll denn der header kopiert werden?
		virtual void
		sendPacket( const Header &header, const SmartPayload& payload = SmartPayload() ) = 0;
		
		
		//! Check if a new packet was received by the backend
		virtual bool
		isPacketAvailable() const = 0;
		
		//! Access the packet.
		virtual const Header&
		getPacketHeader() const = 0;
		
		virtual const uint8_t *
		getPacketPayload() const = 0;
		
		virtual uint8_t
		getPacketPayloadSize() const = 0;
		
		virtual void
		dropPacket() = 0;
	};
}

#endif	// XPCC_BACKEND_INTERFACE_H
