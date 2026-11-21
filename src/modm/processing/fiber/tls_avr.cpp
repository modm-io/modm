/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "scheduler.hpp"
#include <modm/architecture/interface/assert.hpp>
#include <avr/io.h>
#include <string.h>

// AVR has no native TLS, so GCC calls __emutls_get_address() with a control
// object for every thread_local variable, which is placed in a section named
// `.data.__emutls_v.<name>`. The linker sorts input sections by name, so that
// these objects are placed in a table between the `.data.__emutls_u` and
// `.data.__emutls_w` sections. Each variable gets a fixed offset into one
// block of storage, which is located below the closure on each fiber stack
// and at the top of the main stack for the main context.

namespace
{
// GCC uses the word mode for size and alignment, which is 8-bit on AVR, so
// thread_local variables larger than 255B are not supported!
struct EmutlsObject
{
	uint8_t size;
	uint8_t align;
	uint16_t offset;
	const void *templ;
};
static_assert(sizeof(EmutlsObject) == 6);

uint16_t tls_size;
uint16_t tls_mask;
uint8_t *tls_main;

// The block must be aligned to the largest alignment of all variables
uint8_t*
tls_block(uint8_t *top)
{
	return (uint8_t*)((uintptr_t(top) - tls_size) & ~tls_mask);
}
}

extern "C"
{
// Mark the start and end of the table of control objects
modm_section(".data.__emutls_u") EmutlsObject __emutls_start[0];
modm_section(".data.__emutls_w") EmutlsObject __emutls_end[0];

// It is `used`, since LTO cannot see the calls the compiler generates.
void* modm_used
__emutls_get_address(void *object)
{
	// The context is the first member of the task, see Task::tls_reserve()
	const auto *task = (const modm_context_t*) modm::fiber::Scheduler::main.current;
	uint8_t *block = task ? (uint8_t*) task->top : tls_main;
	return block + ((const EmutlsObject*) object)->offset;
}

// Called once at startup, returns the TLS block at the top of the main stack
uint8_t* modm_used
modm_context_tls_startup()
{
	uint16_t offset{0};
	for (auto *object = __emutls_start; object < __emutls_end; object++)
	{
		const uint16_t mask = object->align - 1u;
		offset = (offset + mask) & ~mask;
		object->offset = offset;
		offset += object->size;
		tls_mask |= mask;
	}
	tls_size = offset;
	tls_main = tls_block((uint8_t*)(RAMEND + 1));
	return tls_main;
}

uintptr_t*
modm_context_tls_reserve(uintptr_t *bottom, uintptr_t *top)
{
	top = (uintptr_t*) tls_block((uint8_t*) top);
	modm_assert(uintptr_t(top) - uintptr_t(bottom) >= modm::fiber::StackSizeMinimum, "fbr.tls",
			"Stack size must be larger than minimum stack size + thread-local storage!");
	return top;
}

void modm_used
modm_context_tls_init(uintptr_t *top)
{
	for (const auto *object = __emutls_start; object < __emutls_end; object++)
	{
		uint8_t *data = (uint8_t*) top + object->offset;
		if (object->templ) memcpy(data, object->templ, object->size);
		else memset(data, 0, object->size);
	}
}

// Runs after .data and .bss are initialized and before the constructors to
// reserve the main TLS block at the top of the stack, which is still empty.
void modm_naked modm_section(".init5")
modm_context_tls_startup_init5()
{
	asm volatile
	(
		"ldi r30, lo8(gs(modm_context_tls_startup))	\n\t"
		"ldi r31, hi8(gs(modm_context_tls_startup))	\n\t"
		"icall					\n\t"	// r25:r24 = main TLS block
		"movw r30, r24			\n\t"
		"sbiw r30, 1			\n\t"
		"out __SP_H__, r31		\n\t"	// Move the stack below the block
		"out __SP_L__, r30		\n\t"
		"ldi r30, lo8(gs(modm_context_tls_init))	\n\t"
		"ldi r31, hi8(gs(modm_context_tls_init))	\n\t"
		"icall					\n\t"	// Initialize the main TLS block
	);
}
}
