// coding: utf-8
/* Copyright (c) 2015, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include "../dynamic_postman.hpp"

// ----------------------------------------------------------------------------
xpcc::DynamicPostman::DynamicPostman()
{
}

// ----------------------------------------------------------------------------
xpcc::DynamicPostman::DeliverInfo
xpcc::DynamicPostman::deliverPacket(const Header &header, const SmartPointer& payload)
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
		const SmartPointer& payload) const
{
	if (hasPayload > 0) {
		call(header, payload.getPointer());
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
		const SmartPointer& payload) const
{
	if (hasPayload > 0) {
		call(response, payload.getPointer());
	} else if (hasPayload == 0) {
		callSimple(response);
	}
}
