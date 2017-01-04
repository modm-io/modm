/*
 * Copyright (c) 2009-2010, 2012, Fabian Greif
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2012, 2014-2015, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/processing/timer/timestamp.hpp>
#include <modm/utils/arithmetic_traits.hpp>

#include "timestamp_test.hpp"

// ----------------------------------------------------------------------------
void
TimestampTest::testConstructors()
{
	xpcc::ShortTimestamp t1Short;
	TEST_ASSERT_TRUE(t1Short == 0);

	xpcc::Timestamp t1;
	TEST_ASSERT_TRUE(t1 == 0);

	xpcc::ShortTimestamp t2Short(1000);
	TEST_ASSERT_TRUE(t2Short == 1000);

	xpcc::Timestamp t2(1000);
	TEST_ASSERT_TRUE(t2 == 1000);

	t1Short = 500;
	TEST_ASSERT_TRUE(t1Short == 500);

	t1 = 500;
	TEST_ASSERT_TRUE(t1 == 500);
}

void
TimestampTest::testArithmetics()
{
	xpcc::ShortTimestamp t1Short(200);
	xpcc::ShortTimestamp t2Short(500);
	xpcc::ShortTimestamp t3Short;

	t3Short = t1Short + t2Short;
	TEST_ASSERT_TRUE(t3Short == 700);

	xpcc::Timestamp t1(200);
	xpcc::Timestamp t2(500);
	xpcc::Timestamp t3;

	t3 = t1 + t2;
	TEST_ASSERT_TRUE(t3 == 700);
}

void
TimestampTest::testComparisons()
{
	xpcc::ShortTimestamp t1Short;
	xpcc::ShortTimestamp t2Short;
	xpcc::Timestamp t1;
	xpcc::Timestamp t2;

	TEST_ASSERT_TRUE(t1Short == t2Short);
	TEST_ASSERT_FALSE(t1Short != t2Short);
	TEST_ASSERT_TRUE(t1 == t2);
	TEST_ASSERT_FALSE(t1 != t2);

	t1Short = xpcc::ArithmeticTraits<xpcc::ShortTimestamp::Type>::max / 2;
	t1 = xpcc::ArithmeticTraits<xpcc::Timestamp::Type>::max / 2;

	TEST_ASSERT_FALSE(t1Short == t2Short);
	TEST_ASSERT_TRUE(t1Short != t2Short);
	TEST_ASSERT_FALSE(t1 == t2);
	TEST_ASSERT_TRUE(t1 != t2);

	TEST_ASSERT_TRUE(t1Short > t2Short);
	TEST_ASSERT_TRUE(t1Short >= t2Short);
	TEST_ASSERT_TRUE(t1 > t2);
	TEST_ASSERT_TRUE(t1 >= t2);

	t1Short = xpcc::ArithmeticTraits<xpcc::ShortTimestamp::Type>::max / 2 + 1;
	t1 = xpcc::ArithmeticTraits<xpcc::Timestamp::Type>::max / 2 + 1;

	TEST_ASSERT_TRUE(t1Short < t2Short);
	TEST_ASSERT_TRUE(t1Short <= t2Short);
	TEST_ASSERT_TRUE(t1 < t2);
	TEST_ASSERT_TRUE(t1 <= t2);
}
