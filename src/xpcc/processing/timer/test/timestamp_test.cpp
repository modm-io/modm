// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include <xpcc/processing/timestamp.hpp>
#include <xpcc/utils/arithmetic_traits.hpp>

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
