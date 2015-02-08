// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include "testing_clock.hpp"

#include "clock_test.hpp"

void
ClockTest::testClock()
{
	TestingClock::time = 10;
	TEST_ASSERT_EQUALS(xpcc::Clock::now(), xpcc::Timestamp(10));

	TestingClock::time = 65000;
	TEST_ASSERT_EQUALS(xpcc::Clock::now(), xpcc::Timestamp(65000));

	TestingClock::time = 10;
	TEST_ASSERT_EQUALS(xpcc::Clock::now<xpcc::LongTimestamp>(), xpcc::LongTimestamp(10));

	TestingClock::time = 65000;
	TEST_ASSERT_EQUALS(xpcc::Clock::now<xpcc::LongTimestamp>(), xpcc::LongTimestamp(65000));

	TestingClock::time = 4294967295;
	TEST_ASSERT_EQUALS(xpcc::Clock::now<xpcc::LongTimestamp>(), xpcc::LongTimestamp(4294967295));
}
