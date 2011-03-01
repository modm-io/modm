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

#include "fake_postman.hpp"

xpcc::Postman::DeliverInfo
FakePostman::deliverPacket(const xpcc::Header& header,
			const xpcc::SmartPointer& payload)
{
	this->messagesToDeliver.append(Message(header, payload));
	
	if (header.type != xpcc::Header::REQUEST)
		return ERROR;
	
	if (header.destination == 0)
	{
		// Event
		switch (header.packetIdentifier)
		{
			case 0x20:
				component1->eventNoParameter(header);
				component2->eventNoParameter(header);
				break;
				
			case 0x21:
				component1->eventUint32(header, &payload.get<uint32_t>());
				break;
				
			default:
				return NO_EVENT;
		}
		
		return OK;
	}
	else if (header.destination == 1)
	{
		xpcc::ResponseHandle response(header);
		switch (header.packetIdentifier)
		{
			case 0x10:
				component1->actionNoParameter(response);
				break;
			
			case 0x11:
				component1->actionUint16(response, &payload.get<uint16_t>());
				break;
				
			case 0x12:
				component1->actionDirectResponse(response);
				break;
				
			case 0x13:
				component1->actionDelayedResponse(response);
				break;
				
			case 0x14:
				component1->actionUint16CallAction(response, &payload.get<uint16_t>());
				break;
				
			default:
				return NO_ACTION;
		}
		
		return OK;
	}
	else if (header.destination == 2)
	{
		xpcc::ResponseHandle response(header);
		switch (header.packetIdentifier)
		{
			case 0x10:
				component2->actionNoParameter(response);
				break;
			
			case 0x11:
				component2->actionUint16(response, &payload.get<uint16_t>());
				break;
				
			default:
				return NO_ACTION;
		}
		
		return OK;
	}
	
	return NO_COMPONENT;
}

bool
FakePostman::isComponentAvaliable(uint8_t component) const
{
	return (component == 1 || component == 2);
}
