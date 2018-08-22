/*
 * Copyright (c) 2017, Sascha Schade
 * Copyright (c) 2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <modm/debug/logger.hpp>
#include <modm/processing/timer.hpp>

// ----------------------------------------------------------------------------
#ifndef MODM_BOARD_HAS_LOGGER
#define GENERIC_EXAMPLE_UART
#include "../connect.hpp"
// Create an IODeviceWrapper around the Uart Peripheral we want to use
modm::IODeviceWrapper< Generic::Uart::Instance, modm::IOBuffer::BlockIfFull > loggerDevice;
// Set all four logger streams to use the UART
modm::log::Logger modm::log::debug(loggerDevice);
modm::log::Logger modm::log::info(loggerDevice);
modm::log::Logger modm::log::warning(loggerDevice);
modm::log::Logger modm::log::error(loggerDevice);
#endif

// Set the log level
#undef  MODM_LOG_LEVEL
#define MODM_LOG_LEVEL modm::log::DEBUG

int main()
{
	Board::initialize();
#ifndef MODM_BOARD_HAS_LOGGER
    Generic::Uart::connect();
#endif

    // Use the logging streams to print some messages.
    // Change MODM_LOG_LEVEL above to enable or disable these messages
    MODM_LOG_DEBUG   << "debug"   << modm::endl;
    MODM_LOG_INFO    << "info"    << modm::endl;
    MODM_LOG_WARNING << "warning" << modm::endl;
    MODM_LOG_ERROR   << "error"   << modm::endl;

	Board::Leds::setOutput();

	uint32_t ii(1);

	while (true) {
        Board::Leds::set();
        MODM_LOG_DEBUG << "Setting LEDs" << modm::endl;
        modm::delayMilliseconds(100);

        Board::Leds::reset();
        MODM_LOG_DEBUG.printf("Resetting LEDs\n");
        modm::delayMilliseconds(900);

        ii++;
        if (ii < 100) {
            MODM_LOG_INFO << "Seconds since reboot: " << ii << modm::endl;
        } else if (ii < 300) {
            MODM_LOG_WARNING.printf("I've been sentient for %lu seconds!\n", ii);
        } else {
            MODM_LOG_ERROR << "Kill me!?" << modm::endl;
        }
	}
}
