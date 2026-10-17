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
#include <cstring>

#include "../integration_test.hpp"

/**
 * Tests the initialization of function statics:
 * 1. A static is constructed exactly once.
 * 2. A recursive initialization cannot be resolved without fibers and must
 *    fail the `stat.rec` assertion instead of waiting forever.
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

static modm::Abandonment
recursion_handler(const modm::AssertionInfo &info)
{
#ifdef MODM_CPU_AVR
	// The assertion names are located in Flash on AVRs!
	if (strcmp_P("stat.rec", info.name) == 0)
#else
	if (strcmp(info.name, "stat.rec") == 0)
#endif
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

	// initialize the recursive static
	constructDummy();

	// the assertion did not fail
	return finishTest(false);
}
