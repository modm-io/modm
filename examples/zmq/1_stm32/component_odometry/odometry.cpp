/*
 * Copyright (c) 2016, Sascha Schade
 * Copyright (c) 2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/debug/logger.hpp>

// set new log level
#undef MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::DISABLED

#include "communication/identifier.hpp"
#include "communication/packets.hpp"
#include "communication/communication.hpp"

#include "odometry.hpp"

#include <modm/board/board.hpp>

// ----------------------------------------------------------------------------
component::Odometry::Odometry(uint8_t id, xpcc::Dispatcher &communication) :
	xpcc::AbstractComponent(id, communication),
	timer(50)
{
}


// ----------------------------------------------------------------------------
void
component::Odometry::update()
{
	if (timer.execute())
	{
		MODM_LOG_INFO << MODM_FILE_INFO << "Odometry update" << modm::endl;

		robot::packet::Location location(Timer1::getValue(), 11, 0.5);

		robot::EventPublisher::robotLocation(getCommunicator(), location);
	}
}

void
component::Odometry::actionSetLedRed(
	const xpcc::ResponseHandle& responseHandle,
	const robot::packet::Bool *payload)
{
	GpioOutputB14::set(*payload);
	sendResponse(responseHandle);
}
