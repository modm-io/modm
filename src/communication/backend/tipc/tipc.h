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
#ifndef XPCC_TIPC_H_
#define XPCC_TIPC_H_


#include "../backend_interface.hpp"
#include "header.hpp"
#include "tipc_receiver.h"
#include "tipc_transmitter.h"

namespace xpcc {
	namespace tipc {

		/*
		 * \class	Tipc
		 * \brief	Class that connects the communication to the tipc.
		 *
		 * \ingroup	tipc
		 * \version	$Id$
		 * \author	Martin Rosekeit <martin.rosekeit@rwth-aachen.de>
		 */
		class Tipc : public BackendInterface
		{
			public :
				Tipc();

				~Tipc();

				void
				addFilter(
						xpcc::Header::Type type,
						bool isAcknowledge,
						uint8_t destination,
						uint8_t source,
						uint8_t actionIdentifier);

				//! Send a Message.
				//!
				//! \return	\b true if the packet could be send, \b false otherwise.
				virtual void
				sendPacket(const xpcc::Header &header, const SmartPayload& payload);

				//! Check if a new packet was received by the backend
				virtual bool
				isPacketAvailable() const;

				//! Access the packet.
				virtual const xpcc::Header&
				getPacketHeader() const;

				virtual const uint8_t *
				getPacketPayload() const;

				virtual uint8_t
				getPacketPayloadSize() const;

				virtual void
				dropPacket();

			private :
				Receiver	receiver;
		};
	};
};
 
#endif /*XPCC_TIPC_H_*/
