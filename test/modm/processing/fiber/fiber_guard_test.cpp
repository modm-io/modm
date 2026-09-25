/*
 * Copyright (c) 2023, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "fiber_guard_test.hpp"
#include "shared.hpp"
#include <modm/architecture/detect.hpp>

extern "C"
{

#ifdef MODM_CPU_CORTEX_M
using guard_t = int32_t;
#else
using guard_t = int64_t;
#endif

#ifndef MODM_OS_HOSTED
int __cxa_guard_acquire(guard_t*);
void __cxa_guard_release(guard_t*);
void __cxa_guard_abort(guard_t*);

static guard_t guard{0};
#endif

}

void
FiberGuardTest::setUp()
{
	state = 0;
}

// ================================== GUARD ===================================
#ifndef MODM_OS_HOSTED
static void
f1()
{
	TEST_ASSERT_EQUALS(state++, 0u);
	TEST_ASSERT_EQUALS(guard, guard_t(0));

	TEST_ASSERT_EQUALS(__cxa_guard_acquire(&guard), 1);
	TEST_ASSERT_EQUALS(guard, guard_t(0x10));
	// now while initializing yield to another fiber
	modm::this_fiber::yield(); // goto 1

	TEST_ASSERT_EQUALS(state++, 2u);
	__cxa_guard_release(&guard);
	TEST_ASSERT_EQUALS(guard, guard_t(1));
}

static void
f2()
{
	TEST_ASSERT_EQUALS(state++, 1u);
	TEST_ASSERT_EQUALS(guard, guard_t(0x10));
	TEST_ASSERT_EQUALS(__cxa_guard_acquire(&guard), 0); // goto 2

	TEST_ASSERT_EQUALS(state++, 3u);
	TEST_ASSERT_EQUALS(guard, guard_t(1));
}
#endif

void
FiberGuardTest::testGuard()
{
#ifndef MODM_OS_HOSTED
	modm::fiber::Task fiber1(stack1, f1), fiber2(stack2, f2);
	modm::fiber::Scheduler::run();
#endif
}

// =============================== CONSTRUCTOR ================================
static uint8_t constructor_calls{0};
struct StaticClass
{
	uint8_t counter{0};
	StaticClass()
	{
		constructor_calls++;
	}

	void increment()
	{
		counter++;
	}
};

static StaticClass& instance()
{
	static StaticClass obj;
	return obj;
}

void
FiberGuardTest::testConstructor()
{
	TEST_ASSERT_EQUALS(constructor_calls, 0);

	instance().increment();
	TEST_ASSERT_EQUALS(constructor_calls, 1);
	TEST_ASSERT_EQUALS(instance().counter, 1);
	TEST_ASSERT_EQUALS(constructor_calls, 1);

	instance().increment();
	TEST_ASSERT_EQUALS(constructor_calls, 1);
	TEST_ASSERT_EQUALS(instance().counter, 2);
	TEST_ASSERT_EQUALS(constructor_calls, 1);

	instance().increment();
	TEST_ASSERT_EQUALS(constructor_calls, 1);
	TEST_ASSERT_EQUALS(instance().counter, 3);
	TEST_ASSERT_EQUALS(constructor_calls, 1);
}
