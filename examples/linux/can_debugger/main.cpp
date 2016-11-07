/*
 * Copyright (c) 2015-2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <xpcc/debug/logger.hpp>
#include <xpcc/architecture.hpp>

#include <xpcc/architecture/interface/can.hpp>
#include <xpcc/architecture/platform/driver/can/canusb/canusb.hpp>

#include <cstdlib>

/**
 * Simple example that listens to a CAN bus connected by a CAN2USB.
 *
 * How to use:
 * - Connect a CAN2USB to a CAN bus with traffic.
 * - Connect this CAN2USB by USB to your computer.
 * - Make sure the user can access the /dev/ttyUSB0 device.
 *   Brute force method is: #sudo chmod 777 /dev/ttyUSB0
 *   A wiser method is to add the user the appropriate group
 * - Adjust the baud rate of the CAN bus in this example.
 * - Do
 *   scons run
 * - All CAN messages on the bus should appear on the screen.
 */

static constexpr uint32_t canBusBaudRate = 125000;

xpcc::hosted::CanUsb canUsb;

int
main()
{
	if (not canUsb.open("/dev/ttyUSB0", canBusBaudRate)) {
		XPCC_LOG_ERROR << "Could not open port" << xpcc::endl;
		exit(EXIT_FAILURE);
	}

	while(true)
	{
		if (canUsb.isMessageAvailable())
		{
			xpcc::can::Message message;
			canUsb.getMessage(message);
			
			XPCC_LOG_DEBUG << message << xpcc::endl;
		}
	}

	canUsb.close();
}
