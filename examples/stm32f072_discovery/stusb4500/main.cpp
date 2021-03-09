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
#include <modm/debug/logger.hpp>
#include <modm/driver/usb/stusb4500.hpp>

modm::IODeviceWrapper< Usart1, modm::IOBuffer::BlockIfFull > loggerDevice;
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::INFO
modm::log::Logger modm::log::debug(loggerDevice);
modm::log::Logger modm::log::info(loggerDevice);
modm::log::Logger modm::log::warning(loggerDevice);
modm::log::Logger modm::log::error(loggerDevice);

using Sda = GpioB7;
using Scl = GpioB6;
using MyI2cMaster = I2cMaster1;

modm::Stusb4500<MyI2cMaster> usb{};

int
main()
{
	Board::initialize();

	// Enable USART 1 (TX only)
	Usart1::connect<GpioOutputA9::Tx>();
	Usart1::initialize<Board::SystemClock, 115200_Bd>();

	MODM_LOG_INFO << "STM32F072 Discovery Example: USB-PD with STUSB4500 chip" << modm::endl;

	MyI2cMaster::connect<Scl::Scl, Sda::Sda>();
	MyI2cMaster::initialize<Board::SystemClock, 400_kHz>();

	Board::LedUp::set();

	RF_CALL_BLOCKING(usb.configurePdo(1, 5000, 500)); // must be set to 5V according to USB standard
	RF_CALL_BLOCKING(usb.configurePdo(2, 20000, 1500)); // 20V, 1.5A -> 30W
	RF_CALL_BLOCKING(usb.configurePdo(3, 20000, 4000)); // 20V, 4A -> 80W

	RF_CALL_BLOCKING(usb.setValidPdo(3)); // we prefer PDO 3 (more current)

	modm::delay(200ms);

	// check results
	modm::stusb4500::RdoRegStatusData status = RF_CALL_BLOCKING(usb.getRdoRegStatus());

	MODM_LOG_INFO << "Maximum current: " << status.MaxCurrent << " mA" << modm::endl;

	Board::LedDown::set();

	while (true) {
		Board::LedRight::toggle();
		modm::delay(1000ms);
	}
}
