/*
 * Copyright (c) 2010, Fabian Greif
 * Copyright (c) 2012-2013, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/math/utils/bit_operation.hpp>

#include "bit_operation_test.hpp"

void
BitOperationTest::testReverse8bit()
{
	uint8_t i, k;
	for (i = 1, k = 0x80; i != 0; i <<= 1, k >>= 1)
	{
		TEST_ASSERT_EQUALS(modm::bitReverse(i), k);
	}

	// test some random values
	TEST_ASSERT_EQUALS(modm::bitReverse(static_cast<uint8_t>(0xa3)), 0xc5U);
	TEST_ASSERT_EQUALS(modm::bitReverse(static_cast<uint8_t>(0x0f)), 0xf0U);
	TEST_ASSERT_EQUALS(modm::bitReverse(static_cast<uint8_t>(0xc1)), 0x83U);
}

void
BitOperationTest::testReverse16bit()
{
	uint16_t i, k;
	for (i = 1, k = 0x8000; i != 0; i <<= 1, k >>= 1)
	{
		TEST_ASSERT_EQUALS(modm::bitReverse(i), k);
	}

	// test some random values
	TEST_ASSERT_EQUALS(modm::bitReverse(static_cast<uint16_t>(0xa302)), 0x40c5U);
	TEST_ASSERT_EQUALS(modm::bitReverse(static_cast<uint16_t>(0x00f0)), 0x0f00U);
	TEST_ASSERT_EQUALS(modm::bitReverse(static_cast<uint16_t>(0xc176)), 0x6e83U);
}

void
BitOperationTest::testReverse32bit()
{
	uint32_t i, k;
	for (i = 1, k = 0x80000000; i != 0; i <<= 1, k >>= 1)
	{
		TEST_ASSERT_EQUALS(modm::bitReverse(i), k);
	}

	// test some random values
	TEST_ASSERT_EQUALS(modm::bitReverse(static_cast<uint32_t>(0xa3010ff0)), 0x0ff080c5U);
	TEST_ASSERT_EQUALS(modm::bitReverse(static_cast<uint32_t>(0xa3010ff0)), 0x0ff080c5U);
	TEST_ASSERT_EQUALS(modm::bitReverse(static_cast<uint32_t>(0x0f0c1760)), 0x06e830f0U);
	TEST_ASSERT_EQUALS(modm::bitReverse(static_cast<uint32_t>(0xc1040aaa)), 0x55502083U);
}
