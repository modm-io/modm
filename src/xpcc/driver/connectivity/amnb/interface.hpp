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
 * 
 * $hello please remove me$
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_AMNB__INTERFACE_HPP
#define	XPCC_AMNB__INTERFACE_HPP

#include <cstddef>
#include <stdint.h>
#include <xpcc/architecture/utils.hpp>
#include <xpcc/workflow/timeout.hpp>
#include <xpcc/architecture/driver/delay.hpp>
#include <stdlib.h>
#include <xpcc/architecture/driver/clock.hpp>
#include <xpcc/workflow/timestamp.hpp>

#include "constants.hpp"

#define AMNB_TIMING_DEBUG 1

namespace xpcc
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
			 * \param	flags			see xpcc::amnb::Flags
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
			static bool ALWAYS_INLINE
			sendMessage(uint8_t address, Flags flags, uint8_t command,
					const T& payload);
			
			/**
			 * \brief	Send a empty message
			 */
			static bool ALWAYS_INLINE
			sendMessage(uint8_t address, Flags flags, uint8_t command);
			
			/**
			 * \brief	Check if a message was received
			 * 
			 * Reset the status with a call of dropMessage().
			 */
			static ALWAYS_INLINE bool
			isMessageAvailable();
			
			static ALWAYS_INLINE uint8_t
			getTransmittedAddress();
			
			static ALWAYS_INLINE uint8_t
			getTransmittedCommand();
			
			static ALWAYS_INLINE Flags
			getTransmittedFlags();
			
			static ALWAYS_INLINE uint8_t
			getAddress();
			
			static ALWAYS_INLINE uint8_t
			getCommand();
			
			static ALWAYS_INLINE bool
			isResponse();
			
			/**
			 * \brief	Check if the message is an ACK or NACK
			 * \return	\c true if the message is an ACK, \c false on NACK.
			 */
			static ALWAYS_INLINE bool
			isAcknowledge();
			
			/**
			 * \return	\c true you are allowed to send right now
			 */
			static ALWAYS_INLINE bool
			isBusAvailable();
			
			/**
			 * \brief	Check if there has been an error during transmission
			 * \return	\c true if the message has been transmitted without
			 *			collision.
			 */
			static ALWAYS_INLINE bool
			messageTransmitted();
			
			/**
			 * \brief	Access the data of a received message
			 * 
			 * Data access is only valid after isMessageAvailable() returns
			 * \c true and before any call of dropMessage() or update()
			 */
			static ALWAYS_INLINE const uint8_t *
			getPayload();
			
			/**
			 * \return	Size of the received message. Zero if no message
			 * 			is available at the moment.
			 */
			static ALWAYS_INLINE uint8_t
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
			static xpcc::Timestamp latency;
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
			static xpcc::Timeout<> resetTimer;
			static const uint8_t resetTimeout = 4;
			
			static bool rescheduleTransmit;
			static bool hasMessageToSend;
			static bool messageSent;
			static bool transmitting;
			static xpcc::Timeout<xpcc::amnb::Clock> rescheduleTimer;
			static uint8_t rescheduleTimeout;
			
			static State state;
		};
	}
}

#include "interface_impl.hpp"

#endif	// XPCC_AMNB__INTERFACE_HPP
