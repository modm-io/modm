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

namespace xpcc
{
	namespace apb
	{
		/**
		 * \brief	APB Interface
		 * 
		 * Example:
		 * \include apb_interface.cpp
		 * 
		 * \author	Fabian Greif
		 */
		template <typename DEVICE>
		class Interface
		{
		public:
			typedef enum {
				ACK = 0x80,
				NACK = 0x00
			} Header;
		
		public:
			/**
			 * \brief	Initialize the interface
			 * 
			 * Sets the baudrate for the DEVICE etc.
			 */
			static void
			initialize();
			
			static void
			sendMessage(Header header, const uint8_t *data, uint8_t size);
			
			/**
			 * \brief	Check if a message was received
			 * 
			 * Reset the status with a call of dropMessage().
			 */
			static inline bool
			isMessageAvailable();
			
			/**
			 * \return	Size of the received message. Zero if no message
			 * 			is available at the moment.
			 */
			static inline uint8_t
			getLength();
			
			/**
			 * \brief	Access the data of a received message
			 * 
			 * Data access is only valid after checkForMessage() returns
			 * not zero and before any call of dropMessage() and update()
			 */
			static inline const uint8_t *
			getData();
			
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
			static uint8_t
			crcUpdate(uint8_t crc, uint8_t data);
			
		private:
			static const uint8_t syncByte;
			static const uint8_t crcInitialValue;
			
			typedef enum {
				SYNC,
				HEADER,
				DATA
			} State;
			
			static uint8_t buffer[32];
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
