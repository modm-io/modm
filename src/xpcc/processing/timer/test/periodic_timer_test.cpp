// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include <xpcc/processing/periodic_timer.hpp>
#include <xpcc/architecture/driver/clock_dummy.hpp>

#include "periodic_timer_test.hpp"

void
PeriodicTimerTest::setUp()
{
	xpcc::ClockDummy::setTime(0);
}

void
PeriodicTimerTest::testConstructor()
{
	xpcc::GenericPeriodicTimer<xpcc::ClockDummy, xpcc::ShortTimestamp> timerShort(10);
	xpcc::GenericPeriodicTimer<xpcc::ClockDummy, xpcc::Timestamp> timer(10);

	TEST_ASSERT_EQUALS(timerShort.remaining(), 10l);
	TEST_ASSERT_EQUALS(timer.remaining(), 10l);

	TEST_ASSERT_FALSE(timerShort.isStopped());
	TEST_ASSERT_FALSE(timer.isStopped());

	TEST_ASSERT_TRUE(timerShort.getState() == xpcc::PeriodicTimerState::Armed);
	TEST_ASSERT_TRUE(timer.getState() == xpcc::PeriodicTimerState::Armed);

	TEST_ASSERT_FALSE(timerShort.execute());
	TEST_ASSERT_FALSE(timer.execute());
}

void
PeriodicTimerTest::testTimer()
{
	xpcc::GenericPeriodicTimer<xpcc::ClockDummy, xpcc::ShortTimestamp> timerShort(10);
	xpcc::GenericPeriodicTimer<xpcc::ClockDummy, xpcc::Timestamp> timer(10);

	TEST_ASSERT_EQUALS(timerShort.remaining(), 10l);
	TEST_ASSERT_EQUALS(timer.remaining(), 10l);

	TEST_ASSERT_FALSE(timerShort.execute());
	TEST_ASSERT_FALSE(timer.execute());

	int i;
	for (i = 0; i < 9; ++i) {
		xpcc::ClockDummy::setTime(i);

		TEST_ASSERT_FALSE(timerShort.execute());
		TEST_ASSERT_FALSE(timer.execute());

		TEST_ASSERT_EQUALS(timerShort.remaining(), (10l-i));
		TEST_ASSERT_EQUALS(timer.remaining(), (10l-i));
	}

	TEST_ASSERT_TRUE(timerShort.getState() == xpcc::PeriodicTimerState::Armed);
	TEST_ASSERT_TRUE(timer.getState() == xpcc::PeriodicTimerState::Armed);

	xpcc::ClockDummy::setTime(10);

	TEST_ASSERT_EQUALS(timerShort.remaining(), 0l);
	TEST_ASSERT_EQUALS(timer.remaining(), 0l);

	TEST_ASSERT_TRUE(timerShort.execute());
	TEST_ASSERT_FALSE(timerShort.execute());

	TEST_ASSERT_TRUE(timer.execute());
	TEST_ASSERT_FALSE(timer.execute());

	TEST_ASSERT_EQUALS(timerShort.remaining(), 10l);
	TEST_ASSERT_EQUALS(timer.remaining(), 10l);

	TEST_ASSERT_TRUE(timerShort.getState() == xpcc::PeriodicTimerState::Armed);
	TEST_ASSERT_TRUE(timer.getState() == xpcc::PeriodicTimerState::Armed);


	xpcc::ClockDummy::setTime(20);
	TEST_ASSERT_TRUE(timerShort.getState() == xpcc::PeriodicTimerState::Expired);
	TEST_ASSERT_TRUE(timer.getState() == xpcc::PeriodicTimerState::Expired);

	TEST_ASSERT_TRUE(timerShort.execute());
	TEST_ASSERT_FALSE(timerShort.execute());

	TEST_ASSERT_TRUE(timer.execute());
	TEST_ASSERT_FALSE(timer.execute());

	TEST_ASSERT_TRUE(timerShort.getState() == xpcc::PeriodicTimerState::Armed);
	TEST_ASSERT_TRUE(timer.getState() == xpcc::PeriodicTimerState::Armed);


	// we are going to miss the 30, 40, 50, 60, 70, 80, 90 periods
	// however, execute() should still only return exactly once
	xpcc::ClockDummy::setTime(100);
	TEST_ASSERT_TRUE(timerShort.getState() == xpcc::PeriodicTimerState::Expired);
	TEST_ASSERT_TRUE(timer.getState() == xpcc::PeriodicTimerState::Expired);

	TEST_ASSERT_EQUALS(timerShort.remaining(), -70l);
	TEST_ASSERT_EQUALS(timer.remaining(), -70l);

	TEST_ASSERT_TRUE(timerShort.execute());
	TEST_ASSERT_FALSE(timerShort.execute());

	TEST_ASSERT_TRUE(timer.execute());
	TEST_ASSERT_FALSE(timer.execute());

	TEST_ASSERT_EQUALS(timerShort.remaining(), 10l);
	TEST_ASSERT_EQUALS(timer.remaining(), 10l);

	// we are going to miss a couple of periods more
	xpcc::ClockDummy::setTime(150);
	TEST_ASSERT_TRUE(timerShort.getState() == xpcc::PeriodicTimerState::Expired);
	TEST_ASSERT_TRUE(timer.getState() == xpcc::PeriodicTimerState::Expired);

	TEST_ASSERT_EQUALS(timerShort.remaining(), -40l);
	TEST_ASSERT_EQUALS(timer.remaining(), -40l);

	// lets call execute() with a delay of 5ms after period expiration.
	// it should expired once, and then reschedule for 160ms, not 165ms!!
	xpcc::ClockDummy::setTime(155);
	TEST_ASSERT_TRUE(timerShort.getState() == xpcc::PeriodicTimerState::Expired);
	TEST_ASSERT_TRUE(timer.getState() == xpcc::PeriodicTimerState::Expired);

	TEST_ASSERT_EQUALS(timerShort.remaining(), -45l);
	TEST_ASSERT_EQUALS(timer.remaining(), -45l);

	TEST_ASSERT_TRUE(timerShort.execute());
	TEST_ASSERT_FALSE(timerShort.execute());

	TEST_ASSERT_TRUE(timer.execute());
	TEST_ASSERT_FALSE(timer.execute());

	TEST_ASSERT_EQUALS(timerShort.remaining(), 5l);
	TEST_ASSERT_EQUALS(timer.remaining(), 5l);

	// not yet
	xpcc::ClockDummy::setTime(159);
	TEST_ASSERT_TRUE(timerShort.getState() == xpcc::PeriodicTimerState::Armed);
	TEST_ASSERT_TRUE(timer.getState() == xpcc::PeriodicTimerState::Armed);

	TEST_ASSERT_EQUALS(timerShort.remaining(), 1l);
	TEST_ASSERT_EQUALS(timer.remaining(), 1l);

	// now it should expire
	xpcc::ClockDummy::setTime(160);
	TEST_ASSERT_TRUE(timerShort.getState() == xpcc::PeriodicTimerState::Expired);
	TEST_ASSERT_TRUE(timer.getState() == xpcc::PeriodicTimerState::Expired);

	TEST_ASSERT_EQUALS(timerShort.remaining(), 0l);
	TEST_ASSERT_EQUALS(timer.remaining(), 0l);

	TEST_ASSERT_TRUE(timerShort.execute());
	TEST_ASSERT_FALSE(timerShort.execute());

	TEST_ASSERT_TRUE(timer.execute());
	TEST_ASSERT_FALSE(timer.execute());

	TEST_ASSERT_EQUALS(timerShort.remaining(), 10l);
	TEST_ASSERT_EQUALS(timer.remaining(), 10l);

	// it should not fire at 165ms
	xpcc::ClockDummy::setTime(165);
	TEST_ASSERT_TRUE(timerShort.getState() == xpcc::PeriodicTimerState::Armed);
	TEST_ASSERT_TRUE(timer.getState() == xpcc::PeriodicTimerState::Armed);

	TEST_ASSERT_FALSE(timerShort.execute());
	TEST_ASSERT_FALSE(timer.execute());

	TEST_ASSERT_EQUALS(timerShort.remaining(), 5l);
	TEST_ASSERT_EQUALS(timer.remaining(), 5l);
}

void
PeriodicTimerTest::testRestart()
{
	xpcc::GenericPeriodicTimer<xpcc::ClockDummy, xpcc::ShortTimestamp> timerShort(10);
	xpcc::GenericPeriodicTimer<xpcc::ClockDummy, xpcc::ShortTimestamp> timer(10);

	TEST_ASSERT_TRUE(timerShort.getState() == xpcc::PeriodicTimerState::Armed);
	TEST_ASSERT_TRUE(timer.getState() == xpcc::PeriodicTimerState::Armed);

	TEST_ASSERT_FALSE(timerShort.isStopped());
	TEST_ASSERT_FALSE(timer.isStopped());

	TEST_ASSERT_FALSE(timerShort.execute());
	TEST_ASSERT_FALSE(timer.execute());


	timerShort.stop();
	timer.stop();

	TEST_ASSERT_FALSE(timerShort.getState() == xpcc::PeriodicTimerState::Armed);
	TEST_ASSERT_FALSE(timer.getState() == xpcc::PeriodicTimerState::Armed);

	TEST_ASSERT_TRUE(timerShort.isStopped());
	TEST_ASSERT_TRUE(timer.isStopped());


	timerShort.restart(5);
	timer.restart(5);

	TEST_ASSERT_FALSE(timerShort.isStopped());
	TEST_ASSERT_FALSE(timer.isStopped());

	TEST_ASSERT_TRUE(timerShort.getState() == xpcc::PeriodicTimerState::Armed);
	TEST_ASSERT_TRUE(timer.getState() == xpcc::PeriodicTimerState::Armed);

	TEST_ASSERT_FALSE(timerShort.execute());
	TEST_ASSERT_FALSE(timer.execute());


	xpcc::ClockDummy::setTime(5);
	TEST_ASSERT_TRUE(timerShort.execute());
	TEST_ASSERT_FALSE(timerShort.execute());

	TEST_ASSERT_TRUE(timer.execute());
	TEST_ASSERT_FALSE(timer.execute());
}
