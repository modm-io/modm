/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Georgi Grinshpun
 * Copyright (c) 2012-2013, 2015, Niklas Hauser
 * Copyright (c) 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "fake_postman.hpp"

xpcc::Postman::DeliverInfo
FakePostman::deliverPacket(const xpcc::Header& header,
			const modm::SmartPointer& payload)
{
	this->messagesToDeliver.append(Message(header, payload));
	
	if (header.type != xpcc::Header::Type::REQUEST)
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
FakePostman::isComponentAvailable(uint8_t component) const
{
	return (component == 1 || component == 2);
}
