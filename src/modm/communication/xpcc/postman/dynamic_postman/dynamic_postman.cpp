/*
 * Copyright (c) 2009, Georgi Grinshpun
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012-2013, 2015, Niklas Hauser
 * Copyright (c) 2013, Sascha Schade
 * Copyright (c) 2015, Kevin Läufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "../dynamic_postman.hpp"

// ----------------------------------------------------------------------------
xpcc::DynamicPostman::DynamicPostman()
{
}

// ----------------------------------------------------------------------------
xpcc::DynamicPostman::DeliverInfo
xpcc::DynamicPostman::deliverPacket(const Header &header, const modm::SmartPointer& payload)
{
	if (header.destination == 0)
	{
		// EVENT
		EventMap::const_iterator lowerBound(this->eventMap.lower_bound(header.packetIdentifier));
		EventMap::const_iterator upperBound(this->eventMap.upper_bound(header.packetIdentifier));
		if (lowerBound != upperBound) {
			do {
				lowerBound->second(header, payload);
				lowerBound++;
			}
			while (lowerBound != upperBound);
			return OK;
		}
		return NO_EVENT;
	}
	else
	{
		// REQUEST
		ActionMap::const_iterator iterDestination(this->actionMap.find(header.destination));
		if (iterDestination != this->actionMap.end())
		{
			CallbackMap::const_iterator iterCallback(iterDestination->second.find(header.packetIdentifier));
			if (iterCallback != iterDestination->second.end())
			{
				xpcc::ResponseHandle response(header);
				iterCallback->second(response, payload);
				return OK;
			}
			else {
				return NO_ACTION;
			}
		}
		else {
			return NO_COMPONENT;
		}
	}
}

// ----------------------------------------------------------------------------
bool
xpcc::DynamicPostman::isComponentAvailable(uint8_t component) const
{
	return (this->actionMap.find(component) != this->actionMap.end());
}

// ----------------------------------------------------------------------------
xpcc::DynamicPostman::EventListener::EventListener() :
		hasPayload(-1)
{
}

xpcc::DynamicPostman::EventListener::EventListener(EventCallback call) :
		call(call), hasPayload(1)
{
}

xpcc::DynamicPostman::EventListener::EventListener(EventCallbackSimple call) :
		callSimple(call), hasPayload(0)
{
}

void
xpcc::DynamicPostman::EventListener::operator()(
		const Header& header,
		const modm::SmartPointer& payload) const
{
	if (hasPayload > 0) {
		call(header, *(payload.getPointer()));
	} else if (hasPayload == 0) {
		callSimple(header);
	}
}

// ----------------------------------------------------------------------------
xpcc::DynamicPostman::ActionHandler::ActionHandler() :
		hasPayload(-1)
{
}

xpcc::DynamicPostman::ActionHandler::ActionHandler(ActionCallback call) :
		call(call), hasPayload(1)
{
}

xpcc::DynamicPostman::ActionHandler::ActionHandler(ActionCallbackSimple call) :
		callSimple(call), hasPayload(0)
{
}

void
xpcc::DynamicPostman::ActionHandler::operator()(
		const ResponseHandle& response,
		const modm::SmartPointer& payload) const
{
	if (hasPayload > 0) {
		call(response, *(payload.getPointer()));
	} else if (hasPayload == 0) {
		callSimple(response);
	}
}
