/*
 * Copyright (c) 2016-2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>

/* This example showcases device resetting on hard fault encounter.
 * You can trigger the hard fault by pressing the blue button, which will execute an undefined instruction.
 *
 * Connecting a debugger changes this behavior: the hardfault will now trigger a breakpoint.
 * This allows normal debugging of the failure reason.
 * The red LED indicated when the debugger has been recognized.
 * Pressing the blue button now, will make GDB jump to the breakpoint.
 */

using namespace Board;

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	Leds::setOutput();

	for (uint16_t i=0; i < 12; i++)
	{
		Leds::write(1 << (i % 4));
		modm::delayMilliseconds(100);
	}

	if (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk) {
		// if this LED is on, the debugger is connected
		LedRed::set();
	}

	modm::delayMilliseconds(1000);

	while (1)
	{
		LedGreen::toggle();
		LedOrange::toggle();

		if (Button::read()) {
			// execute undefined instruction
			// the hard fault handler will reset the device
			// or, if the debugger is connected, will trigger a breakpoint
			asm volatile (".short 0xde00");
		}

		modm::delayMilliseconds(500);
	}

	return 0;
}
