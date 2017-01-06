/*

 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	MODM_RPR_CONSTANTS_HPP
#define	MODM_RPR_CONSTANTS_HPP

#include <stdint.h>

namespace modm
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

#endif	// MODM_RPR_CONSTANTS_HPP
