/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2011, Georgi Grinshpun
 * Copyright (c) 2012-2013, Niklas Hauser
 * Copyright (c) 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_COMMUNICATOR_HPP
	#error	"Don't include this file directly, use 'communicator.hpp' instead"
#endif


// ----------------------------------------------------------------------------
template<typename T>
void
xpcc::Communicator::callAction(uint8_t receiver,
		uint8_t actionIdentifier, const T& data)
{
	Header header(Header::Type::REQUEST, false,
			receiver,
			this->ownIdentifier,
			actionIdentifier);

	modm::SmartPointer payload(&data);

	this->dispatcher.addMessage(header, payload);
}

// ----------------------------------------------------------------------------
template<typename T>
void
xpcc::Communicator::callAction(uint8_t receiver, uint8_t actionIdentifier,
		const T& data, ResponseCallback& responseCallback)
{
	Header header(Header::Type::REQUEST, false,
			receiver,
			this->ownIdentifier,
			actionIdentifier);

	modm::SmartPointer payload(&data);

	this->dispatcher.addMessage(header, payload, responseCallback);
}

// ----------------------------------------------------------------------------
template<typename T>
void
xpcc::Communicator::publishEvent(uint8_t eventIdentifier, const T& data)
{
	Header header(Header::Type::REQUEST, false,
			0,
			this->ownIdentifier,
			eventIdentifier);

	modm::SmartPointer payload(&data);	// no metadata is sent with Events
	this->dispatcher.addMessage(header, payload);
}

// ----------------------------------------------------------------------------
template<typename T>
void
xpcc::Communicator::sendResponse(const ResponseHandle& handle, const T& data)
{
	Header header(Header::Type::RESPONSE, false,
			handle.destination,
			this->ownIdentifier,
			handle.packetIdentifier);

	modm::SmartPointer payload(&data);
	this->dispatcher.addResponse(header, payload);
}

template<typename T>
void
xpcc::Communicator::sendNegativeResponse(const ResponseHandle& handle, const T& data)
{
	Header header(Header::Type::NEGATIVE_RESPONSE, false,
			handle.destination,
			this->ownIdentifier,
			handle.packetIdentifier);

	modm::SmartPointer payload(&data);
	this->dispatcher.addResponse(header, payload);
}
