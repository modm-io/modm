/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009, Thorsten Lajewski
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, 2015, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "testing_clock.hpp"

#include "clock_test.hpp"

void
ClockTest::testClock()
{
	// test 16bit timestamp "ShortTimestamp"
	TestingClock::time = 0;
	TEST_ASSERT_EQUALS(modm::Clock::nowShort(), modm::ShortTimestamp(0));

	TestingClock::time = 10;
	TEST_ASSERT_EQUALS(modm::Clock::nowShort(), modm::ShortTimestamp(10));

	TestingClock::time = 65000;
	TEST_ASSERT_EQUALS(modm::Clock::nowShort(), modm::ShortTimestamp(65000));

	TestingClock::time = 65535;
	TEST_ASSERT_EQUALS(modm::Clock::nowShort(), modm::ShortTimestamp(65535));

	// overflow in timestamp, but not the Clock!
	TestingClock::time = 65536;
	TEST_ASSERT_EQUALS(modm::Clock::nowShort(), modm::ShortTimestamp(0));


	// test 32bit timestamp "Timestamp"
	TestingClock::time = 0;
	TEST_ASSERT_EQUALS(modm::Clock::now(), modm::Timestamp(0));

	TestingClock::time = 10;
	TEST_ASSERT_EQUALS(modm::Clock::now(), modm::Timestamp(10));

	TestingClock::time = 65536;
	TEST_ASSERT_EQUALS(modm::Clock::now(), modm::Timestamp(65536));

	TestingClock::time = 4294967295;
	TEST_ASSERT_EQUALS(modm::Clock::now(), modm::Timestamp(4294967295));

	// overflow in both timestamp and Clock!
	TestingClock::time = uint32_t(4294967296);
	TEST_ASSERT_EQUALS(modm::Clock::now(), modm::Timestamp(0));
}
