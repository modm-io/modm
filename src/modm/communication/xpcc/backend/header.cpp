/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009, Thorsten Lajewski
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012-2013, Niklas Hauser
 * Copyright (c) 2013, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "header.hpp"

// ----------------------------------------------------------------------------
bool
xpcc::Header::operator == (const Header& other) const
{
	return ((this->type == other.type) &&
			(this->isAcknowledge == other.isAcknowledge) &&
			(this->destination == other.destination) &&
			(this->source == other.source) &&
			(this->packetIdentifier == other.packetIdentifier));
}

// ----------------------------------------------------------------------------
modm::IOStream&
modm::operator << (modm::IOStream& s, const xpcc::Header& header)
{
	s	<< "(t=" << static_cast<int32_t>(header.type)
		<< ",a=" << header.isAcknowledge
		<< ",d=" << modm::hex << header.destination << modm::ascii
		<< ",s=" << modm::hex << header.source << modm::ascii
		<< ",i=" << modm::hex << header.packetIdentifier << modm::ascii << ")";

	return s;
}
