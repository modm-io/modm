/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2011, Georgi Grinshpun
 * Copyright (c) 2012-2013, Niklas Hauser
 * Copyright (c) 2013, 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "communicator.hpp"

// ----------------------------------------------------------------------------
xpcc::Communicator::Communicator(
		const uint8_t ownIdentifier,
		Dispatcher &inDispatcher) :
	ownIdentifier(ownIdentifier), dispatcher(inDispatcher)
{
}

// ----------------------------------------------------------------------------
void
xpcc::Communicator::callAction(uint8_t receiver, uint8_t actionIdentifier)
{
	Header header(Header::Type::REQUEST, false,
			receiver,
			this->ownIdentifier,
			actionIdentifier);

	modm::SmartPointer payload;
	this->dispatcher.addMessage(header, payload);
}

void
xpcc::Communicator::callAction(uint8_t receiver, uint8_t actionIdentifier, ResponseCallback& responseCallback)
{
	Header header(Header::Type::REQUEST, false,
			receiver,
			this->ownIdentifier,
			actionIdentifier);

	modm::SmartPointer payload;
	this->dispatcher.addMessage(header, payload, responseCallback);
}

// ----------------------------------------------------------------------------
void
xpcc::Communicator::publishEvent(uint8_t eventIdentifier)
{
	Header header(Header::Type::REQUEST, false,
			0,
			this->ownIdentifier,
			eventIdentifier);

	modm::SmartPointer payload;
	this->dispatcher.addMessage(header, payload);
}

// ----------------------------------------------------------------------------
void
xpcc::Communicator::sendResponse(const ResponseHandle& handle)
{
	Header header(Header::Type::RESPONSE, false,
			handle.destination,
			this->ownIdentifier,
			handle.packetIdentifier);

	modm::SmartPointer payload;
	this->dispatcher.addResponse(header, payload);
}

void
xpcc::Communicator::sendNegativeResponse(const ResponseHandle& handle)
{
	Header header(Header::Type::NEGATIVE_RESPONSE, false,
			handle.destination,
			this->ownIdentifier,
			handle.packetIdentifier);

	modm::SmartPointer payload;
	this->dispatcher.addResponse(header, payload);
}
