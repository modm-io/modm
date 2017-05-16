/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2010, Georgi Grinshpun
 * Copyright (c) 2012-2013, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <cstring>
#include "fake_io_device.hpp"

uint8_t FakeIODevice::sendBuffer[40];
uint8_t FakeIODevice::bytesSend = 0;

uint8_t FakeIODevice::receiveBuffer[40];
uint8_t FakeIODevice::receivePosition = 0;
uint8_t FakeIODevice::bytesReceived = 0;

void
FakeIODevice::setBaudrate(uint32_t)
{
}

void
FakeIODevice::write(uint8_t data)
{
	sendBuffer[bytesSend++] = data;
}

bool
FakeIODevice::read(uint8_t& byte)
{
	if (receivePosition >= bytesReceived) {
		return false;
	}
	else {
		byte = receiveBuffer[receivePosition++];
		return true;
	}
}


void
FakeIODevice::reset()
{
	bytesReceived = 0;
	receivePosition = 0;
	bytesSend = 0;
}

void
FakeIODevice::moveSendToReceiveBuffer()
{
	std::memcpy(receiveBuffer, sendBuffer, bytesSend);
	bytesReceived = bytesSend;
	receivePosition = 0;
	
	bytesSend = 0;
}
