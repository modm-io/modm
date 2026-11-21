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

#include "cxa_guard_test.hpp"
#include <modm/architecture/detect.hpp>


extern "C"
{

#ifdef MODM_CPU_CORTEX_M
using guard_t = int32_t;
#else
using guard_t = int64_t;
#endif

int __cxa_guard_acquire(guard_t*);
void __cxa_guard_release(guard_t*);
void __cxa_guard_abort(guard_t*);

}


namespace
{
	guard_t guard{0};

	uint8_t constructor_calls{0};
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

	StaticClass& instance()
	{
		static StaticClass obj;
		return obj;
	}
}

void
CxaGuardTest::testGuard()
{
	TEST_ASSERT_EQUALS(guard, guard_t(0));

	TEST_ASSERT_EQUALS(__cxa_guard_acquire(&guard), 1);
#ifndef MODM_OS_HOSTED
	TEST_ASSERT_EQUALS(guard, guard_t(0x10));
#endif

	__cxa_guard_abort(&guard);
	TEST_ASSERT_EQUALS(guard, guard_t(0));

	TEST_ASSERT_EQUALS(__cxa_guard_acquire(&guard), 1);
#ifndef MODM_OS_HOSTED
	TEST_ASSERT_EQUALS(guard, guard_t(0x10));
#endif

	__cxa_guard_release(&guard);
	TEST_ASSERT_EQUALS(uint8_t(guard), uint8_t(1)); // libc++abi uses the upper bytes

	TEST_ASSERT_EQUALS(__cxa_guard_acquire(&guard), 0);
	TEST_ASSERT_EQUALS(uint8_t(guard), uint8_t(1)); // libc++abi uses the upper bytes

	__cxa_guard_release(&guard);
	TEST_ASSERT_EQUALS(uint8_t(guard), uint8_t(1)); // libc++abi uses the upper bytes
}

void
CxaGuardTest::testConstructor()
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
