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
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_AMNB__INTERFACE_HPP
#define	XPCC_AMNB__INTERFACE_HPP

#include <cstddef>
#include <stdint.h>
#include <xpcc/architecture/utils.hpp>
#include <xpcc/workflow/timeout.hpp>

#include "constants.hpp"

namespace xpcc
{
	namespace amnb
	{
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
		 * \author	Fabian Greif, Niklas Hauser
		 * \ingroup	amnb
		 */
		template <typename Device>
		class Interface
		{
		public:
			/**
			 * \brief	Initialize the interface
			 * 
			 * Sets the baudrate for the DEVICE etc.
			 */
			static void
			initialize();
			
			/**
			 * \brief	Send a message
			 * 
			 * \param	address			receiver address
			 * \param	flags			see xpcc::amnb::Flags
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
			static void ALWAYS_INLINE
			sendMessage(uint8_t address, Flags flags, uint8_t command,
					const T& payload);
			
			/**
			 * \brief	Send a empty message
			 */
			static void ALWAYS_INLINE
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
			 * \brief	Check if anythings is being transmitted on the bus
			 * \return	\c true if the bus is not busy
			 */
			static inline bool
			isBusAvailable();
			
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
			static bool hasMessageToSend;
			static uint8_t lengthOfSendingMessage;
			static bool transmitting;
			static xpcc::Timeout<> timer;
			static const uint8_t timeout = 4;
			
			static State state;
		};
	}
}

#include "interface_impl.hpp"

#endif	// XPCC_AMNB__INTERFACE_HPP
