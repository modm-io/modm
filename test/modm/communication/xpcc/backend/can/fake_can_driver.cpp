/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012-2014, Niklas Hauser
 * Copyright (c) 2015, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "fake_can_driver.hpp"

FakeCanDriver::FakeCanDriver() :
	sendSlots(0)
{
}

bool
FakeCanDriver::isMessageAvailable()
{
	return (not receiveList.isEmpty());
}

bool
FakeCanDriver::getMessage(modm::can::Message& message)
{
	if (isMessageAvailable())
	{
		message = receiveList.getFront();
		receiveList.removeFront();
		
		return true;
	}
	else {
		return false;
	}
}

bool
FakeCanDriver::isReadyToSend()
{
	return (this->sendSlots > 0);
}

bool
FakeCanDriver::sendMessage(const modm::can::Message& message)
{
	if (this->isReadyToSend())
	{
		this->sendList.append(message);
		this->sendSlots--;
		return true;
	}
	else {
		return false;
	}
}

uint8_t
FakeCanDriver::getReceiveErrorCounter()
{
	return 0;
}

uint8_t
FakeCanDriver::getTransmitErrorCounter()
{
	return 0;
}

modm::Can::BusState
FakeCanDriver::getBusState()
{
	return modm::Can::BusState::Connected;
}
