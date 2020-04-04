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
	modm::delay_ms(20);
	Board::DisplayReset::set();
	modm::delay_ms(10);

	// initialize system clock and external SDRAM before accessing external memories
	Board::SystemClock::enable();
	board_initialize_sdram();
}

MODM_HARDWARE_INIT(modm_board_init);
