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

int
main()
{
	Board::initialize();

	// simulate some stack usage
	asm volatile ("push {r0-r7}");
	asm volatile ("push {r0-r7}");
	asm volatile ("push {r0-r7}");
	asm volatile ("pop {r0-r7}");
	asm volatile ("pop {r0-r7}");
	asm volatile ("pop {r0-r7}");

	// load some patterns into r0-r7
	asm volatile ("ldr r0, =0xA000000A");
	asm volatile ("ldr r1, =0x0AAAAAA0");
	asm volatile ("ldr r2, =0x00A00A00");
	asm volatile ("ldr r3, =0x000AA000");
	asm volatile ("ldr r4, =0x04040404");
	asm volatile ("ldr r5, =0x05050505");
	asm volatile ("ldr r6, =0x06060606");
	asm volatile ("ldr r7, =0x07070707");

	// undefined instruction
	asm volatile (".short 0xde00");

	// stack overflow
	while(1) asm volatile ("push {r0-r7}");

	while (1)
	{
		modm::delayMilliseconds(1000);
		Board::LedUp::toggle();
	}

	return 0;
}
