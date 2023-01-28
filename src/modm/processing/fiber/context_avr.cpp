/*
 * Copyright (c) 2021, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "context.h"
#include <avr/io.h>

/* Stack layout (growing downwards):
 * Only need to save callee registers: r2-r17, r28-r29
 *
 * return address (2-3 bytes)
 * r2
 * r3
 * r4
 * r5
 * r6
 * r7
 * r8
 * r9
 * r10
 * r11
 * r12
 * r13
 * r14
 * r15
 * r16
 * r17
 * r28
 * r29
 */

static void modm_naked
modm_context_entry()
{
	// Load argument 1 and return to function
	asm volatile(
		"pop r25	\n\t"
		"pop r24	\n\t"
		"ret		\n\t"
	);
}

modm_context_t
modm_context_init(uintptr_t stack, uintptr_t arg, uintptr_t fn, uintptr_t end)
{
	uint8_t* sp = (uint8_t*)arg;

	// Return address for fiber ret
	*--sp = uint16_t(end);
	*--sp = uint16_t(end) >> 8;
#ifdef __AVR_3_BYTE_PC__
	*--sp = 0;
#endif
	// return address for modm_context_entry ret
	*--sp = uint16_t(fn);
	*--sp = uint16_t(fn) >> 8;
#ifdef __AVR_3_BYTE_PC__
	*--sp = 0;
#endif

	// Argument 1
	*--sp = uint16_t(arg);
	*--sp = uint16_t(arg) >> 8;

	// return address for jumpcontext ret
	*--sp = uint16_t(modm_context_entry);
	*--sp = uint16_t(modm_context_entry) >> 8;
#ifdef __AVR_3_BYTE_PC__
	*--sp = 0;
#endif

	// saved register context r2-r17, r28-r29, +2 for something unclear
	sp -= 19;

	return {(uintptr_t) sp, arg - stack};
}

static modm_context_t main_context;

void
modm_context_start(uintptr_t to)
{
	modm_context_jump(&(main_context.sp), to);
}

void modm_naked
modm_context_jump(uintptr_t*, uintptr_t)
{
	asm volatile
	(
		// Push callee-saved registers on stack
		".irp regs, 2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,28,29	\n\t"
		"push r\\regs 		\n\t"
		".endr 				\n\t"

		// Store the SP of current fiber
		"in	r18, __SP_L__	\n\t"
		"in	r19, __SP_H__	\n\t"
		"mov ZL, r24		\n\t"
		"mov ZH, r25		\n\t"
		"st  Z+, r18		\n\t"
		"st  Z,  r19		\n\t"

		// Save SREG and disable interrupts
		"in r18, __SREG__	\n\t"
		"cli				\n\t"

		// Load the SP of next fiber
		"out __SP_L__, r22	\n\t"
		"out __SP_H__, r23	\n\t"

		// Re-enable interrupts by restoring SREG
		"out __SREG__, r18	\n\t"

		// Pop callee-saved registers from stack
		".irp regs, 29,28,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2	\n\t"
		"pop r\\regs		\n\t"
		".endr				\n\t"

		"ret				\n\t"
	);
}

void
modm_context_end()
{
	uintptr_t dummy;
	modm_context_jump(&dummy, main_context.sp);
	__builtin_unreachable();
}
