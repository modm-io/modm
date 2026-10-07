/*
 * Copyright (c) 2020, Niklas Hauser
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
#include <modm/architecture/interface/interrupt.hpp>
#include <cstring>

#include "../integration_test.hpp"

/**
 * Tests the initialization of function statics:
 * 1. A static is constructed exactly once.
 * 2. A recursive initialization inside an interrupt cannot be resolved and
 *    must fail the `stat.rec` assertion.
 */

static uint8_t constructed{0};
struct Counter
{
	Counter() { constructed++; }
};

static void
constructCounter()
{
	static Counter counter;
}

void constructDummy();
struct Dummy
{
	Dummy()
	{
		MODM_LOG_INFO << "Dummy class constructed" << modm::endl;
		constructDummy(); // recursive initialization
	}
};

void
constructDummy()
{
	static Dummy dummy;
}

MODM_ISR(EXTI0)
{
	constructDummy();
}

static modm::Abandonment
recursion_handler(const modm::AssertionInfo &info)
{
	if (strcmp(info.name, "stat.rec") == 0)
	{
		finishTest(constructed == 1);
	}
	return modm::Abandonment::DontCare;
}
MODM_ASSERTION_HANDLER(recursion_handler);

int
main()
{
	Board::initialize();

	constructCounter();
	constructCounter();
	constructCounter();

	// initialize the recursive static inside an interrupt
	NVIC_EnableIRQ(EXTI0_IRQn);
	NVIC_SetPendingIRQ(EXTI0_IRQn);
	modm::delay(100ms);

	// the assertion did not fail
	return finishTest(false);
}
