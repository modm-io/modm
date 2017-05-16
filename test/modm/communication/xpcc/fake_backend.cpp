/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012-2013, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "fake_backend.hpp"

void
FakeBackend::update()
{
}

// ----------------------------------------------------------------------------
void
FakeBackend::sendPacket(const xpcc::Header &header,
		modm::SmartPointer payload)
{
	this->messagesSend.append(Message(header, payload));
}

// ----------------------------------------------------------------------------
bool
FakeBackend::isPacketAvailable() const
{
	return !this->messagesToReceive.isEmpty();
}

const xpcc::Header&
FakeBackend::getPacketHeader() const
{
	return this->messagesToReceive.getFront().header;
}

const modm::SmartPointer
FakeBackend::getPacketPayload() const
{
	return this->messagesToReceive.getFront().payload;
}

void
FakeBackend::dropPacket()
{
	this->messagesToReceive.removeFront();
}
