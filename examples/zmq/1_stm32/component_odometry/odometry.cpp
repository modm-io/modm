/* Copyright (c) 2016, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 * ------------------------------------------------------------------------- */

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
