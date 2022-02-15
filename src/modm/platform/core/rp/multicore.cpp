/*
 * Copyright (c) 2022, Andrey Kunitsyn
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "multicore.hpp"
#include "../device.hpp"

using namespace modm::platform::multicore;

modm_weak void
modm_initialize_core1(void)
{}

static void
core1_wrapper(void (*entry)(void))
{
	modm_initialize_core1();
	(*entry)();
}

static void modm_naked core1_trampoline(void)
{
	asm volatile ("pop {r0, pc}");  // r0 = entry, pc = core1_wrapper
}

static void
runCore1(void (*entry)(), uint32_t *stack_bottom, uint32_t stack_size)
{
	// assert(!(stack_size & 3u));
	uint32_t *stack_ptr = stack_bottom + (stack_size / sizeof(uint32_t));
	// push 1 value onto top of stack for core1_trampoline
	stack_ptr -= 2;
	stack_ptr[0] = (uintptr_t)entry;
	stack_ptr[1] = (uintptr_t)core1_wrapper;

	// Allow for the fact that the caller may have already enabled the FIFO IRQ for their
	// own purposes (expecting FIFO content after core 1 is launched). We must disable
	// the IRQ during the handshake, then restore afterwards.
	const bool enabled = NVIC_GetEnableIRQ(SIO_IRQ_PROC0_IRQn);
	NVIC_DisableIRQ(SIO_IRQ_PROC0_IRQn);

	// Boot code start core 1 at waiting following protocol on Mailbox
	// Values to be sent in order over the FIFO from core 0 to core 1
	//
	// value for VTOR register
	// initial stack pointer (SP)
	// initial program counter (PC) (don't forget to set the thumb bit!)
	const uint32_t cmd_sequence[] = {
		0, 0, 1, (uintptr_t)SCB->VTOR, (uintptr_t)stack_ptr, (uintptr_t)core1_trampoline};

	uint32_t seq = 0;
	do {
		uint32_t cmd = cmd_sequence[seq];
		// Always drain the READ FIFO (from core 1) before sending a 0
		if (!cmd)
		{
			Mailbox::drain();
			// Execute a SEV as core 1 may be waiting for FIFO space via WFE
			__SEV();
		}
		Mailbox::pushBlocked(cmd);
		uint32_t response = Mailbox::popBlocked();
		// Move to next state on correct response (echo-d value) otherwise start over
		seq = cmd == response ? seq + 1 : 0;
	}
	while (seq < sizeof(cmd_sequence) / sizeof(cmd_sequence[0]));

	if (enabled) NVIC_EnableIRQ(SIO_IRQ_PROC0_IRQn);
	else NVIC_DisableIRQ(SIO_IRQ_PROC0_IRQn);
}

extern "C" uint32_t __stack1_start;
extern "C" uint32_t __main1_stack_top;

void
Core1::run(void (*func)())
{
	uint32_t *stack = &__main1_stack_top;
	runCore1(func, stack, (stack - &__stack1_start) * sizeof(uint32_t));
}
