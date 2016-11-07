/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012-2013, Niklas Hauser
 * Copyright (c) 2013, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_SAB__CONSTANTS_HPP
#define	XPCC_SAB__CONSTANTS_HPP

#include <stdint.h>

namespace xpcc
{
	namespace sab
	{
		/**
		 * \brief	Error code
		 * 
		 * Error codes below 0x20 are reserved for the system. Every other
		 * code may be used by user.
		 * 
		 * \ingroup	sab
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
			ERROR__NO_ACTION = 0x01, 		///< No corresponding action found for this command
			
			/**
			 * \brief	Unexpected payload length
			 * 
			 * The payload length of the received message differs from the
			 * expected length for the given command.
			 */
			ERROR__WRONG_PAYLOAD_LENGTH = 0x02,
			
			/**
			 * \brief	No response given by the user
			 * 
			 * This error code is generated when no response method is called
			 * by the user during an action callback.
			 */
			ERROR__NO_RESPONSE = 0x03,
		};
		
		/**
		 * \brief	Flags
		 * \ingroup	sab
		 */
		enum Flags
		{
			REQUEST = 0x00,
			ACK = 0xc0,
			NACK = 0x80
		};
		
		/**
		 * \brief	Maximum length for the payload
		 * \ingroup	sab
		 */
		const uint8_t maxPayloadLength = 32;
		
		/**
		 * \internal
		 * \brief	Universal base class for the SAB interface
		 * \ingroup	sab
		 */
		const uint8_t syncByte = 0x54;
		
		/**
		 * \internal
		 * \brief	Initial value for the CRC8 calculation
		 * \ingroup	sab
		 */
		const uint8_t crcInitialValue = 0x00;
	}
}

#endif	// XPCC_SAB__CONSTANTS_HPP
