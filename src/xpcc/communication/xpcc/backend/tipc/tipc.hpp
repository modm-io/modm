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
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__TIPC_HPP
#define XPCC__TIPC_HPP

#include <xpcc/architecture/platform/hosted/linux/tipc/header.hpp>
#include <xpcc/architecture/platform/hosted/linux/tipc/tipc_receiver.hpp>
#include <xpcc/architecture/platform/hosted/linux/tipc/tipc_transmitter.hpp>

#include <xpcc/container/smart_pointer.hpp>

#include "../backend_interface.hpp"

namespace xpcc
{
	/**
	 * \brief	Class that connects the communication to the TIPC
	 * 
	 * Messages that are received by the same connector, that has transmitted
	 * them, will be ignored.
	 * 
	 * \see 	tipc
	 * 
	 * \ingroup	backend
	 * \author	Martin Rosekeit <martin.rosekeit@rwth-aachen.de>
	 */
	class TipcConnector : public BackendInterface
	{
	public :
		TipcConnector(unsigned int domainId=tipc::Header::DOMAIN_ID_UNDEFINED);
		
		~TipcConnector();
		
		/**
		 * \brief	Add a new event to receive
		 * 
		 * Call this method for every event you want to receive.
		 * 
		 * \param	id	Identifier of the event.
		 */
		inline void
		addEventId(uint8_t id)
		{
			this->receiver.addEventId(id);
		}
		
		/**
		 * \brief	Add a new receiver
		 * 
		 * You need to call this method for every component implemented in
		 * this module.
		 * 
		 * \param	id	Identifier of the receiving component.
		 */
		inline void
		addReceiverId(uint8_t id)
		{
			this->receiver.addReceiverId(id);
		}
		
		/// Check if a new packet was received by the backend
		virtual bool
		isPacketAvailable() const;
		
		/**
		 * \brief	Access the packet header
		 * 
		 * Only valid if isPacketAvailable() returns \c true.
		 */
		virtual const Header&
		getPacketHeader() const;
		
		/**
		 * \brief	Access the packet payload
		 * 
		 * Only valid if isPacketAvailable() returns \c true.
		 */
		virtual const SmartPointer
		getPacketPayload() const;
		
		/**
		 * \brief	Delete the current packet
		 * 
		 * Only valid if isPacketAvailable() returns \c true.
		 */
		virtual void
		dropPacket();
		
		/**
		 * \brief	Update method
		 * 
		 * Does nothing here as TIPC is implemented with threads.
		 */
		virtual void
		update();
		
		/**
		 * Send a Message.
		 */
		virtual void
		sendPacket(const Header &header,
				   SmartPointer payload = SmartPointer());
		
	private:
		tipc::Transmitter transmitter;
		tipc::Receiver receiver;
	};
};
 
#endif	// XPCC__TIPC_HPP
