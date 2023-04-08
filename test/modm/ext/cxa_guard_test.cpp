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

namespace
{
	int guard{0};

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
	TEST_ASSERT_EQUALS(guard, 0);

	int retval = __cxa_guard_acquire(&guard);
	TEST_ASSERT_EQUALS(retval, 1);
	TEST_ASSERT_EQUALS(guard, 0x100);

	__cxa_guard_release(&guard);
	TEST_ASSERT_EQUALS(guard, 1);

	retval = __cxa_guard_acquire(&guard);
	TEST_ASSERT_EQUALS(retval, 0);
	TEST_ASSERT_EQUALS(guard, 1);

	__cxa_guard_release(&guard);
	TEST_ASSERT_EQUALS(guard, 1);
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
