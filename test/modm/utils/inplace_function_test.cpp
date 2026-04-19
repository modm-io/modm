/*
 * Copyright (c) 2026, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "inplace_function_test.hpp"
#include <modm/utils/inplace_function.hpp>

void
InplaceFunctionTest::testCall()
{
	int value = 0;

	modm::inplace_function<int(int), 8> func = [&value](int x) {
		value = x;
		return x * 2;
	};

	TEST_ASSERT_EQUALS(func(42), 84);
	TEST_ASSERT_EQUALS(value, 42);
}
