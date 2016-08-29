/* Copyright (c) 2016, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 * ------------------------------------------------------------------------- */

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
