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

#include "dispatcher.hpp"

#include <xpcc/debug/logger/logger.hpp>
// set the Loglevel
#undef  XPCC_LOG_LEVEL
#define XPCC_LOG_LEVEL xpcc::log::INFO

xpcc::Dispatcher::Dispatcher(
		BackendInterface *backend,
		Postman* postman) :
	backend(backend),
	postman(postman),
	responseManager(backend, postman)
{
}

// ----------------------------------------------------------------------------
void
xpcc::Dispatcher::update()
{
	this->backend->update();
	
	//Check if a new packet was received by the backend
	while (this->backend->isPacketAvailable())
	{
		const Header& header = this->backend->getPacketHeader();
		const SmartPointer& payload = this->backend->getPacketPayload();
		
		if (header.type == Header::REQUEST && !header.isAcknowledge)
		{
			this->handleActionCall(header, payload);
		}
		else
		{
			this->responseManager.handlePacket(header, payload);
			if (!header.isAcknowledge && header.destination != 0)
			{
				if (postman->isComponentAvaliable(header)) {
					this->sendAcknowledge(header);
				}
			}
		}
		
		this->backend->dropPacket();
	}
	
	// check if there are packets to send
	this->responseManager.handleWaitingMessages();
}

void
xpcc::Dispatcher::handleActionCall(const Header& header,
		const SmartPointer& payload)
{
	xpcc::Postman::DeliverInfo result =
			postman->deliverPacket(header, payload);
	
	if (result == Postman::OK && header.destination != 0)
	{
		// transmit ACK:
		// Message is not an EVENT and the destination is outside
		// this board because internal messages aren't received
		// by the backend
		this->sendAcknowledge(header);
	}
	
	// TODO Error reporting
}

void
xpcc::Dispatcher::sendAcknowledge(const Header& header)
{
	Header ackHeader(
			header.type, true,
			header.source, header.destination,
			header.packetIdentifier);
	
	this->backend->sendPacket(ackHeader);
}
