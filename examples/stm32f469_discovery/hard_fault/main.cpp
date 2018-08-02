/*
 * Copyright (c) 2016-2018, Niklas Hauser
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

using namespace Board;

extern "C" void
modm_hardfault_coredump_start()
{
	Leds::setOutput();
	Leds::write(0b1111);
}

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	LedOrange::set();

	MODM_LOG_INFO << "Press Button to cause a Hardfault!" << modm::endl;

	if (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk) {
		// if this LED is on, the debugger is connected
		LedRed::set();
		MODM_LOG_INFO << "Debugger connected!" << modm::endl;
	}

	while (1)
	{
		LedGreen::toggle();
		LedOrange::toggle();

		if (Button::read()) {
			// execute undefined instructed
			// the hard fault handler will blink the blue LED
			// or, if the debugger is connected, will trigger a breakpoint
			asm volatile (".short 0xde00");
		}

		modm::delayMilliseconds(500);

	}

	return 0;
}
