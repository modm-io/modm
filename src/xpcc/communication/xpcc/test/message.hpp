/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2012-2013, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <modm/communication/communication.hpp>

struct Message
{
	Message(const xpcc::Header& header, const xpcc::SmartPointer& payload) :
		header(header), payload(payload)
	{
	}
	
	bool
	operator == (const Message& other) {
		return ((header == other.header) && (payload == other.payload));
	}
	
	xpcc::Header header;
	xpcc::SmartPointer payload;
};

#endif
