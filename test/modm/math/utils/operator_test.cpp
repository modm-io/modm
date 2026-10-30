/*
 * Copyright (c) 2010-2011, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/math/utils/operator.hpp>
#include <cmath>

#include "operator_test.hpp"

static_assert(modm::math::sqrt(0) == 0);
static_assert(modm::math::sqrt(452613) == 673);
static_assert(modm::math::sqrt(4294800000UL) == 65535);
static_assert(modm::math::sqrt(0xffffffffUL) == 65535);

void
OperatorTest::testSqrt()
{
	TEST_ASSERT_EQUALS(modm::math::sqrt(0), 0U);
	TEST_ASSERT_EQUALS(modm::math::sqrt(1), 1U);
	TEST_ASSERT_EQUALS(modm::math::sqrt(10), 3U);
	TEST_ASSERT_EQUALS(modm::math::sqrt(100), 10U);
	TEST_ASSERT_EQUALS(modm::math::sqrt(1000), 32U);
	TEST_ASSERT_EQUALS(modm::math::sqrt(10000), 100U);
	TEST_ASSERT_EQUALS(modm::math::sqrt(100000), 316U);
	TEST_ASSERT_EQUALS(modm::math::sqrt(123456), 351U);
	TEST_ASSERT_EQUALS(modm::math::sqrt(452613), 673U);
	TEST_ASSERT_EQUALS(modm::math::sqrt(1000000), 1000U);
	TEST_ASSERT_EQUALS(modm::math::sqrt(10000000), 3162U);
	TEST_ASSERT_EQUALS(modm::math::sqrt(4000000000UL), 63246U);
	TEST_ASSERT_EQUALS(modm::math::sqrt(4294800000UL), 65535U);

#ifndef MODM_CPU_AVR
	// rounds to nearest like the floating point square root
	for (uint32_t x = 0; x < 0xfffe0000; x += (x < 70000) ? 1 : 0x10001)
	{
		TEST_ASSERT_EQUALS(modm::math::sqrt(x), uint16_t(std::lround(std::sqrt(double(x)))));
	}
#endif
}
