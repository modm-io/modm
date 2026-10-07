/*
 * Copyright (c) 2024, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/processing.hpp>
#include <modm/architecture/interface/assert.hpp>
#include <string.h>

#include "../integration_test.hpp"

using namespace std::chrono_literals;

/**
 * Tests that the scheduler detects a fiber that overflows its stack.
 * The test passes when the `fbr.stkof` assertion fails.
 */

static modm::Abandonment
overflow_handler(const modm::AssertionInfo &info)
{
#ifdef MODM_CPU_AVR
	// The assertion names are located in Flash on AVRs!
	if (strcmp_P("fbr.stkof", info.name) == 0)
#else
	if (strcmp(info.name, "fbr.stkof") == 0)
#endif
	{
		finishTest(true);
	}
	return modm::Abandonment::DontCare;
}
MODM_ASSERTION_HANDLER(overflow_handler);

modm::Fiber bad_fiber([]
{
	while(1)
	{
#ifdef MODM_CPU_AVR
		asm volatile ("push r1");
#else
		asm volatile ("push {r0-r7}");
#endif
		modm::this_fiber::yield();
	}
});

modm::Fiber watchdog([]
{
	// the overflow must have been detected long before
	modm::this_fiber::sleep_for(2s);
	finishTest(false);
});

int
main()
{
	Board::initialize();

	modm::fiber::Scheduler::run();

	return 0;
}
