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

#include <xpcc/math/utils/operator.hpp>

#include "operator_test.hpp"

void
OperatorTest::testSqrt()
{
	TEST_ASSERT_EQUALS(xpcc::math::sqrt(0), 0U);
	TEST_ASSERT_EQUALS(xpcc::math::sqrt(1), 1U);
	TEST_ASSERT_EQUALS(xpcc::math::sqrt(10), 3U);
	TEST_ASSERT_EQUALS(xpcc::math::sqrt(100), 10U);
	TEST_ASSERT_EQUALS(xpcc::math::sqrt(1000), 32U);
	TEST_ASSERT_EQUALS(xpcc::math::sqrt(10000), 100U);
	TEST_ASSERT_EQUALS(xpcc::math::sqrt(100000), 316U);
	TEST_ASSERT_EQUALS(xpcc::math::sqrt(123456), 351U);
	TEST_ASSERT_EQUALS(xpcc::math::sqrt(452613), 673U);
	TEST_ASSERT_EQUALS(xpcc::math::sqrt(1000000), 1000U);
	TEST_ASSERT_EQUALS(xpcc::math::sqrt(10000000), 3162U);
	TEST_ASSERT_EQUALS(xpcc::math::sqrt(4000000000UL), 63246U);
	TEST_ASSERT_EQUALS(xpcc::math::sqrt(4294800000UL), 65535U);
}

void
OperatorTest::testMultiplication()
{
	TEST_ASSERT_EQUALS(xpcc::math::mul((uint16_t) 1, (uint16_t) 1), 1U);
	TEST_ASSERT_EQUALS(xpcc::math::mul((uint16_t) 10, (uint16_t) 10), 100U);
	TEST_ASSERT_EQUALS(xpcc::math::mul((uint16_t) 100, (uint16_t) 100), 10000U);
	TEST_ASSERT_EQUALS(xpcc::math::mul((uint16_t) 1000, (uint16_t) 1000), 1000000U);
	TEST_ASSERT_EQUALS(xpcc::math::mul((uint16_t) 10000, (uint16_t) 10000), 100000000U);
	TEST_ASSERT_EQUALS(xpcc::math::mul((uint16_t) 60000, (uint16_t) 60000), 3600000000U);
	
	TEST_ASSERT_EQUALS(xpcc::math::mul((int16_t) 1, (int16_t) 1), 1);
	TEST_ASSERT_EQUALS(xpcc::math::mul((int16_t) 10, (int16_t) 10), 100);
	TEST_ASSERT_EQUALS(xpcc::math::mul((int16_t) 100, (int16_t) 100), 10000);
	TEST_ASSERT_EQUALS(xpcc::math::mul((int16_t) 1000, (int16_t) 1000), 1000000);
	TEST_ASSERT_EQUALS(xpcc::math::mul((int16_t) 10000, (int16_t) 10000), 100000000);
	
	TEST_ASSERT_EQUALS(xpcc::math::mul((int16_t) -1, (int16_t) -1), 1);
	TEST_ASSERT_EQUALS(xpcc::math::mul((int16_t) -10, (int16_t) 10), -100);
	TEST_ASSERT_EQUALS(xpcc::math::mul((int16_t) 100, (int16_t) -100), -10000);
	TEST_ASSERT_EQUALS(xpcc::math::mul((int16_t) 1000, (int16_t) 1000), 1000000);
	TEST_ASSERT_EQUALS(xpcc::math::mul((int16_t) 32000, (int16_t) 32000), 1024000000);
	TEST_ASSERT_EQUALS(xpcc::math::mul((int16_t) -32000, (int16_t) -32000), 1024000000);
}

void
OperatorTest::testMultiplyAccumulate()
{
	int32_t offset = -2130;
	
	TEST_ASSERT_EQUALS(xpcc::math::mac(offset, (int16_t) 1, (int16_t) 1), 1 - 2130);
	TEST_ASSERT_EQUALS(xpcc::math::mac(offset, (int16_t) 10, (int16_t) 10), 100 - 2130);
	TEST_ASSERT_EQUALS(xpcc::math::mac(offset, (int16_t) 100, (int16_t) 100), 10000 - 2130);
	TEST_ASSERT_EQUALS(xpcc::math::mac(offset, (int16_t) 1000, (int16_t) 1000), 1000000 - 2130);
	TEST_ASSERT_EQUALS(xpcc::math::mac(offset, (int16_t) 10000, (int16_t) 10000), 100000000 - 2130);
	TEST_ASSERT_EQUALS(xpcc::math::mac(offset, (int16_t) -10000, (int16_t) -10000), 100000000 - 2130);
	
	offset = 1235678;
	
	TEST_ASSERT_EQUALS(xpcc::math::mac(offset, (int16_t) -1, (int16_t) -1), 1 + 1235678);
	TEST_ASSERT_EQUALS(xpcc::math::mac(offset, (int16_t) -10, (int16_t) 10), -100 + 1235678);
	TEST_ASSERT_EQUALS(xpcc::math::mac(offset, (int16_t) 100, (int16_t) -100), -10000 + 1235678);
	TEST_ASSERT_EQUALS(xpcc::math::mac(offset, (int16_t) 1000, (int16_t) 1000), 1000000 + 1235678);
	TEST_ASSERT_EQUALS(xpcc::math::mac(offset, (int16_t) -1000, (int16_t) 1000), -1000000 + 1235678);
	TEST_ASSERT_EQUALS(xpcc::math::mac(offset, (int16_t) 32000, (int16_t) 32000), 1024000000 + 1235678);
	TEST_ASSERT_EQUALS(xpcc::math::mac(offset, (int16_t) -32000, (int16_t) -32000), 1024000000 + 1235678);
	TEST_ASSERT_EQUALS(xpcc::math::mac(offset, (int16_t) -32000, (int16_t) 32000), -1024000000 + 1235678);
}
