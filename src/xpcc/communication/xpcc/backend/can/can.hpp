// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
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

#ifndef	XPCC_CAN_HPP
#define	XPCC_CAN_HPP

#include "../backend_interface.hpp"

namespace xpcc
{
	/**
	 * \brief	The CAN interface to the XPCC communication
	 *
	 * \subsection can_interface_definition The Needed Interface
	 * The interface of the per template parameter given driver has to
	 * provide the following static methods.
	 * \code
	 *	static inline bool
	 *	isMessageAvailable();
	 *
	 *	static bool
	 *	getMessage(Can::Message& message);
	 *
	 *	static bool
	 *	canSend();
	 *
	 *	static bool
	 *	sendMessage(const Can::Message& message);
	 * \endcode
	 *
	 * \ingroup	communication
	 * \version	$Id$
	 */
	template<typename C>
	class CanInterface : public BackendInterface
	{
	public:
		CanInterface();
		
		virtual
		~CanInterface();
		
		virtual void
		sendPacket(const Header &header, SmartPointer payload);
		
		virtual bool
		isPacketAvailable() const {
			return (receivedMessages != 0);
		}
		
		virtual const Header&
		getPacketHeader() const {
			return receivedMessages->header;
		}
		
		virtual const xpcc::SmartPointer
		getPacketPayload() const {
			return this->receivedMessages->data;
		}
		
		virtual uint8_t
		getPacketPayloadSize() const {
			return receivedMessages->size;
		}
		
		virtual void
		dropPacket();
		
		virtual void
		update() = 0;
	
	protected:
		/**
		 *  \brief	Try to send a CAN message
		 *
		 * \return	\b true if the message could be send, \b false otherwise
		 */
		virtual bool
		sendCanMessage(const Header &header, const uint8_t *data, uint8_t size) = 0;
		
		
		virtual bool
		isCanMessageAvailable() const = 0;
		
		virtual bool
		retrieveCanMessage() = 0;
		
		virtual uint32_t
		getCanIdentifier() = 0;
		
		/**
		 *  \brief	Read message data
		 *
		 * \param[out]	data
		 * \return		size
		 */
		virtual uint8_t
		getCanData(uint8_t *data) = 0;
	
	private:
		void
		sendWaitingMessages();
		
		void
		checkAndReceiveMessages();
		
		void
		readMessage();
		
		class SendListItem
		{
		public:
			SendListItem(const Header& header, const SmartPointer& payload) :
				next(0), header(header), payload(payload),
				fragmentIndex(0) {
			}
			
			SendListItem *next;
			
			Header header;
			SmartPointer payload;
			
			uint8_t fragmentIndex;
		
		private:
			SendListItem(const SendListItem& other);
			
			SendListItem&
			operator=(const SendListItem& other);
		};
		
		class ReceiveListItem
		{
		public:
			ReceiveListItem(const Header& header) :
				next(0), header(header), size(0),
				receivedFragments(0) {
			}
			
			ReceiveListItem *next;
			
			Header header;
			SmartPointer data;
			uint8_t size;
			
			uint8_t receivedFragments;
			// TODO timeout
		
		private:
			ReceiveListItem(const ReceiveListItem& other);
			
			ReceiveListItem&
			operator=(const ReceiveListItem& other);
		};
		
		SendListItem *sendList;
		ReceiveListItem *receivingMessages;
		ReceiveListItem *receivedMessages;
	};
}

#include "can_impl.hpp"

#endif	// XPCC_CAN_HPP
