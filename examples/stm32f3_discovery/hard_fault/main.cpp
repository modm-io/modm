/*
 * Copyright (c) 2011, Fabian Greif
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2013-2017, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	// simulate some stack usage
	asm volatile ("push {r0-r12}");
	asm volatile ("push {r0-r12}");
	asm volatile ("push {r0-r12}");
	asm volatile ("pop {r0-r12}");
	asm volatile ("pop {r0-r12}");
	asm volatile ("pop {r0-r12}");

	// divide by zero
	volatile uint8_t number = 42;
	volatile uint8_t divisor = 0;
	number /= divisor;

	// undefined instruction
	asm volatile (".short 0xde00");

	// stack overflow
	while(1) asm volatile ("push {r0-r12}");

	while (1)
	{
		modm::delayMilliseconds(250);
		Board::LedSouth::toggle();
	}

	return 0;
}
