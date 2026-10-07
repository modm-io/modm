/*
 * Copyright (c) 2011, Fabian Greif
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2013-2017, Niklas Hauser
 * Copyright (c) 2014, 2016, Sascha Schade
 * Copyright (c) 2022, Andrey Kunitsyn
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#if __has_include(<modm/board.hpp>)
#include <modm/board.hpp>
#else
#include <modm/platform.hpp>
#endif
#include <modm/debug/logger.hpp>

// ----------------------------------------------------------------------------
// Set the log level
#undef MODM_LOG_LEVEL
#define MODM_LOG_LEVEL modm::log::DEBUG

#if not defined MODM_BOARD_HAS_LOGGER and not defined MODM_OS_HOSTED
// This board does not provide a logger, so we need to create our own:
// Create an IODeviceWrapper around the Uart Peripheral we want to use
modm::IODeviceWrapper<Uart0, modm::IOBuffer::BlockIfFull> loggerDevice;

// Set all four logger streams to use the UART
modm::log::Logger modm::log::debug(loggerDevice);
modm::log::Logger modm::log::info(loggerDevice);
modm::log::Logger modm::log::warning(loggerDevice);
modm::log::Logger modm::log::error(loggerDevice);
#define CUSTOM_LOGGER
#endif

// ----------------------------------------------------------------------------
int
main()
{
#if __has_include(<modm/board.hpp>)
	Board::initialize();
#endif
#ifdef CUSTOM_LOGGER
	// initialize Uart0 for MODM_LOG_*
	Uart0::connect<GpioOutput0::Tx>();
	Uart0::initialize<Board::SystemClock, 115200_Bd>();
#endif

	// Use the logging streams to print some messages.
	// Change MODM_LOG_LEVEL above to enable or disable these messages
	MODM_LOG_DEBUG   << MODM_FILE_INFO << "debug"   << modm::endl;
	MODM_LOG_INFO    << MODM_FILE_INFO << "info"    << modm::endl;
	MODM_LOG_WARNING << MODM_FILE_INFO << "warning" << modm::endl;
	MODM_LOG_ERROR   << MODM_FILE_INFO << "error"   << modm::endl;

#if __has_include(<modm/board.hpp>)
	uint32_t uptime{};
	while (true)
	{
		Board::Leds::toggle();
		modm::delay(1s);
		MODM_LOG_INFO << "Seconds since reboot: " << ++uptime << modm::endl;
	}
#endif
	return 0;
}
