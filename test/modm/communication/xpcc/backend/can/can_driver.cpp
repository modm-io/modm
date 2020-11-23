/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012-2014, Niklas Hauser
 * Copyright (c) 2015-2020, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "can_driver.hpp"

modm_test::platform::CanDriver::CanDriver() :
	sendSlots(0)
{
}

bool
modm_test::platform::CanDriver::isMessageAvailable()
{
	return (not receiveList.isEmpty());
}

bool
modm_test::platform::CanDriver::getMessage(modm::can::Message& message)
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
modm_test::platform::CanDriver::isReadyToSend()
{
	return (this->sendSlots > 0);
}

bool
modm_test::platform::CanDriver::sendMessage(const modm::can::Message& message)
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
modm_test::platform::CanDriver::getReceiveErrorCounter()
{
	return 0;
}

uint8_t
modm_test::platform::CanDriver::getTransmitErrorCounter()
{
	return 0;
}

modm::Can::BusState
modm_test::platform::CanDriver::getBusState()
{
	return modm::Can::BusState::Connected;
}
