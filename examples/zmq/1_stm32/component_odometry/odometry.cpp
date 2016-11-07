/*
 * Copyright (c) 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <xpcc/debug/logger.hpp>

// set new log level
#undef XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::DISABLED

#include "communication/identifier.hpp"
#include "communication/packets.hpp"
#include "communication/communication.hpp"

#include "odometry.hpp"

#include <xpcc/architecture/platform.hpp>

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
		XPCC_LOG_INFO << XPCC_FILE_INFO << "Odometry update" << xpcc::endl;

		robot::packet::Location location(xpcc::stm32::Timer1::getValue(), 11, 0.5);

		robot::EventPublisher::robotLocation(getCommunicator(), location);
	}
}

void
component::Odometry::actionSetLedRed(
	const xpcc::ResponseHandle& responseHandle,
	const robot::packet::Bool *payload)
{
	xpcc::stm32::GpioOutputB14::set(*payload);
	sendResponse(responseHandle);
}
