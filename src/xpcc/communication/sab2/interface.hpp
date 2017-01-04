/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2009, Georgi Grinshpun
 * Copyright (c) 2012-2013, 2016, Niklas Hauser
 * Copyright (c) 2013, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_SAB2_INTERFACE_HPP
#define	XPCC_SAB2_INTERFACE_HPP

#include <cstddef>
#include <stdint.h>

#include <modm/architecture/utils.hpp>
#include <modm/utils/template_metaprogramming.hpp>

#include "constants.hpp"

namespace xpcc
{
	namespace sab2
	{
		/**
		 * \internal
		 * \ingroup	sab2
		 */
		uint16_t
		crcUpdate(uint16_t crc, uint8_t data);
		
		/**
		 * \brief	SAB2 interface
		 * 
		 * The framing is adapted from the HDLC asynchronous framing.
		 * See http://en.wikipedia.org/wiki/High-Level_Data_Link_Control#Asynchronous_framing
		 * 
		 * \author	Fabian Greif
		 * \ingroup	sab2
		 */
		template <typename Device, std::size_t N = maxPayloadLength>
		class Interface
		{
		public:
			// select the type of the index variables with some template magic :-)
			typedef typename xpcc::tmp::Select< (N >= 255),
												uint16_t,
												uint8_t >::Result Index;
			
			typedef Index Size;
			
		public:
			/**
			 * \brief	Initialize the interface
			 * 
			 * The UART has the be configured before calling this method.
			 */
			static void
			initialize();
			
			/**
			 * \brief	Send a message
			 * 
			 * \param	address			receiver address
			 * \param	flags			see xpcc::sab::Flags
			 * \param	command			command byte
			 * \param	*payload		data field
			 * \param	payloadLength	size of the data field
			 */
			static void
			sendMessage(uint8_t address, Flags flags, uint8_t command,
					const void *payload, Size payloadLength);
			
			/**
			 * \brief	Send a message
			 */
			template <typename T>
			static void xpcc_always_inline
			sendMessage(uint8_t address, Flags flags, uint8_t command,
					const T& payload);
			
			/**
			 * \brief	Send a empty message
			 */
			static void xpcc_always_inline
			sendMessage(uint8_t address, Flags flags, uint8_t command);
			
			/**
			 * \brief	Check if a message was received
			 * 
			 * Reset the status with a call of dropMessage().
			 */
			static inline bool
			isMessageAvailable();
			
			static inline uint8_t
			getAddress();
			
			static inline uint8_t
			getCommand();
			
			static inline bool
			isResponse();
			
			/**
			 * \brief	Check if the message is an ACK or NACK
			 * \return	\c true if the message is an ACK, \c false on NACK.
			 */
			static inline bool
			isAcknowledge();
			
			/**
			 * \brief	Access the data of a received message
			 * 
			 * Data access is only valid after isMessageAvailable() returns
			 * \c true and before any call of dropMessage() or update()
			 */
			static inline const uint8_t *
			getPayload();
			
			/**
			 * \return	Size of the received message. Zero if no message
			 * 			is available at the moment.
			 */
			static inline Size
			getPayloadLength();
			
			/**
			 * \brief	End procession of the current message
			 */
			static void
			dropMessage();
			
			/**
			 * \brief	Update internal status
			 * 
			 * Has to be called periodically. Encodes received messages.
			 */
			static void
			update();
			
		private:
			static void
			writeByteEscaped(uint8_t data);
			
			static uint8_t buffer[N + 4];
			static uint16_t crc;
			static Size length;
			static Size lengthOfReceivedMessage;
			static bool nextEscaped;
		};
	}
}

#include "interface_impl.hpp"

#endif	// XPCC_SAB2_INTERFACE_HPP
