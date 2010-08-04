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

#ifndef	XPCC_APB__INTERFACE_HPP
#define	XPCC_APB__INTERFACE_HPP

#include <stdint.h>
#include <xpcc/utils/macros.hpp>

namespace xpcc
{
	namespace apb
	{
		/**
		 * \internal
		 */
		class Base
		{
		public:
			enum Error
			{
				GENERAL_ERROR = 0,
				NO_ACTION,
				WRONG_PAYLOAD_LENGTH
			};
			
			static const uint8_t masterAddress = 0;
			static const uint8_t maxPayloadLength = 32;
			
		protected:
			Base()
			{
			}
			
			static const uint8_t syncByte = 0x54;
			static const uint8_t crcInitialValue = 0x00;
			
			static uint8_t
			crcUpdate(uint8_t crc, uint8_t data);
		};
		
		/**
		 * \brief	APB Interface
		 * 
		 * Example:
		 * \include apb_interface.cpp
		 * 
		 * \ingroup	apb
		 * \author	Fabian Greif
		 */
		template <typename Device>
		class Interface : public Base
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
			 * \param	header			header flags and receiver address
			 * \param	acknowledge		\c true if message is an acknowledge,
			 * 							\c false otherwise
			 * \param	command			command byte
			 * \param	*payload		data field
			 * \param	payloadLength	size of the data field
			 */
			static void
			sendMessage(uint8_t address, bool acknowledge, uint8_t command,
					const void *payload, uint8_t payloadLength);
			
			/**
			 * \brief	Send a message
			 */
			template <typename T>
			static void ALWAYS_INLINE
			sendMessage(uint8_t address, bool acknowledge, uint8_t command,
					const T& payload);
			
			/**
			 * \brief	Send a empty message
			 */
			static void
			sendMessage(uint8_t address, bool acknowledge, uint8_t command);
			
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
			enum State
			{
				SYNC,
				LENGTH,
				DATA
			};
			
			enum Flags
			{
				ACK = 0x80,
				NACK = 0x00
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

#endif	// XPCC_APB__INTERFACE_HPP
