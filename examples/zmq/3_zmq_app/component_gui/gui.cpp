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

#include "communication/identifier.hpp"
#include "communication/packets.hpp"
#include "communication/communication.hpp"

#include "gui.hpp"

// set new log level
#undef XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::DEBUG

// ----------------------------------------------------------------------------
component::Gui::Gui(uint8_t id, xpcc::Dispatcher &communication) :
	xpcc::AbstractComponent(id, communication)
{
}


// ----------------------------------------------------------------------------
void
component::Gui::update()
{
}

// ----------------------------------------------------------------------------
void
component::Gui::eventRobotLocation(xpcc::Header /* header */, const robot::packet::Location * const location)
{
	XPCC_LOG_DEBUG << XPCC_FILE_INFO << *location << xpcc::endl;
	// XPCC_LOG_DEBUG.printf("Payload: x = %5d, y = %d, phi = %4.2f\n", location->x, location->y, location->phi);

	static bool positive = false;

	bool pos_new = (location->x > 0);
	if (positive xor pos_new)
	{
		XPCC_LOG_DEBUG << "State change to " << pos_new << xpcc::endl;

		positive = pos_new;
		robot::packet::Bool b(positive);

		robot::Odometry::setLedRed(getCommunicator(), b);
	}
}
