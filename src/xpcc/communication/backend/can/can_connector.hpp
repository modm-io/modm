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

#ifndef	XPCC__CAN_CONNECTOR_HPP
#define	XPCC__CAN_CONNECTOR_HPP

#include <xpcc/container/linked_list.hpp>

#include "../backend_interface.hpp"

namespace xpcc
{
	/**
	 * \brief	The CAN connector to the XPCC communication
	 *
	 * \section can_interface_definition The Needed Interface
	 * 
	 * The interface of the per template parameter given driver has to
	 * provide the following static methods.
	 * 
	 * \code
	 * static bool
	 * isMessageAvailable();
	 *
	 * static bool
	 * getMessage(can::Message& message);
	 *
	 * /// The CAN controller has a free slot to send a new message.
	 * /// \return true if a slot is available, false otherwise
	 * static bool
	 * isReadyToSend();
	 * 
	 * /// Send a message over the CAN.
	 * /// \return true if the message was send, false otherwise
	 * static bool
	 * sendMessage(const can::Message& message);
	 * \endcode
	 *
	 * \section structure Definition of the structure of a CAN message
	 * 
	 * \image html xpcc_can_identifier.png
	 *
	 * Changes in the highest 4 bits:
	 * - 2 bit: Action [0], Response [1], Neg. Response [2], not used [3]
     * - 1 bit: Request [0], Acknowledge [1] (NACK implicit in the payload)
     * - 1 bit: Message Counter active [1] / not active [1]
     *
     * Every event is send with the destination identifier \c 0x00.
	 * 
	 * \todo message counter for fragmented messages
	 * \todo timeout
	 *
	 * \ingroup	backend
	 */
	template <typename Driver>
	class CanConnector : public BackendInterface
	{
	public:
		virtual
		~CanConnector();
		
		virtual void
		sendPacket(const Header &header, SmartPointer payload);
		
		virtual bool
		isPacketAvailable() const;
		
		virtual const Header&
		getPacketHeader() const;
		
		virtual const xpcc::SmartPointer
		getPacketPayload() const;
		
		virtual uint8_t
		getPacketPayloadSize() const;
		
		virtual void
		dropPacket();
		
		virtual void
		update();
	
	protected:
		/**
		 *  \brief	Try to send a CAN message via CAN Driver
		 *
		 * \return	\b true if the message could be send, \b false otherwise
		 */
		static bool
		sendMessage(
				const Header &header,
				const uint8_t *data,
				uint8_t size,
				bool fragmentated);
		
		/// Convert a packet header to a can identifier
		static uint32_t
		convertToIdentifier(const Header & header, bool fragmentated);
		
		/**
		 * \brief	Convert a can identifier to a packet header
		 * 
		 * \param[in]	identifier	29-bit CAN identifier
		 * \param[out]	header		Packet header
		 * 
		 * \return	\c true if the message is part of a fragmented packet,
		 * 			\c false otherwise.
		 */
		static bool
		convertToHeader(const uint32_t & identifier, Header & header);
		
		/// Calculate the number of fragments needed to send a message
		/// with a length of \p messageSize.
		static inline uint8_t
		getNumberOfFragments(uint8_t messageSize);
		
		void
		sendWaitingMessages();
		
		bool
		retrieveMessage();
		
		void
		checkAndReceiveMessages();
		
	protected:
		class SendListItem
		{
		public:
			SendListItem(const Header& header, const SmartPointer& payload) :
				header(header), payload(payload),
				fragmentIndex(0)
			{
			}
			
			SendListItem(const SendListItem& other) :
				header(other.header), payload(other.payload),
				fragmentIndex(other.fragmentIndex)
			{
			}
			
			Header header;
			SmartPointer payload;
			
			uint8_t fragmentIndex;
			
		private:
			SendListItem&
			operator = (const SendListItem& other);
		};
		
		class ReceiveListItem
		{
		public:
			ReceiveListItem(uint8_t size, const Header& header) :
				header(header), payload(size),
				receivedFragments(0)
			{
			}
			
			ReceiveListItem(const ReceiveListItem& other) :
				header(other.header), payload(other.payload),
				receivedFragments(other.receivedFragments)
			{
			}
			
			bool
			operator == (const Header& header)
			{
				return (this->header == header);
			}
			
			Header header;
			SmartPointer payload;
			
			uint8_t receivedFragments;
			
		private:
			ReceiveListItem&
			operator = (const ReceiveListItem& other);
		};
		
		typedef xpcc::LinkedList<SendListItem> SendList;
		typedef xpcc::LinkedList<ReceiveListItem> ReceiveList;
		
		SendList sendList;
		ReceiveList receivingMessages;
		ReceiveList receivedMessages;
	};
}

#include "can_connector_impl.hpp"

#endif	// XPCC__CAN_CONNECTOR_HPP
