/*
 * Copyright (c) 2016-2017, Niklas Hauser
 * Copyright (c) 2017, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/architecture/interface/assert.hpp>
#include <string.h>
#include <stdlib.h>

#include "../integration_test.hpp"

/**
 * Tests that assertions call their handlers, that handlers can ignore
 * assertions and that the core reports its errors as assertions.
 * The test passes when the last assertion abandons execution after every
 * expected assertion was seen exactly once.
 */

static bool
is(const char *name, const char *expected)
{
#ifdef MODM_CPU_AVR
	// The assertion names are located in Flash on AVRs!
	return strcmp_P(expected, name) == 0;
#else
	return strcmp(name, expected) == 0;
#endif
}

static uint8_t seen_io, seen_uart, seen_irq, seen_other;
static bool allocations_failed{true};

static modm::Abandonment
counting_handler(const modm::AssertionInfo &info)
{
	if (is(info.name, "io.tx")) { seen_io++; return modm::Abandonment::Ignore; }
	if (is(info.name, "uart.init")) { seen_uart++; return modm::Abandonment::Ignore; }
	if (is(info.name, "nvic.undef")) { seen_irq++; return modm::Abandonment::Ignore; }
	// only debug builds report a failed malloc
	if (is(info.name, "malloc")) { return modm::Abandonment::Ignore; }
	if (is(info.name, "can.init"))
	{
#ifdef MODM_CPU_CORTEX_M
		constexpr uint8_t expected_irq = 1;
#else
		constexpr uint8_t expected_irq = 0;
#endif
		MODM_LOG_INFO << "Assertions: io=" << seen_io << " uart=" << seen_uart << " irq=" << seen_irq
					  << " other=" << seen_other << ", allocations failed: " << allocations_failed << modm::endl;
		// does not return on microcontrollers
		exit(finishTest(seen_io == 1 and seen_uart == 1 and seen_irq == expected_irq and
						seen_other == 0 and allocations_failed));
	}
	seen_other++;
	return modm::Abandonment::DontCare;
}
MODM_ASSERTION_HANDLER(counting_handler);

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	// these fail, but are ignored by our handler
	modm_assert_continue_fail(false, "io.tx", "IO transmit buffer is full!");
	modm_assert_continue_fail(false, "uart.init", "UART init failed!");

#ifdef MODM_CPU_CORTEX_M
	// trigger an IRQ with undefined handler
	NVIC_EnableIRQ(IRQn_Type(0));
	NVIC_SetPendingIRQ(IRQn_Type(0));

	// an out of memory returns a nullptr: we definitely don't have 32MB RAM
	void *volatile ptr = malloc(1 << 25);
	allocations_failed &= (ptr == nullptr);
	ptr = new (std::nothrow) uint8_t[1 << 25];
	allocations_failed &= (ptr == nullptr);
#endif

	// does not fail, should not be optimized away
	volatile bool true_condition = true;
	modm_assert(true_condition, "can.init", "CAN init timed out!");

	// always fails: our handler reports the result
	modm_assert(false, "can.init", "CAN init timed out!");

	// we should not get here
	return finishTest(false);
}
