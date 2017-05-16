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

#include <modm/debug/logger.hpp>
#include <modm/architecture/architecture.hpp>

#include <modm/architecture/interface/can.hpp>
#include <modm/architecture/platform/driver/can/canusb/canusb.hpp>

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

static constexpr modm::Can::Bitrate canBusBitRate = modm::Can::kBps125;

modm::hosted::SerialInterface port("/dev/ttyUSB0", 115200);
modm::hosted::CanUsb<modm::hosted::SerialInterface> canUsb(port);

int
main()
{
	if (not canUsb.open(canBusBitRate)) {
		MODM_LOG_ERROR << "Could not connect to CAN2USB port" << modm::endl;
		exit(EXIT_FAILURE);
	}

	while(true)
	{
		if (canUsb.isMessageAvailable())
		{
			modm::can::Message message;
			canUsb.getMessage(message);
			
			MODM_LOG_DEBUG << message << modm::endl;
		}
	}

	canUsb.close();
}
