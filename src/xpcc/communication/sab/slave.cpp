/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2009-2010, Martin Rosekeit
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

#include "slave.hpp"

xpcc::sab::Response::Response(Transmitter *parent) :
	transmitter(parent), triggered(false)
{
}

void
xpcc::sab::Response::error(uint8_t errorCode)
{
	triggered = true;
	
	uint8_t tempError = errorCode;
	transmitter->send(false, &tempError, 1);
}

void
xpcc::sab::Response::send()
{
	triggered = true;
	transmitter->send(true, 0, 0);
}

void
xpcc::sab::Response::send(const void *payload, std::size_t length)
{
	triggered = true;
	transmitter->send(true, payload, length);
}
