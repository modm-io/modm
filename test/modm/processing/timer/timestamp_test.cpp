/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2010, 2012, Fabian Greif
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
	modm::ShortTimestamp t1Short;
	TEST_ASSERT_TRUE(t1Short == 0);

	modm::Timestamp t1;
	TEST_ASSERT_TRUE(t1 == 0);

	modm::ShortTimestamp t2Short(1000);
	TEST_ASSERT_TRUE(t2Short == 1000);

	modm::Timestamp t2(1000);
	TEST_ASSERT_TRUE(t2 == 1000);

	t1Short = 500;
	TEST_ASSERT_TRUE(t1Short == 500);

	t1 = 500;
	TEST_ASSERT_TRUE(t1 == 500);
}

void
TimestampTest::testArithmetics()
{
	modm::ShortTimestamp t1Short(200);
	modm::ShortTimestamp t2Short(500);
	modm::ShortTimestamp t3Short;

	t3Short = t1Short + t2Short;
	TEST_ASSERT_TRUE(t3Short == 700);

	modm::Timestamp t1(200);
	modm::Timestamp t2(500);
	modm::Timestamp t3;

	t3 = t1 + t2;
	TEST_ASSERT_TRUE(t3 == 700);
}

void
TimestampTest::testComparisons()
{
	modm::ShortTimestamp t1Short;
	modm::ShortTimestamp t2Short;
	modm::Timestamp t1;
	modm::Timestamp t2;

	TEST_ASSERT_TRUE(t1Short == t2Short);
	TEST_ASSERT_FALSE(t1Short != t2Short);
	TEST_ASSERT_TRUE(t1 == t2);
	TEST_ASSERT_FALSE(t1 != t2);

	t1Short = modm::ArithmeticTraits<modm::ShortTimestamp::Type>::max / 2;
	t1 = modm::ArithmeticTraits<modm::Timestamp::Type>::max / 2;

	TEST_ASSERT_FALSE(t1Short == t2Short);
	TEST_ASSERT_TRUE(t1Short != t2Short);
	TEST_ASSERT_FALSE(t1 == t2);
	TEST_ASSERT_TRUE(t1 != t2);

	TEST_ASSERT_TRUE(t1Short > t2Short);
	TEST_ASSERT_TRUE(t1Short >= t2Short);
	TEST_ASSERT_TRUE(t1 > t2);
	TEST_ASSERT_TRUE(t1 >= t2);

	t1Short = modm::ArithmeticTraits<modm::ShortTimestamp::Type>::max / 2 + 1;
	t1 = modm::ArithmeticTraits<modm::Timestamp::Type>::max / 2 + 1;

	TEST_ASSERT_TRUE(t1Short < t2Short);
	TEST_ASSERT_TRUE(t1Short <= t2Short);
	TEST_ASSERT_TRUE(t1 < t2);
	TEST_ASSERT_TRUE(t1 <= t2);
}
