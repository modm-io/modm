/*
 * Copyright (c) 2009, Georgi Grinshpun
 * Copyright (c) 2009-2012, Fabian Greif
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

#ifndef	MODM_SAB_INTERFACE_HPP
#define	MODM_SAB_INTERFACE_HPP

#include <cstddef>
#include <stdint.h>
#include <modm/architecture/utils.hpp>

#include "constants.hpp"

namespace modm
{
	namespace sab
	{
		/**
		 * \internal
		 * \brief	Universal base class for the SAB interface
		 * 
		 * \see		<a href="http://www.maxim-ic.com/app-notes/index.mvp/id/27" target="_blank">
		 * 			Understanding and Using Cyclic Redundancy Checks with Maxim iButton Products</a>
		 * \ingroup	sab
		 */
		uint8_t
		crcUpdate(uint8_t crc, uint8_t data);
		
		/**
		 * \brief	SAB interface
		 * 
		 * Example:
		 * \include sab_interface.cpp
		 * 
		 * \author	Fabian Greif
		 * \ingroup	sab
		 */
		template <typename Device>
		class Interface
		{
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
			 * \param	flags			see modm::sab::Flags
			 * \param	command			command byte
			 * \param	*payload		data field
			 * \param	payloadLength	size of the data field
			 */
			static void
			sendMessage(uint8_t address, Flags flags, uint8_t command,
					const void *payload, uint8_t payloadLength);
			
			/**
			 * \brief	Send a message
			 */
			template <typename T>
			static void modm_always_inline
			sendMessage(uint8_t address, Flags flags, uint8_t command,
					const T& payload);
			
			/**
			 * \brief	Send a empty message
			 */
			static void modm_always_inline
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
			static inline uint8_t
			getPayloadLength();
			
			/**
			 * \brief	End processing of the current message
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
			enum State
			{
				SYNC,
				LENGTH,
				DATA
			};
			
			static uint8_t buffer[maxPayloadLength + 3];
			static uint8_t crc;
			static uint8_t position;
			static uint8_t length;
			static uint8_t lengthOfReceivedMessage;
			
			static State state;
		};
	}
}

#include "interface_impl.hpp"

#endif	// MODM_SAB_INTERFACE_HPP
