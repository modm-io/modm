/*
 * Copyright (c) 2015-2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "board.hpp"
#include <modm/utils/bit_constants.hpp>

// Create an IODeviceWrapper around the Uart Peripheral we want to use
modm::IODeviceWrapper< Board::stlink::Uart, modm::IOBuffer::BlockIfFull > loggerDevice;

// Set all four logger streams to use the UART
modm::log::Logger modm::log::debug(loggerDevice);
modm::log::Logger modm::log::info(loggerDevice);
modm::log::Logger modm::log::warning(loggerDevice);
modm::log::Logger modm::log::error(loggerDevice);

extern void
board_initialize_sdram();

static void
modm_board_init(void)
{
	// Reset LCD
	Board::DisplayReset::setOutput(modm::Gpio::Low);
	modm::delayMilliseconds(20);
	Board::DisplayReset::set();
	modm::delayMilliseconds(10);

	// initialize system clock and external SDRAM before accessing external memories
	Board::systemClock::enable();
	board_initialize_sdram();
}

__attribute__((section(".hardware_init"), used))
static const uint32_t modm_board_init_ptr = (uint32_t) &modm_board_init;

modm_extern_c void
modm_abandon(const char * module,
			 const char * location,
			 const char * failure,
			 uintptr_t context)
{
	MODM_LOG_ERROR << "Assertion '" << module << "." << location << "." << failure << "'";
	if (context) { MODM_LOG_ERROR << " @ " << (void *) context << " (" << (uint32_t) context << ")"; }
	MODM_LOG_ERROR << " failed! Abandoning..." << modm::endl;

	Board::LedBlue::setOutput();
	while(1) {
		Board::LedBlue::set();
		modm::delayMilliseconds(20);
		Board::LedBlue::reset();
		modm::delayMilliseconds(180);
	}
}
