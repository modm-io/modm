/*
 * Copyright (c) 2021, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/driver/usb/stusb4500.hpp>

using Sda = GpioB7;
using Scl = GpioB6;
using MyI2cMaster = I2cMaster1;

modm::Stusb4500<MyI2cMaster> usb{};

int
main()
{
	Board::initialize();

	MODM_LOG_INFO << "STM32F072 Discovery Example: USB-PD with STUSB4500 chip" << modm::endl;

	MyI2cMaster::connect<Scl::Scl, Sda::Sda>();
	MyI2cMaster::initialize<Board::SystemClock, 400_kHz>();

	Board::LedUp::set();

	usb.configurePdo(1, 5000, 500); // must be set to 5V according to USB standard
	usb.configurePdo(2, 20000, 1500); // 20V, 1.5A -> 30W
	usb.configurePdo(3, 20000, 4000); // 20V, 4A -> 80W

	usb.setValidPdo(3); // we prefer PDO 3 (more current)

	modm::delay(200ms);

	// check results
	modm::stusb4500::RdoRegStatusData status = usb.getRdoRegStatus();

	MODM_LOG_INFO << "Maximum current: " << status.MaxCurrent << " mA" << modm::endl;

	Board::LedDown::set();

	while (true) {
		Board::LedRight::toggle();
		modm::delay(1000ms);
	}
}
