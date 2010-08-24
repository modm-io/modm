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

#include <cstddef>
#include <stdint.h>
#include <xpcc/architecture/platform.hpp>

namespace xpcc
{
	namespace apb
	{
		/**
		 * \brief	Error code
		 * 
		 * Error codes below 0x20 are reserved for the system. Every other
		 * code may be used by user.
		 * 
		 * \ingroup	apb
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
		 * \ingroup	apb
		 */
		enum Flags
		{
			REQUEST = 0x00,
			ACK = 0xc0,
			NACK = 0x80
		};
		
		/**
		 * \brief	Maximum length for the payload
		 * \ingroup	apb
		 */
		const uint8_t maxPayloadLength = 32;
		
		/**
		 * \internal
		 * \brief	Universal base class for the APB interface
		 * \ingroup	apb
		 */
		const uint8_t syncByte = 0x54;
		
		/**
		 * \internal
		 * \brief	Initial value for the CRC8 calculation
		 * \ingroup	apb
		 */
		const uint8_t crcInitialValue = 0x00;
		
		/**
		 * \internal
		 * \brief	Universal base class for the APB interface
		 * 
		 * \see		<a href="http://www.maxim-ic.com/app-notes/index.mvp/id/27" target="_blank">
		 * 			Understanding and Using Cyclic Redundancy Checks with Maxim iButton Products</a>
		 * \ingroup	apb
		 */
		uint8_t
		crcUpdate(uint8_t crc, uint8_t data);
		
		/**
		 * \brief	APB interface
		 * 
		 * Example:
		 * \include apb_interface.cpp
		 * 
		 * \ingroup	apb
		 * \author	Fabian Greif
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
			 * \param	flags			see xpcc::apb::Flags
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
