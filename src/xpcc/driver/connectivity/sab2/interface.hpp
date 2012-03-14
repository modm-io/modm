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

#ifndef	XPCC_SAB2__INTERFACE_HPP
#define	XPCC_SAB2__INTERFACE_HPP

#include <cstddef>
#include <stdint.h>

#include <xpcc/architecture/utils.hpp>
#include <xpcc/utils/template_metaprogramming.hpp>

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

#endif	// XPCC_SAB2__INTERFACE_HPP
