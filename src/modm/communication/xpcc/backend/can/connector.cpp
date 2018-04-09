/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2010, 2012, Fabian Greif
 * Copyright (c) 2012-2014, 2018, Niklas Hauser
 * Copyright (c) 2013, 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <stdlib.h>

#include "connector.hpp"

// ----------------------------------------------------------------------------
uint8_t xpcc::CanConnectorBase::messageCounter = 0;

// ----------------------------------------------------------------------------
uint32_t
xpcc::CanConnectorBase::convertToIdentifier(const Header & header,
		bool fragmentated)
{
	uint32_t identifier = 0;

	switch (header.type)
	{
		case xpcc::Header::Type::REQUEST:
			identifier = 0;
			break;
		case xpcc::Header::Type::RESPONSE:
			identifier = 1;
			break;
		case xpcc::Header::Type::NEGATIVE_RESPONSE:
			identifier = 2;
			break;
		case xpcc::Header::Type::TIMEOUT:
			identifier = 3;
			break;
	}

	identifier = identifier << 1;
	if (header.isAcknowledge){
		identifier |= 1;
	}
	identifier = identifier << 1;
	// Message counter
	identifier = identifier << 1;

	if (fragmentated){
		identifier |= 1;
	}
	identifier = identifier << 8;
	identifier |= header.destination;
	identifier = identifier << 8;
	identifier |= header.source;
	identifier = identifier << 8;
	identifier |= header.packetIdentifier;

	return identifier;
}

// ----------------------------------------------------------------------------
bool
xpcc::CanConnectorBase::convertToHeader(const uint32_t & identifier,
		xpcc::Header & header)
{
	const uint8_t *ptr = reinterpret_cast<const uint8_t *>(&identifier);

	header.packetIdentifier = ptr[0];
	header.source 			= ptr[1];
	header.destination		= ptr[2];

	uint8_t flags = ptr[3];

	if (flags & 0x04) {
		header.isAcknowledge = true;
	}
	else {
		header.isAcknowledge = false;
	}

	switch (flags & 0x18)
	{
		case 0x00:
			header.type = xpcc::Header::Type::REQUEST;
			break;
		case 0x08:
			header.type = xpcc::Header::Type::RESPONSE;
			break;
		case 0x10:
			header.type = xpcc::Header::Type::NEGATIVE_RESPONSE;
			break;
		case 0x18:
			header.type = xpcc::Header::Type::TIMEOUT;
			break;
		default:
			// unknown type
			//MODM_LOG_ERROR << "Unknown Type" << modm::flush;
			header.type = xpcc::Header::Type::REQUEST;
	}

	// check if the message is a fragment
	return isFragment(identifier);
}

// ----------------------------------------------------------------------------
uint8_t
xpcc::CanConnectorBase::getNumberOfFragments(uint8_t messageSize)
{
	div_t n = div(messageSize, 6);
	return (n.rem > 0) ? n.quot + 1 : n.quot;
}
