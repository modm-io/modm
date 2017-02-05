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

extern void
board_initialize_sdram();

void
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

modm_section(".hardware_init.modm_board_init")
uint32_t modm_board_init_ptr = (uint32_t) &modm_board_init;
