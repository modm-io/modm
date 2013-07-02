// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2012, Roboterclub Aachen e.V.
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
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_RPR__CONSTANTS_HPP
#define	XPCC_RPR__CONSTANTS_HPP

#include <stdint.h>

namespace xpcc
{
	namespace rpr
	{
		/**
		 * \brief	Message Types
		 * \ingroup	token
		 */
		enum MessageType
		{
			MESSAGE_TYPE_ANY,
			MESSAGE_TYPE_UNICAST,
			MESSAGE_TYPE_MULTICAST,
			MESSAGE_TYPE_BROADCAST,
		};
		
		enum CommandType
		{
			COMMAND_ANY = 0xff,
		};
		
		enum AddressType
		{
			ADDRESS_ANY = 0xffff,
		};
		
		typedef struct
		{
			// Destination Address
			MessageType type;
			uint16_t destination;
			
			// Source Address
			uint16_t source;
			
			// Payload
			uint8_t command;
			uint8_t *payload;
			std::size_t length;
		}
		Message;
		
		typedef struct
		{
			Message *message;
		}
		ErrorMessage;
		
//		private: // ?
		/**
		 * \brief	Bit Masks for the higher address byte
		 * \ingroup	token
		 */
		enum Address
		{
			ADDRESS_INDIVIDUAL_GROUP = 0x8000,
			ADDRESS_BROADCAST = 0xffff,
			ADDRESS_VALUE = 0x7fff,
		};
		
		/**
		 * \brief	Maximum length for the payload
		 * \ingroup	token
		 */
		const uint8_t maxPayloadLength = 48;
		
		/**
		 * \internal
		 * \ingroup	token
		 */
		const uint8_t startDelimiterByte = 0x7e;
		const uint8_t endDelimiterByte = 0x7c;
		const uint8_t controlEscapeByte = 0x7d;
		
		/**
		 * \internal
		 * \brief	Initial value for the CRC16 calculation
		 * \ingroup	token
		 */
		const uint16_t crcInitialValue = 0xffff;
	}
}

#endif	// XPCC_RPR__CONSTANTS_HPP
