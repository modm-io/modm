/*
 * Copyright (c) 2021, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>

// Blink LED from the window watchdog interrupt, which is triggered in software.
// A custom handler is configured at runtime in the vector table located in SRAM.

// If the LED is blinking with a period of 1 second, the vector table has been
// successfully relocated to RAM.
// Set the option "modm:platform:core:vector_table_location" in project.xml
// to place the vector table in RAM on F0 devices without vector table relocation
// support in the Cortex-M0 core.

static void
wwdgHandler()
{
	Board::Leds::toggle();
}

int
main()
{
	Board::initialize();
	Board::Leds::setOutput();

	// Set custom handler, only works if vector table is in RAM
	NVIC_SetVector(WWDG_IRQn, reinterpret_cast<uintptr_t>(&wwdgHandler));
	NVIC_EnableIRQ(WWDG_IRQn);

	uint32_t counter{0};
	while (true)
	{
		modm::delay(500ms);
		// Every STM32 has this interrupt, so we do not need a peripheral to trigger it
		NVIC_SetPendingIRQ(WWDG_IRQn);
#ifdef MODM_BOARD_HAS_LOGGER
		MODM_LOG_INFO << "loop: " << counter << modm::endl;
#endif
		counter++;
	}

	return 0;
}
