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
#include <type_traits>

#include "timestamp_test.hpp"
using namespace std::chrono_literals;

// ----------------------------------------------------------------------------
void
DurationTest::testConstructors()
{
	modm::ShortDuration t1Short{};
	TEST_ASSERT_TRUE(t1Short == 0ms);

	modm::Duration t1{};
	TEST_ASSERT_TRUE(t1 == 0ms);

	modm::ShortDuration t2Short(1s);
	TEST_ASSERT_TRUE(t2Short == 1000ms);

	modm::Duration t2(1s);
	TEST_ASSERT_TRUE(t2 == 1000ms);

	t1Short = modm::ShortDuration(500ms);
	TEST_ASSERT_TRUE(t1Short == 500ms);

	t1 = modm::Duration(500ms);
	TEST_ASSERT_TRUE(t1 == 500ms);
}

void
DurationTest::testArithmetics()
{
	modm::ShortDuration t1Short(200ms);
	modm::ShortDuration t2Short(500ms);
	modm::ShortDuration t3Short;

	t3Short = t1Short + t2Short;
	TEST_ASSERT_TRUE(t3Short == 700ms);

	modm::Duration t1(200ms);
	modm::Duration t2(500ms);
	modm::Duration t3;

	t3 = t1 + t2;
	TEST_ASSERT_TRUE(t3 == 0.7s);
}

void
DurationTest::testComparisons()
{
	modm::ShortDuration t1Short{};
	modm::ShortDuration t2Short{};
	modm::Duration t1{};
	modm::Duration t2{};

	TEST_ASSERT_TRUE(t1Short == t2Short);
	TEST_ASSERT_FALSE(t1Short != t2Short);
	TEST_ASSERT_TRUE(t1 == t2);
	TEST_ASSERT_FALSE(t1 != t2);

	t1Short = modm::ShortDuration::max();
	t1 = modm::Duration::max();

	TEST_ASSERT_FALSE(t1Short == t2Short);
	TEST_ASSERT_TRUE(t1Short != t2Short);
	TEST_ASSERT_FALSE(t1 == t2);
	TEST_ASSERT_TRUE(t1 != t2);

	TEST_ASSERT_TRUE(t1Short > t2Short);
	TEST_ASSERT_TRUE(t1Short >= t2Short);
	TEST_ASSERT_TRUE(t1 > t2);
	TEST_ASSERT_TRUE(t1 >= t2);

	t1Short = std::chrono::milliseconds(modm::ShortDuration::max() + 1ms);
	t1 = std::chrono::milliseconds(modm::Duration::max() + 1ms);

	TEST_ASSERT_TRUE(t1Short == t2Short);
	TEST_ASSERT_FALSE(t1Short != t2Short);
	TEST_ASSERT_TRUE(t1 == t2);
	TEST_ASSERT_FALSE(t1 != t2);
}
