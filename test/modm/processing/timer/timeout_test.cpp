/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2010, 2012, 2018, Fabian Greif
 * Copyright (c) 2012, 2014-2015, 2017, 2020, Niklas Hauser
 * Copyright (c) 2013, 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "timeout_test.hpp"
#include <type_traits>
#include <modm/processing/timer.hpp>
#include <modm-test/mock/clock.hpp>
#include <modm/debug.hpp>

using namespace std::chrono_literals;
using test_clock = modm_test::chrono::milli_clock;

// ----------------------------------------------------------------------------
void
TimeoutTest::setUp()
{
	test_clock::setTime(0);
}

void
TimeoutTest::testDefaultConstructor()
{
	modm::ShortTimeout timeoutShort;
	modm::Timeout timeout;

	TEST_ASSERT_EQUALS(timeoutShort.remaining(), 0ms);
	TEST_ASSERT_EQUALS(timeout.remaining(), 0ms);

	TEST_ASSERT_TRUE(timeoutShort.isStopped());
	TEST_ASSERT_TRUE(timeout.isStopped());

	TEST_ASSERT_FALSE(timeoutShort.isArmed());
	TEST_ASSERT_FALSE(timeout.isArmed());

	TEST_ASSERT_FALSE(timeoutShort.isExpired());
	TEST_ASSERT_FALSE(timeout.isExpired());

	TEST_ASSERT_FALSE(timeoutShort.execute());
	TEST_ASSERT_FALSE(timeout.execute());

	TEST_ASSERT_FALSE(timeoutShort.isExpired());
	TEST_ASSERT_FALSE(timeout.isExpired());

	TEST_ASSERT_EQUALS(timeoutShort.remaining(), 0ms);
	TEST_ASSERT_EQUALS(timeout.remaining(), 0ms);
}

void
TimeoutTest::testZeroConstructor()
{
	modm::ShortTimeout timeoutShort{0ms};
	modm::Timeout timeout{0ms};

	TEST_ASSERT_EQUALS(timeoutShort.remaining(), 0ms);
	TEST_ASSERT_EQUALS(timeout.remaining(), 0ms);

	TEST_ASSERT_FALSE(timeoutShort.isStopped());
	TEST_ASSERT_FALSE(timeout.isStopped());

	// immediately expires!
	TEST_ASSERT_FALSE(timeoutShort.isArmed());
	TEST_ASSERT_FALSE(timeout.isArmed());

	TEST_ASSERT_TRUE(timeoutShort.isExpired());
	TEST_ASSERT_TRUE(timeout.isExpired());

	TEST_ASSERT_TRUE(timeoutShort.execute());
	TEST_ASSERT_TRUE(timeout.execute());

	TEST_ASSERT_TRUE(timeoutShort.isExpired());
	TEST_ASSERT_TRUE(timeout.isExpired());

	TEST_ASSERT_FALSE(timeoutShort.execute());
	TEST_ASSERT_FALSE(timeout.execute());

	TEST_ASSERT_EQUALS(timeoutShort.remaining(), 0ms);
	TEST_ASSERT_EQUALS(timeout.remaining(), 0ms);
}

void
TimeoutTest::testNegativeConstructor()
{
	modm::ShortTimeout timeoutShort{-1ms};
	modm::Timeout timeout{-10ms};

	TEST_ASSERT_EQUALS(timeoutShort.remaining(), 0ms);
	TEST_ASSERT_EQUALS(timeout.remaining(), 0ms);

	TEST_ASSERT_TRUE(timeoutShort.isStopped());
	TEST_ASSERT_TRUE(timeout.isStopped());

	TEST_ASSERT_FALSE(timeoutShort.isArmed());
	TEST_ASSERT_FALSE(timeout.isArmed());

	TEST_ASSERT_FALSE(timeoutShort.isExpired());
	TEST_ASSERT_FALSE(timeout.isExpired());

	TEST_ASSERT_FALSE(timeoutShort.execute());
	TEST_ASSERT_FALSE(timeout.execute());

	TEST_ASSERT_FALSE(timeoutShort.isExpired());
	TEST_ASSERT_FALSE(timeout.isExpired());

	TEST_ASSERT_EQUALS(timeoutShort.remaining(), 0ms);
	TEST_ASSERT_EQUALS(timeout.remaining(), 0ms);
}

void
TimeoutTest::testBasics()
{
	modm::ShortTimeout timeoutShort(10ms);
	modm::Timeout timeout(10ms);

	TEST_ASSERT_EQUALS(timeoutShort.remaining(), 10ms);
	TEST_ASSERT_EQUALS(timeout.remaining(), 10ms);

	TEST_ASSERT_FALSE(timeoutShort.execute());
	TEST_ASSERT_FALSE(timeout.execute());

	TEST_ASSERT_TRUE(timeoutShort.isArmed());
	TEST_ASSERT_TRUE(timeout.isArmed());

	TEST_ASSERT_FALSE(timeoutShort.isExpired());
	TEST_ASSERT_FALSE(timeout.isExpired());

	for (int i = 0; i < 9; ++i) {
		test_clock::setTime(i);
		TEST_ASSERT_FALSE(timeoutShort.execute());
		TEST_ASSERT_FALSE(timeout.execute());

		TEST_ASSERT_FALSE(timeoutShort.isExpired());
		TEST_ASSERT_FALSE(timeout.isExpired());

		TEST_ASSERT_EQUALS(timeoutShort.remaining(), std::chrono::milliseconds(10-i));
		TEST_ASSERT_EQUALS(timeout.remaining(), std::chrono::milliseconds(10-i));
	}

	test_clock::setTime(10);
	TEST_ASSERT_TRUE(timeoutShort.isExpired());
	TEST_ASSERT_TRUE(timeout.isExpired());

	TEST_ASSERT_FALSE(timeoutShort.isArmed());
	TEST_ASSERT_FALSE(timeout.isArmed());

	TEST_ASSERT_EQUALS(timeoutShort.remaining(), 0ms);
	TEST_ASSERT_EQUALS(timeout.remaining(), 0ms);

	TEST_ASSERT_TRUE(timeoutShort.execute());
	TEST_ASSERT_TRUE(timeout.execute());

	TEST_ASSERT_FALSE(timeoutShort.execute());
	TEST_ASSERT_FALSE(timeout.execute());

	TEST_ASSERT_TRUE(timeoutShort.isExpired());
	TEST_ASSERT_TRUE(timeout.isExpired());

	TEST_ASSERT_FALSE(timeoutShort.isArmed());
	TEST_ASSERT_FALSE(timeout.isArmed());

	TEST_ASSERT_EQUALS(timeoutShort.remaining(), 0ms);
	TEST_ASSERT_EQUALS(timeout.remaining(), 0ms);


	// check that the class does not hold the state
	test_clock::setTime(11);
	TEST_ASSERT_FALSE(timeoutShort.execute());
	TEST_ASSERT_FALSE(timeout.execute());

	TEST_ASSERT_TRUE(timeoutShort.isExpired());
	TEST_ASSERT_TRUE(timeout.isExpired());

	TEST_ASSERT_FALSE(timeoutShort.isArmed());
	TEST_ASSERT_FALSE(timeout.isArmed());

	TEST_ASSERT_EQUALS(timeoutShort.remaining(), -1ms);
	TEST_ASSERT_EQUALS(timeout.remaining(), -1ms);

	timeoutShort.stop();
	timeout.stop();

	TEST_ASSERT_TRUE(timeoutShort.isStopped());
	TEST_ASSERT_TRUE(timeout.isStopped());

	TEST_ASSERT_FALSE(timeoutShort.execute());
	TEST_ASSERT_FALSE(timeout.execute());

	TEST_ASSERT_FALSE(timeoutShort.isArmed());
	TEST_ASSERT_FALSE(timeout.isArmed());

	TEST_ASSERT_EQUALS(timeoutShort.remaining(), 0ms);
	TEST_ASSERT_EQUALS(timeout.remaining(), 0ms);
}

void
TimeoutTest::testTimeOverflow()
{
	modm::ShortTimeout timeoutShort;
	modm::ShortDuration time{modm::ShortDuration::max()};
	TEST_ASSERT_EQUALS(time, 65535ms);

	// overflow after 65535 for uint16_t => 65535+100 = 99
	test_clock::setTime(time.count() + 100);
	TEST_ASSERT_EQUALS(time + modm::ShortDuration(100ms), 99ms);


	test_clock::setTime(0);
	timeoutShort.restart(time);
	TEST_ASSERT_EQUALS(timeoutShort.remaining(), 65535ms);
	TEST_ASSERT_FALSE(timeoutShort.execute());

	test_clock::setTime(100);
	TEST_ASSERT_EQUALS(timeoutShort.remaining(), 65435ms);
	TEST_ASSERT_FALSE(timeoutShort.execute());

	timeoutShort.restart(time);
	TEST_ASSERT_EQUALS(timeoutShort.remaining(), 65535ms);
	TEST_ASSERT_FALSE(timeoutShort.execute());

	test_clock::setTime(65535);
	timeoutShort.restart(time-1ms);
	TEST_ASSERT_EQUALS(timeoutShort.remaining(), 65534ms);
	TEST_ASSERT_FALSE(timeoutShort.execute());

	test_clock::setTime(65534);
	timeoutShort.restart(time+1ms);
	TEST_ASSERT_EQUALS(timeoutShort.remaining(), 0ms);
	TEST_ASSERT_TRUE(timeoutShort.execute());

	test_clock::setTime(-1000);
	timeoutShort.restart(500ms);
	TEST_ASSERT_EQUALS(timeoutShort.remaining(), 500ms);
	TEST_ASSERT_FALSE(timeoutShort.execute());
}

void
TimeoutTest::testRestart()
{
	modm::ShortTimeout timeoutShort;
	modm::Timeout timeout;

	TEST_ASSERT_FALSE(timeoutShort.execute());
	TEST_ASSERT_FALSE(timeout.execute());

	TEST_ASSERT_FALSE(timeoutShort.isArmed());
	TEST_ASSERT_FALSE(timeout.isArmed());

	TEST_ASSERT_FALSE(timeoutShort.isExpired());
	TEST_ASSERT_FALSE(timeout.isExpired());

	TEST_ASSERT_EQUALS(timeoutShort.remaining(), 0ms);
	TEST_ASSERT_EQUALS(timeout.remaining(), 0ms);


	timeoutShort.restart(42ms);
	timeout.restart(42ms);

	TEST_ASSERT_EQUALS(timeoutShort.remaining(), 42ms);
	TEST_ASSERT_EQUALS(timeout.remaining(), 42ms);

	TEST_ASSERT_FALSE(timeoutShort.execute());
	TEST_ASSERT_FALSE(timeout.execute());

	TEST_ASSERT_TRUE(timeoutShort.isArmed());
	TEST_ASSERT_TRUE(timeout.isArmed());

	TEST_ASSERT_FALSE(timeoutShort.isExpired());
	TEST_ASSERT_FALSE(timeout.isExpired());


	test_clock::setTime(10);
	TEST_ASSERT_FALSE(timeoutShort.execute());
	TEST_ASSERT_FALSE(timeout.execute());

	TEST_ASSERT_TRUE(timeoutShort.isArmed());
	TEST_ASSERT_TRUE(timeout.isArmed());

	TEST_ASSERT_FALSE(timeoutShort.isExpired());
	TEST_ASSERT_FALSE(timeout.isExpired());

	TEST_ASSERT_EQUALS(timeoutShort.remaining(), 32ms);
	TEST_ASSERT_EQUALS(timeout.remaining(), 32ms);

	test_clock::setTime(50);
	TEST_ASSERT_FALSE(timeoutShort.isArmed());
	TEST_ASSERT_FALSE(timeout.isArmed());

	TEST_ASSERT_TRUE(timeoutShort.isExpired());
	TEST_ASSERT_TRUE(timeout.isExpired());

	TEST_ASSERT_TRUE(timeoutShort.execute());
	TEST_ASSERT_TRUE(timeout.execute());

	TEST_ASSERT_FALSE(timeoutShort.execute());
	TEST_ASSERT_FALSE(timeout.execute());

	TEST_ASSERT_FALSE(timeoutShort.isArmed());
	TEST_ASSERT_FALSE(timeout.isArmed());

	TEST_ASSERT_TRUE(timeoutShort.isExpired());
	TEST_ASSERT_TRUE(timeout.isExpired());

	TEST_ASSERT_EQUALS(timeoutShort.remaining(), -8ms);
	TEST_ASSERT_EQUALS(timeout.remaining(), -8ms);


	timeoutShort.restart(60ms);
	timeout.restart(60ms);

	TEST_ASSERT_TRUE(timeoutShort.isArmed());
	TEST_ASSERT_TRUE(timeout.isArmed());

	TEST_ASSERT_FALSE(timeoutShort.execute());
	TEST_ASSERT_FALSE(timeout.execute());

	TEST_ASSERT_FALSE(timeoutShort.isExpired());
	TEST_ASSERT_FALSE(timeout.isExpired());

	TEST_ASSERT_EQUALS(timeoutShort.remaining(), 60ms);
	TEST_ASSERT_EQUALS(timeout.remaining(), 60ms);


	test_clock::setTime(150);

	TEST_ASSERT_TRUE(timeoutShort.execute());
	TEST_ASSERT_TRUE(timeout.execute());

	TEST_ASSERT_FALSE(timeoutShort.execute());
	TEST_ASSERT_FALSE(timeout.execute());

	TEST_ASSERT_TRUE(timeoutShort.isExpired());
	TEST_ASSERT_TRUE(timeout.isExpired());

	TEST_ASSERT_EQUALS(timeoutShort.remaining(), -40ms);
	TEST_ASSERT_EQUALS(timeout.remaining(), -40ms);
}
