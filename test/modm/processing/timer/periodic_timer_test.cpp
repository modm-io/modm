/*
 * Copyright (c) 2009-2010, 2018, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012, 2015, Niklas Hauser
 * Copyright (c) 2013, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "periodic_timer_test.hpp"
#include <modm/processing/timer.hpp>
#include <modm-test/mock/clock.hpp>

using namespace std::chrono_literals;
using test_clock = modm_test::chrono::milli_clock;

void
PeriodicTimerTest::setUp()
{
	test_clock::setTime(0);
}

void
PeriodicTimerTest::testConstructor()
{
	modm::ShortPeriodicTimer timerShort(10ms);
	modm::PeriodicTimer timer(10ms);

	TEST_ASSERT_EQUALS(timerShort.remaining(), 10ms);
	TEST_ASSERT_EQUALS(timer.remaining(), 10ms);

	TEST_ASSERT_FALSE(timerShort.isStopped());
	TEST_ASSERT_FALSE(timer.isStopped());

	TEST_ASSERT_TRUE(timerShort.state() == modm::TimerState::Armed);
	TEST_ASSERT_TRUE(timer.state() == modm::TimerState::Armed);

	TEST_ASSERT_FALSE(timerShort.execute());
	TEST_ASSERT_FALSE(timer.execute());

	timerShort.restart(0ms);
	timer.restart(0ms);

	TEST_ASSERT_EQUALS(timerShort.execute(), 1u);
	TEST_ASSERT_EQUALS(timer.execute(), 1u);
}

void
PeriodicTimerTest::testTimer()
{
	modm::ShortPeriodicTimer timerShort(10ms);
	modm::PeriodicTimer timer(10ms);

	TEST_ASSERT_EQUALS(timerShort.remaining(), 10ms);
	TEST_ASSERT_EQUALS(timer.remaining(), 10ms);

	TEST_ASSERT_EQUALS(timerShort.execute(), 0u);
	TEST_ASSERT_EQUALS(timer.execute(), 0u);

	int i;
	for (i = 0; i < 9; ++i) {
		test_clock::setTime(i);

		TEST_ASSERT_EQUALS(timerShort.execute(), 0u);
		TEST_ASSERT_EQUALS(timer.execute(), 0u);

		TEST_ASSERT_EQUALS(timerShort.remaining(), std::chrono::milliseconds(10-i));
		TEST_ASSERT_EQUALS(timer.remaining(), std::chrono::milliseconds(10-i));
	}

	TEST_ASSERT_TRUE(timerShort.state() == modm::TimerState::Armed);
	TEST_ASSERT_TRUE(timer.state() == modm::TimerState::Armed);

	test_clock::setTime(10);

	TEST_ASSERT_EQUALS(timerShort.remaining(), 0ms);
	TEST_ASSERT_EQUALS(timer.remaining(), 0ms);

	TEST_ASSERT_EQUALS(timerShort.execute(), 1u);
	TEST_ASSERT_EQUALS(timerShort.execute(), 0u);

	TEST_ASSERT_EQUALS(timer.execute(), 1u);
	TEST_ASSERT_EQUALS(timer.execute(), 0u);

	TEST_ASSERT_EQUALS(timerShort.remaining(), 10ms);
	TEST_ASSERT_EQUALS(timer.remaining(), 10ms);

	TEST_ASSERT_TRUE(timerShort.state() == modm::TimerState::Armed);
	TEST_ASSERT_TRUE(timer.state() == modm::TimerState::Armed);


	test_clock::setTime(20);
	TEST_ASSERT_TRUE(timerShort.state() == modm::TimerState::Expired);
	TEST_ASSERT_TRUE(timer.state() == modm::TimerState::Expired);

	TEST_ASSERT_EQUALS(timerShort.execute(), 1u);
	TEST_ASSERT_EQUALS(timerShort.execute(), 0u);

	TEST_ASSERT_EQUALS(timer.execute(), 1u);
	TEST_ASSERT_EQUALS(timer.execute(), 0u);

	TEST_ASSERT_TRUE(timerShort.state() == modm::TimerState::Armed);
	TEST_ASSERT_TRUE(timer.state() == modm::TimerState::Armed);


	// we are going to miss the 30, 40, 50, 60, 70, 80, 90 periods
	// however, execute() should still only return exactly once
	test_clock::setTime(100);
	TEST_ASSERT_TRUE(timerShort.state() == modm::TimerState::Expired);
	TEST_ASSERT_TRUE(timer.state() == modm::TimerState::Expired);

	TEST_ASSERT_EQUALS(timerShort.remaining(), -70ms);
	TEST_ASSERT_EQUALS(timer.remaining(), -70ms);

	TEST_ASSERT_EQUALS(timerShort.execute(), 7u);
	TEST_ASSERT_EQUALS(timerShort.execute(), 0u);

	TEST_ASSERT_EQUALS(timer.execute(), 7u);
	TEST_ASSERT_EQUALS(timer.execute(), 0u);

	TEST_ASSERT_EQUALS(timerShort.remaining(), 10ms);
	TEST_ASSERT_EQUALS(timer.remaining(), 10ms);

	// we are going to miss a couple of periods more
	test_clock::setTime(150);
	TEST_ASSERT_TRUE(timerShort.state() == modm::TimerState::Expired);
	TEST_ASSERT_TRUE(timer.state() == modm::TimerState::Expired);

	TEST_ASSERT_EQUALS(timerShort.remaining(), -40ms);
	TEST_ASSERT_EQUALS(timer.remaining(), -40ms);

	// lets call execute() with a delay of 5ms after period expiration.
	// it should expired once, and then reschedule for 160ms, not 165ms!!
	test_clock::setTime(155);
	TEST_ASSERT_TRUE(timerShort.state() == modm::TimerState::Expired);
	TEST_ASSERT_TRUE(timer.state() == modm::TimerState::Expired);

	TEST_ASSERT_EQUALS(timerShort.remaining(), -45ms);
	TEST_ASSERT_EQUALS(timer.remaining(), -45ms);

	TEST_ASSERT_EQUALS(timerShort.execute(), 5u);
	TEST_ASSERT_EQUALS(timerShort.execute(), 0u);

	TEST_ASSERT_EQUALS(timer.execute(), 5u);
	TEST_ASSERT_EQUALS(timer.execute(), 0u);

	TEST_ASSERT_EQUALS(timerShort.remaining(), 5ms);
	TEST_ASSERT_EQUALS(timer.remaining(), 5ms);

	// not yet
	test_clock::setTime(159);
	TEST_ASSERT_TRUE(timerShort.state() == modm::TimerState::Armed);
	TEST_ASSERT_TRUE(timer.state() == modm::TimerState::Armed);

	TEST_ASSERT_EQUALS(timerShort.remaining(), 1ms);
	TEST_ASSERT_EQUALS(timer.remaining(), 1ms);

	// now it should expire
	test_clock::setTime(160);
	TEST_ASSERT_TRUE(timerShort.state() == modm::TimerState::Expired);
	TEST_ASSERT_TRUE(timer.state() == modm::TimerState::Expired);

	TEST_ASSERT_EQUALS(timerShort.remaining(), 0ms);
	TEST_ASSERT_EQUALS(timer.remaining(), 0ms);

	TEST_ASSERT_EQUALS(timerShort.execute(), 1u);
	TEST_ASSERT_EQUALS(timerShort.execute(), 0u);

	TEST_ASSERT_EQUALS(timer.execute(), 1u);
	TEST_ASSERT_EQUALS(timer.execute(), 0u);

	TEST_ASSERT_EQUALS(timerShort.remaining(), 10ms);
	TEST_ASSERT_EQUALS(timer.remaining(), 10ms);

	// it should not fire at 165ms
	test_clock::setTime(165);
	TEST_ASSERT_TRUE(timerShort.state() == modm::TimerState::Armed);
	TEST_ASSERT_TRUE(timer.state() == modm::TimerState::Armed);

	TEST_ASSERT_EQUALS(timerShort.execute(), 0u);
	TEST_ASSERT_EQUALS(timer.execute(), 0u);

	TEST_ASSERT_EQUALS(timerShort.remaining(), 5ms);
	TEST_ASSERT_EQUALS(timer.remaining(), 5ms);
}

void
PeriodicTimerTest::testRestart()
{
	modm::ShortPeriodicTimer timerShort(10ms);
	modm::PeriodicTimer timer(10ms);

	TEST_ASSERT_TRUE(timerShort.state() == modm::TimerState::Armed);
	TEST_ASSERT_TRUE(timer.state() == modm::TimerState::Armed);

	TEST_ASSERT_FALSE(timerShort.isStopped());
	TEST_ASSERT_FALSE(timer.isStopped());

	TEST_ASSERT_FALSE(timerShort.execute());
	TEST_ASSERT_FALSE(timer.execute());


	timerShort.stop();
	timer.stop();

	TEST_ASSERT_FALSE(timerShort.state() == modm::TimerState::Armed);
	TEST_ASSERT_FALSE(timer.state() == modm::TimerState::Armed);

	TEST_ASSERT_TRUE(timerShort.isStopped());
	TEST_ASSERT_TRUE(timer.isStopped());


	timerShort.restart(5ms);
	timer.restart(5ms);

	TEST_ASSERT_FALSE(timerShort.isStopped());
	TEST_ASSERT_FALSE(timer.isStopped());

	TEST_ASSERT_TRUE(timerShort.state() == modm::TimerState::Armed);
	TEST_ASSERT_TRUE(timer.state() == modm::TimerState::Armed);

	TEST_ASSERT_EQUALS(timerShort.execute(), 0u);
	TEST_ASSERT_EQUALS(timer.execute(), 0u);


	test_clock::setTime(5);
	TEST_ASSERT_EQUALS(timerShort.execute(), 1u);
	TEST_ASSERT_EQUALS(timerShort.execute(), 0u);

	TEST_ASSERT_EQUALS(timer.execute(), 1u);
	TEST_ASSERT_EQUALS(timer.execute(), 0u);
}
