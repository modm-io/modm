/*
 * Copyright (c) 2013, 2015, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_RPR__INTERFACE_HPP
#define	XPCC_RPR__INTERFACE_HPP

#include <cstddef>
#include <stdint.h>

#include <xpcc/architecture/utils.hpp>
#include <xpcc/utils/template_metaprogramming.hpp>
#include <xpcc/container/queue.hpp>
#include <xpcc/processing/timer.hpp>
#include <xpcc/utils/allocator/dynamic.hpp>

#include "constants.hpp"

namespace xpcc
{
	namespace rpr
	{
		/**
		 * \internal
		 * \ingroup	token
		 */
		uint16_t
		crcUpdate(uint16_t crc, uint8_t data);
		
		/**
		 * \brief	Token Ring interface
		 *
		 * \author	Fabian Greif
		 * \author	Niklas Hauser
		 * \ingroup	token
		 */
		template <typename Device, std::size_t N = maxPayloadLength>
		class Interface
		{
		public:
			/**
			 * \brief	Initialize the interface
			 * 
			 * The UART has the be configured before calling this method.
			 */
			static void
			initialize(uint16_t address, uint16_t groupAddress=0x7fff);
			
			/**
			 * \brief	Send a message
			 * 
			 * \param	destination		14bit destination address
			 * \param	type			type of message for address formatting
			 * \param	command			command byte
			 * \param	*payload		data field
			 * \param	payloadLength	size of the data field
			 * \param	priority		Message priority, higher is more important
			 */
			static bool
			sendMessage(uint16_t destination,
						MessageType type,
						uint8_t command,
						const void *payload,
						std::size_t payloadLength);
			
			static inline bool
			sendMessage(Message *message);
			
			/**
			 * \brief	Returns a pointer to a struct with information
			 *			about the received message
			 */
			static inline Message *
			getReceivedMessage();
			
			/**
			 * \brief remove the received message from the receiver queue
			 *
			 */
			static inline void
			dropReceivedMessage();
			
			/**
			 * \brief	Update internal status
			 * 
			 * Has to be called periodically. Encodes received messages.
			 */
			static void
			update();
			
		private:
			typedef xpcc::BoundedQueue< Message, 10 > Queue;
			
			static void
			writeByteEscaped(uint8_t data);
			
			static void
			writeMessage(Message *message);
			
			// more advanced list management
			static bool
			pushMessage(Queue &queue, Message *message);
			
			static void
			popMessage(Queue &queue);
			
			static Message *
			getMessage(Queue &queue);
			
			static bool
			moveMessage(Queue &destination, Queue &source);
			
		private:	
			static uint16_t _address;
			static uint16_t _groupAddress;
			
			static Queue messagesToSend;
			static Queue receivedMessages;
			
			static xpcc::allocator::Dynamic<uint8_t> bufferAllocator;
			
			static Message receiveBuffer;
			static uint8_t rx_buffer[N+8];
			
			static uint16_t crc;
			static uint8_t length;
			static bool nextEscaped;
			
			static uint8_t addressBuffer;
			
			enum Status
			{
				STATUS_START_DELIMITER_RECEIVED = 0x80,
				STATUS_END_DELIMITER_RECEIVED = 0x40,
				
				STATUS_DESTINATION_RECOGNISED = 0x04,
				STATUS_RX_BUFFER_OVERFLOW = 0x02,
				STATUS_SOURCE_RECOGNISED = 0x01,
			};
			
			static uint8_t status;
		};
	}
}

#include "interface_impl.hpp"

#endif	// XPCC_RPR__INTERFACE_HPP
