/*
 * Copyright (c) 2009-2010, Fabian Greif
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

#include <modm/processing/timer.hpp>
#include <modm/architecture/driver/clock_dummy.hpp>

#include "periodic_timer_test.hpp"

void
PeriodicTimerTest::setUp()
{
	modm::ClockDummy::setTime(0);
}

void
PeriodicTimerTest::testConstructor()
{
	modm::GenericPeriodicTimer<modm::ClockDummy, modm::ShortTimestamp> timerShort(10);
	modm::GenericPeriodicTimer<modm::ClockDummy, modm::Timestamp> timer(10);

	TEST_ASSERT_EQUALS(timerShort.remaining(), 10l);
	TEST_ASSERT_EQUALS(timer.remaining(), 10l);

	TEST_ASSERT_FALSE(timerShort.isStopped());
	TEST_ASSERT_FALSE(timer.isStopped());

	TEST_ASSERT_TRUE(timerShort.getState() == modm::PeriodicTimerState::Armed);
	TEST_ASSERT_TRUE(timer.getState() == modm::PeriodicTimerState::Armed);

	TEST_ASSERT_FALSE(timerShort.execute());
	TEST_ASSERT_FALSE(timer.execute());
}

void
PeriodicTimerTest::testTimer()
{
	modm::GenericPeriodicTimer<modm::ClockDummy, modm::ShortTimestamp> timerShort(10);
	modm::GenericPeriodicTimer<modm::ClockDummy, modm::Timestamp> timer(10);

	TEST_ASSERT_EQUALS(timerShort.remaining(), 10l);
	TEST_ASSERT_EQUALS(timer.remaining(), 10l);

	TEST_ASSERT_FALSE(timerShort.execute());
	TEST_ASSERT_FALSE(timer.execute());

	int i;
	for (i = 0; i < 9; ++i) {
		modm::ClockDummy::setTime(i);

		TEST_ASSERT_FALSE(timerShort.execute());
		TEST_ASSERT_FALSE(timer.execute());

		TEST_ASSERT_EQUALS(timerShort.remaining(), (10l-i));
		TEST_ASSERT_EQUALS(timer.remaining(), (10l-i));
	}

	TEST_ASSERT_TRUE(timerShort.getState() == modm::PeriodicTimerState::Armed);
	TEST_ASSERT_TRUE(timer.getState() == modm::PeriodicTimerState::Armed);

	modm::ClockDummy::setTime(10);

	TEST_ASSERT_EQUALS(timerShort.remaining(), 0l);
	TEST_ASSERT_EQUALS(timer.remaining(), 0l);

	TEST_ASSERT_TRUE(timerShort.execute());
	TEST_ASSERT_FALSE(timerShort.execute());

	TEST_ASSERT_TRUE(timer.execute());
	TEST_ASSERT_FALSE(timer.execute());

	TEST_ASSERT_EQUALS(timerShort.remaining(), 10l);
	TEST_ASSERT_EQUALS(timer.remaining(), 10l);

	TEST_ASSERT_TRUE(timerShort.getState() == modm::PeriodicTimerState::Armed);
	TEST_ASSERT_TRUE(timer.getState() == modm::PeriodicTimerState::Armed);


	modm::ClockDummy::setTime(20);
	TEST_ASSERT_TRUE(timerShort.getState() == modm::PeriodicTimerState::Expired);
	TEST_ASSERT_TRUE(timer.getState() == modm::PeriodicTimerState::Expired);

	TEST_ASSERT_TRUE(timerShort.execute());
	TEST_ASSERT_FALSE(timerShort.execute());

	TEST_ASSERT_TRUE(timer.execute());
	TEST_ASSERT_FALSE(timer.execute());

	TEST_ASSERT_TRUE(timerShort.getState() == modm::PeriodicTimerState::Armed);
	TEST_ASSERT_TRUE(timer.getState() == modm::PeriodicTimerState::Armed);


	// we are going to miss the 30, 40, 50, 60, 70, 80, 90 periods
	// however, execute() should still only return exactly once
	modm::ClockDummy::setTime(100);
	TEST_ASSERT_TRUE(timerShort.getState() == modm::PeriodicTimerState::Expired);
	TEST_ASSERT_TRUE(timer.getState() == modm::PeriodicTimerState::Expired);

	TEST_ASSERT_EQUALS(timerShort.remaining(), -70l);
	TEST_ASSERT_EQUALS(timer.remaining(), -70l);

	TEST_ASSERT_TRUE(timerShort.execute());
	TEST_ASSERT_FALSE(timerShort.execute());

	TEST_ASSERT_TRUE(timer.execute());
	TEST_ASSERT_FALSE(timer.execute());

	TEST_ASSERT_EQUALS(timerShort.remaining(), 10l);
	TEST_ASSERT_EQUALS(timer.remaining(), 10l);

	// we are going to miss a couple of periods more
	modm::ClockDummy::setTime(150);
	TEST_ASSERT_TRUE(timerShort.getState() == modm::PeriodicTimerState::Expired);
	TEST_ASSERT_TRUE(timer.getState() == modm::PeriodicTimerState::Expired);

	TEST_ASSERT_EQUALS(timerShort.remaining(), -40l);
	TEST_ASSERT_EQUALS(timer.remaining(), -40l);

	// lets call execute() with a delay of 5ms after period expiration.
	// it should expired once, and then reschedule for 160ms, not 165ms!!
	modm::ClockDummy::setTime(155);
	TEST_ASSERT_TRUE(timerShort.getState() == modm::PeriodicTimerState::Expired);
	TEST_ASSERT_TRUE(timer.getState() == modm::PeriodicTimerState::Expired);

	TEST_ASSERT_EQUALS(timerShort.remaining(), -45l);
	TEST_ASSERT_EQUALS(timer.remaining(), -45l);

	TEST_ASSERT_TRUE(timerShort.execute());
	TEST_ASSERT_FALSE(timerShort.execute());

	TEST_ASSERT_TRUE(timer.execute());
	TEST_ASSERT_FALSE(timer.execute());

	TEST_ASSERT_EQUALS(timerShort.remaining(), 5l);
	TEST_ASSERT_EQUALS(timer.remaining(), 5l);

	// not yet
	modm::ClockDummy::setTime(159);
	TEST_ASSERT_TRUE(timerShort.getState() == modm::PeriodicTimerState::Armed);
	TEST_ASSERT_TRUE(timer.getState() == modm::PeriodicTimerState::Armed);

	TEST_ASSERT_EQUALS(timerShort.remaining(), 1l);
	TEST_ASSERT_EQUALS(timer.remaining(), 1l);

	// now it should expire
	modm::ClockDummy::setTime(160);
	TEST_ASSERT_TRUE(timerShort.getState() == modm::PeriodicTimerState::Expired);
	TEST_ASSERT_TRUE(timer.getState() == modm::PeriodicTimerState::Expired);

	TEST_ASSERT_EQUALS(timerShort.remaining(), 0l);
	TEST_ASSERT_EQUALS(timer.remaining(), 0l);

	TEST_ASSERT_TRUE(timerShort.execute());
	TEST_ASSERT_FALSE(timerShort.execute());

	TEST_ASSERT_TRUE(timer.execute());
	TEST_ASSERT_FALSE(timer.execute());

	TEST_ASSERT_EQUALS(timerShort.remaining(), 10l);
	TEST_ASSERT_EQUALS(timer.remaining(), 10l);

	// it should not fire at 165ms
	modm::ClockDummy::setTime(165);
	TEST_ASSERT_TRUE(timerShort.getState() == modm::PeriodicTimerState::Armed);
	TEST_ASSERT_TRUE(timer.getState() == modm::PeriodicTimerState::Armed);

	TEST_ASSERT_FALSE(timerShort.execute());
	TEST_ASSERT_FALSE(timer.execute());

	TEST_ASSERT_EQUALS(timerShort.remaining(), 5l);
	TEST_ASSERT_EQUALS(timer.remaining(), 5l);
}

void
PeriodicTimerTest::testRestart()
{
	modm::GenericPeriodicTimer<modm::ClockDummy, modm::ShortTimestamp> timerShort(10);
	modm::GenericPeriodicTimer<modm::ClockDummy, modm::ShortTimestamp> timer(10);

	TEST_ASSERT_TRUE(timerShort.getState() == modm::PeriodicTimerState::Armed);
	TEST_ASSERT_TRUE(timer.getState() == modm::PeriodicTimerState::Armed);

	TEST_ASSERT_FALSE(timerShort.isStopped());
	TEST_ASSERT_FALSE(timer.isStopped());

	TEST_ASSERT_FALSE(timerShort.execute());
	TEST_ASSERT_FALSE(timer.execute());


	timerShort.stop();
	timer.stop();

	TEST_ASSERT_FALSE(timerShort.getState() == modm::PeriodicTimerState::Armed);
	TEST_ASSERT_FALSE(timer.getState() == modm::PeriodicTimerState::Armed);

	TEST_ASSERT_TRUE(timerShort.isStopped());
	TEST_ASSERT_TRUE(timer.isStopped());


	timerShort.restart(5);
	timer.restart(5);

	TEST_ASSERT_FALSE(timerShort.isStopped());
	TEST_ASSERT_FALSE(timer.isStopped());

	TEST_ASSERT_TRUE(timerShort.getState() == modm::PeriodicTimerState::Armed);
	TEST_ASSERT_TRUE(timer.getState() == modm::PeriodicTimerState::Armed);

	TEST_ASSERT_FALSE(timerShort.execute());
	TEST_ASSERT_FALSE(timer.execute());


	modm::ClockDummy::setTime(5);
	TEST_ASSERT_TRUE(timerShort.execute());
	TEST_ASSERT_FALSE(timerShort.execute());

	TEST_ASSERT_TRUE(timer.execute());
	TEST_ASSERT_FALSE(timer.execute());
}
