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
	// test 16bit timestamp "ShortTimestamp"
	TestingClock::time = 0;
	TEST_ASSERT_EQUALS(xpcc::Clock::nowShort(), xpcc::ShortTimestamp(0));

	TestingClock::time = 10;
	TEST_ASSERT_EQUALS(xpcc::Clock::nowShort(), xpcc::ShortTimestamp(10));

	TestingClock::time = 65000;
	TEST_ASSERT_EQUALS(xpcc::Clock::nowShort(), xpcc::ShortTimestamp(65000));

	TestingClock::time = 65535;
	TEST_ASSERT_EQUALS(xpcc::Clock::nowShort(), xpcc::ShortTimestamp(65535));

	// overflow in timestamp, but not the Clock!
	TestingClock::time = 65536;
	TEST_ASSERT_EQUALS(xpcc::Clock::nowShort(), xpcc::ShortTimestamp(0));


	// test 32bit timestamp "Timestamp"
	TestingClock::time = 0;
	TEST_ASSERT_EQUALS(xpcc::Clock::now(), xpcc::Timestamp(0));

	TestingClock::time = 10;
	TEST_ASSERT_EQUALS(xpcc::Clock::now(), xpcc::Timestamp(10));

	TestingClock::time = 65536;
	TEST_ASSERT_EQUALS(xpcc::Clock::now(), xpcc::Timestamp(65536));

	TestingClock::time = 4294967295;
	TEST_ASSERT_EQUALS(xpcc::Clock::now(), xpcc::Timestamp(4294967295));

	// overflow in both timestamp and Clock!
	TestingClock::time = uint32_t(4294967296);
	TEST_ASSERT_EQUALS(xpcc::Clock::now(), xpcc::Timestamp(0));
}
