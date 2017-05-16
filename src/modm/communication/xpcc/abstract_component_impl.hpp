/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2011, Georgi Grinshpun
 * Copyright (c) 2012-2013, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_ABSTRACT_COMPONENT_HPP
	#error	"Don't include this file directly, use 'abstract_component.hpp' instead"
#endif

// ----------------------------------------------------------------------------
xpcc::Communicator *
xpcc::AbstractComponent::getCommunicator()
{
	return &this->communicator;
}

// ----------------------------------------------------------------------------
void
xpcc::AbstractComponent::callAction(uint8_t receiver, uint8_t actionIdentifier)
{
	this->communicator.callAction(receiver, actionIdentifier);
}

void
xpcc::AbstractComponent::callAction(uint8_t receiver, uint8_t actionIdentifier, ResponseCallback& responseCallback)
{
	this->communicator.callAction(receiver, actionIdentifier, responseCallback);
}


// ----------------------------------------------------------------------------
void
xpcc::AbstractComponent::publishEvent(uint8_t eventIdentifier)
{
	this->communicator.publishEvent(eventIdentifier);
}

// ----------------------------------------------------------------------------
template<typename T>
void
xpcc::AbstractComponent::callAction(uint8_t receiver,
		uint8_t actionIdentifier, const T& data)
{
	this->communicator.callAction(receiver, actionIdentifier, data);
}

template<typename T>
void
xpcc::AbstractComponent::callAction(uint8_t receiver, uint8_t actionIdentifier,
		const T& data, ResponseCallback& responseCallback)
{
	this->communicator.callAction(receiver, actionIdentifier, data, responseCallback);
}

// ----------------------------------------------------------------------------
template<typename T>
void
xpcc::AbstractComponent::publishEvent(uint8_t eventIdentifier, const T& data)
{
	communicator.publishEvent(eventIdentifier, data);
}

// ----------------------------------------------------------------------------
void
xpcc::AbstractComponent::sendResponse(const ResponseHandle& handle)
{
	this->communicator.sendResponse(handle);
}

template<typename T>
void
xpcc::AbstractComponent::sendResponse(const ResponseHandle& handle, const T& data)
{
	this->communicator.sendResponse(handle, data);
}

void
xpcc::AbstractComponent::sendNegativeResponse(const ResponseHandle& handle)
{
	this->communicator.sendNegativeResponse(handle);
}

template<typename T>
void
xpcc::AbstractComponent::sendNegativeResponse(const ResponseHandle& handle, const T& data)
{
	this->communicator.sendNegativeResponse(handle, data);
}
