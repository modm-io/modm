/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2011-2013, Niklas Hauser
 * Copyright (c) 2013, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_AMNB_CONSTANTS_HPP
#define	XPCC_AMNB_CONSTANTS_HPP

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
			ERROR_GENERAL_ERROR = 0x00,
			ERROR_ACTION_NO_ACTION = 0x01, ///< No corresponding action found for this command
			
			/**
			 * \brief	Unexpected payload length
			 * 
			 * The payload length of the received message differs from the
			 * expected length for the given command.
			 */
			ERROR_ACTION_WRONG_PAYLOAD_LENGTH = 0x02,
			
			/**
			 * \brief	No response given by the user
			 * 
			 * This error code is generated when no response method is called
			 * by the user during an action callback.
			 */
			ERROR_ACTION_NO_RESPONSE = 0x03,
			
			ERROR_QUERY_ERROR_CODE = 0x04, ///< Query answer contains an more detailed error code
			
			ERROR_QUERY_TIMEOUT = 0x05, ///< Query timed out
			
			ERROR_QUERY_WRONG_PAYLOAD_LENGTH = 0x06, ///< Query answer has wrong payload length
			
			ERROR_QUERY_IN_PROGRESS = 0x07, ///< Query is already in progress
			
			ERROR_TRANSMITTER_BUSY = 0x08,	///< Interface is currently transmitting
			
			ERROR_MESSAGE_OVERWRITTEN = 0x09, ///< Another message will be transmitted before this one
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

#endif	// XPCC_AMNB_CONSTANTS_HPP
