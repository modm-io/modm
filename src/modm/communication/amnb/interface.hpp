/*
 * Copyright (c) 2009, Georgi Grinshpun
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2011-2013, 2015-2016, Niklas Hauser
 * Copyright (c) 2013, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	MODM_AMNB_INTERFACE_HPP
#define	MODM_AMNB_INTERFACE_HPP

#include <cstddef>
#include <stdint.h>
#include <modm/architecture/utils.hpp>
#include <modm/processing/timer.hpp>
#include <modm/architecture/interface/delay.hpp>
#include <stdlib.h>
#include <modm/architecture/interface/clock.hpp>
#include <modm/processing/timer/timestamp.hpp>

#include "constants.hpp"

#define AMNB_TIMING_DEBUG 0

namespace modm
{
	namespace amnb
	{
		/**
		 * This clock should run at 10kHz. It is used to provide the back-off
		 * timer with finer control over timing, which enabled much higher
		 * thoughput.
		 * When using a very high datarate and short message length this clock
		 * can be run at higher speeds than the recommended 10kHz.
		 */
		class Clock
		{
		public:
			static Timestamp
			now();
			
			/// \brief	Set the current time
			static inline void
			increment(uint_fast16_t step = 1)
			{
				time += step;
			}
			
		protected:
			static uint_fast16_t time;
		};
		
		/**
		 * \internal
		 * \brief	Universal base class for the AMNB interface
		 * 
		 * \see		<a href="http://www.maxim-ic.com/app-notes/index.mvp/id/27" target="_blank">
		 * 			Understanding and Using Cyclic Redundancy Checks with Maxim iButton Products</a>
		 * \ingroup	amnb
		 */
		uint8_t
		crcUpdate(uint8_t crc, uint8_t data);
		
		/**
		 * \brief	AMNB interface
		 * 
		 * \author	Niklas Hauser
		 *
		 * \ingroup	amnb
		 */
		template <typename Device, uint8_t PROBABILITY=50, uint8_t TIMEOUT=4>
		class Interface
		{
		public:
			/**
			 * \brief	Initialize the interface
			 * 
			 * \param seed for the random number generator
			 */
			static void
			initialize(int seed);
			
			/**
			 * \brief	Send a message
			 * 
			 * \param	address			receiver address
			 * \param	flags			see modm::amnb::Flags
			 * \param	command			command byte
			 * \param	*payload		data field
			 * \param	payloadLength	size of the data field
			 */
			static bool
			sendMessage(uint8_t address, Flags flags, uint8_t command,
					const void *payload, uint8_t payloadLength);
			
			/**
			 * \brief	Send a message
			 */
			template <typename T>
			static bool modm_always_inline
			sendMessage(uint8_t address, Flags flags, uint8_t command,
					const T& payload);
			
			/**
			 * \brief	Send a empty message
			 */
			static bool modm_always_inline
			sendMessage(uint8_t address, Flags flags, uint8_t command);
			
			/**
			 * \brief	Check if a message was received
			 * 
			 * Reset the status with a call of dropMessage().
			 */
			static modm_always_inline bool
			isMessageAvailable();
			
			static modm_always_inline uint8_t
			getTransmittedAddress();
			
			static modm_always_inline uint8_t
			getTransmittedCommand();
			
			static modm_always_inline Flags
			getTransmittedFlags();
			
			static modm_always_inline uint8_t
			getAddress();
			
			static modm_always_inline uint8_t
			getCommand();
			
			static modm_always_inline bool
			isResponse();
			
			/**
			 * \brief	Check if the message is an ACK or NACK
			 * \return	\c true if the message is an ACK, \c false on NACK.
			 */
			static modm_always_inline bool
			isAcknowledge();
			
			/**
			 * \return	\c true you are allowed to send right now
			 */
			static modm_always_inline bool
			isBusAvailable();
			
			/**
			 * \brief	Check if there has been an error during transmission
			 * \return	\c true if the message has been transmitted without
			 *			collision.
			 */
			static modm_always_inline bool
			messageTransmitted();
			
			/**
			 * \brief	Access the data of a received message
			 * 
			 * Data access is only valid after isMessageAvailable() returns
			 * \c true and before any call of dropMessage() or update()
			 */
			static modm_always_inline const uint8_t *
			getPayload();
			
			/**
			 * \return	Size of the received message. Zero if no message
			 * 			is available at the moment.
			 */
			static modm_always_inline uint8_t
			getPayloadLength();
			
			/**
			 * \brief	End procession of the current message
			 */
			static void
			dropMessage();
			
			/**
			 * \brief	Update internal status
			 * 
			 * Has to be called periodically. Decodes received messages.
			 */
			static void
			update();
			
#if AMNB_TIMING_DEBUG
			static modm::Timestamp latency;
			static uint8_t collisions;
#endif
			
		private:
			static bool
			writeMessage();
			
			enum State
			{
				SYNC,
				LENGTH,
				DATA
			};
			
			static uint8_t rx_buffer[maxPayloadLength + 3];
			static uint8_t tx_buffer[maxPayloadLength + 4];
			static uint8_t crc;
			static uint8_t position;
			static uint8_t length;
			static uint8_t lengthOfReceivedMessage;
			static uint8_t lengthOfTransmitMessage;
			static modm::ShortTimeout resetTimer;
			static const uint8_t resetTimeout = 4;
			
			static bool rescheduleTransmit;
			static bool hasMessageToSend;
			static bool messageSent;
			static bool transmitting;
			static modm::Timeout<modm::amnb::Clock> rescheduleTimer;
			static uint8_t rescheduleTimeout;
			
			static State state;
		};
	}
}

#include "interface_impl.hpp"

#endif	// MODM_AMNB_INTERFACE_HPP
