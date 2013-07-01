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
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_AMNB__CONSTANTS_HPP
#define	XPCC_AMNB__CONSTANTS_HPP

#include <stdint.h>

namespace xpcc
{
	namespace amnb
	{
		/**
		 * \brief	Error code
		 * 
		 * Error codes below 0x20 are reserved for the system. Every other
		 * code may be used by user.
		 * 
		 * \ingroup	amnb
		 */
		enum Error
		{
			/**
			 * \brief	Universal error code
			 * 
			 * Use this code if you're not sure what error to signal. If
			 * the user should react to the error code, create a specific
			 * one for the given problem.
			 */
			ERROR__GENERAL_ERROR = 0x00,
			ERROR__ACTION_NO_ACTION = 0x01, ///< No corresponding action found for this command
			
			/**
			 * \brief	Unexpected payload length
			 * 
			 * The payload length of the received message differs from the
			 * expected length for the given command.
			 */
			ERROR__ACTION_WRONG_PAYLOAD_LENGTH = 0x02,
			
			/**
			 * \brief	No response given by the user
			 * 
			 * This error code is generated when no response method is called
			 * by the user during an action callback.
			 */
			ERROR__ACTION_NO_RESPONSE = 0x03,
			
			ERROR__QUERY_ERROR_CODE = 0x04, ///< Query answer contains an more detailed error code
			
			ERROR__QUERY_TIMEOUT = 0x05, ///< Query timed out
			
			ERROR__QUERY_WRONG_PAYLOAD_LENGTH = 0x06, ///< Query answer has wrong payload length
			
			ERROR__QUERY_IN_PROGRESS = 0x07, ///< Query is already in progress
			
			ERROR__TRANSMITTER_BUSY = 0x08,	///< Interface is currently transmitting
			
			ERROR__MESSAGE_OVERWRITTEN = 0x09, ///< Another message will be transmitted before this one
		};
		
		/**
		 * \brief	Flags
		 * \ingroup	amnb
		 */
		enum Flags
		{
			BROADCAST = 0x00,	///< send a message without getting a response
			REQUEST = 0x40,		///< request data from a node
			NACK = 0x80,		///< data request Negative ACKnowledge
			ACK = 0xc0			///< data request positive ACKnowledge
		};
		
		/**
		 * \brief	Maximum length for the payload
		 * \ingroup	amnb
		 */
		const uint8_t maxPayloadLength = 32;
		
		/**
		 * \internal
		 * \brief	Universal base class for the AMNB interface
		 * \ingroup	amnb
		 */
		const uint8_t syncByte = 0x54;
		
		/**
		 * \internal
		 * \brief	Initial value for the CRC8 calculation
		 * \ingroup	amnb
		 */
		const uint8_t crcInitialValue = 0x00;
	}
}

#endif	// XPCC_AMNB__CONSTANTS_HPP
