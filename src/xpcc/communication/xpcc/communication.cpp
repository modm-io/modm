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

#include "communication.hpp"

#include <xpcc/debug/logger/logger.hpp>
// set the Loglevel
#undef  XPCC_LOG_LEVEL
#define XPCC_LOG_LEVEL xpcc::log::DEBUG

xpcc::Communication::Communication(
		BackendInterface *backend,
		Postman* postman) :
	backend(backend),
	postman(postman),
	responseManager()
{
}

// ----------------------------------------------------------------------------
xpcc::Communication::~Communication()
{
}

// ----------------------------------------------------------------------------
void
xpcc::Communication::setPostman( xpcc::Postman* postman )
{
	this->postman = postman;
}

// ----------------------------------------------------------------------------
void
xpcc::Communication::update(){
	if ( this->postman != 0 ) {
		this->backend->update();
		//Check if a new packet was received by the backend
		while( this->backend->isPacketAvailable() )
		{
			//switch(postman->deliverPacket(backend))){
			//	case NO_ACTION: // send error message
			//		break;
			//	case OK:
			//	case NO_COMPONENT:
			//	case NO_EVENT:
			//}
			const Header& header( this->backend->getPacketHeader() );
			const SmartPointer payload( this->backend->getPacketPayload() );

			if ( header.type == Header::REQUEST && !header.isAcknowledge )
			{
				if ( postman->deliverPacket( header, payload ) == Postman::OK ) {
					if ( header.destination != 0 ) {
						// transmit ACK (is not an EVENT)
						Header ackHeader(
								header.type,
								true,
								header.source,
								header.destination,
								header.packetIdentifier);
						this->backend->sendPacket( ackHeader );
					}
				}
			}
			else{
				this->responseManager.handlePacket( header, payload );
			}

			this->backend->dropPacket();
		}

		// check somehow if there are packets to send
		this->responseManager.handleWaitingMessages(*postman, *backend);
	}
	else {
		XPCC_LOG_ERROR
				<< XPCC_FILE_INFO
				<< "No postman set!"
				<< xpcc::flush;
		while( this->backend->isPacketAvailable() ) {
			this->backend->dropPacket();
		}
	}
}

// ----------------------------------------------------------------------------
uint8_t
xpcc::Communication::getCurrentComponent() const
{
	return this->currentComponent;
}
		
// ----------------------------------------------------------------------------

void
xpcc::Communication::setCurrentComponent(uint8_t id)
{
	this->currentComponent = id;
}

// ----------------------------------------------------------------------------
void
xpcc::Communication::callAction(uint8_t receiver, uint8_t actionIdentifier)
{
	Header header(	Header::REQUEST,
					false,
					receiver,
					currentComponent,
					actionIdentifier);

	this->responseManager.addActionCall(header);
}

// ----------------------------------------------------------------------------
void
xpcc::Communication::callAction(uint8_t receiver, uint8_t actionIdentifier, Callback& responseCallback)
{
	Header header(	Header::REQUEST,
					false,
					receiver,
					currentComponent,
					actionIdentifier);

	this->responseManager.addActionCall(header, responseCallback);
}

