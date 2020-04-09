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

#include "clock_test.hpp"
#include <modm/architecture/interface/clock.hpp>
#include <modm-test/mock/clock.hpp>
using test_clock = modm_test::chrono::milli_clock;

void
ClockTest::testClock()
{
	using time_point = modm::Clock::time_point;
	using duration = modm::Clock::duration;
	// test 32bit timestamp "Timestamp"
	test_clock::setTime(0);
	TEST_ASSERT_EQUALS(modm::Clock::now(), time_point(duration(0)));

	test_clock::setTime(10);
	TEST_ASSERT_EQUALS(modm::Clock::now(), time_point(duration(10)));

	test_clock::setTime(65'536);
	TEST_ASSERT_EQUALS(modm::Clock::now(), time_point(duration(65'536)));

	test_clock::setTime(4'294'967'295);
	TEST_ASSERT_EQUALS(modm::Clock::now(), time_point(duration(4'294'967'295)));

	// overflow in both timestamp and Clock!
	test_clock::setTime(uint32_t(4'294'967'296));
	TEST_ASSERT_EQUALS(modm::Clock::now(), time_point(duration(0)));
}
