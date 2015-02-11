// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

	TEST_ASSERT_FALSE(timerShort.isStopped());
	TEST_ASSERT_FALSE(timer.isStopped());

	TEST_ASSERT_FALSE(timerShort.hasFired());
	TEST_ASSERT_FALSE(timer.hasFired());

	TEST_ASSERT_FALSE(timerShort.isExpired());
	TEST_ASSERT_FALSE(timer.isExpired());

	TEST_ASSERT_FALSE(timerShort.hasFired());
	TEST_ASSERT_FALSE(timer.hasFired());
}

void
PeriodicTimerTest::testTimer()
{
	xpcc::GenericPeriodicTimer<xpcc::ClockDummy, xpcc::ShortTimestamp> timerShort(10);
	xpcc::GenericPeriodicTimer<xpcc::ClockDummy, xpcc::Timestamp> timer(10);

	TEST_ASSERT_FALSE(timerShort.isExpired());
	TEST_ASSERT_FALSE(timer.isExpired());

	int i;
	for (i = 0; i < 9; ++i) {
		xpcc::ClockDummy::setTime(i);
		TEST_ASSERT_FALSE(timerShort.isExpired());
	}

	TEST_ASSERT_FALSE(timerShort.hasFired());
	TEST_ASSERT_FALSE(timer.hasFired());

	xpcc::ClockDummy::setTime(10);
	TEST_ASSERT_FALSE(timerShort.hasFired());
	TEST_ASSERT_FALSE(timer.hasFired());

	TEST_ASSERT_TRUE(timerShort.isExpired());
	TEST_ASSERT_FALSE(timerShort.isExpired());

	TEST_ASSERT_TRUE(timer.isExpired());
	TEST_ASSERT_FALSE(timer.isExpired());

	TEST_ASSERT_TRUE(timerShort.hasFired());
	TEST_ASSERT_TRUE(timer.hasFired());


	xpcc::ClockDummy::setTime(20);
	TEST_ASSERT_FALSE(timerShort.hasFired());
	TEST_ASSERT_FALSE(timer.hasFired());

	TEST_ASSERT_TRUE(timerShort.isExpired());
	TEST_ASSERT_FALSE(timerShort.isExpired());

	TEST_ASSERT_TRUE(timer.isExpired());
	TEST_ASSERT_FALSE(timer.isExpired());

	TEST_ASSERT_TRUE(timerShort.hasFired());
	TEST_ASSERT_TRUE(timer.hasFired());


	// we are going to miss the 30, 40, 50, 60, 70, 80, 90 periods
	// however, isExpired() should still only return exactly once
	xpcc::ClockDummy::setTime(100);
	TEST_ASSERT_FALSE(timerShort.hasFired());
	TEST_ASSERT_FALSE(timer.hasFired());

	TEST_ASSERT_TRUE(timerShort.isExpired());
	TEST_ASSERT_FALSE(timerShort.isExpired());

	TEST_ASSERT_TRUE(timer.isExpired());
	TEST_ASSERT_FALSE(timer.isExpired());

	TEST_ASSERT_TRUE(timerShort.hasFired());
	TEST_ASSERT_TRUE(timer.hasFired());

	// we are going to miss a couple of periods more
	xpcc::ClockDummy::setTime(150);
	TEST_ASSERT_FALSE(timerShort.hasFired());
	TEST_ASSERT_FALSE(timer.hasFired());

	// lets call isExpired() with a delay of 5ms after period expiration.
	// it should expired once, and then reschedule for 160ms, not 165ms!!
	xpcc::ClockDummy::setTime(155);
	TEST_ASSERT_FALSE(timerShort.hasFired());
	TEST_ASSERT_FALSE(timer.hasFired());

	TEST_ASSERT_TRUE(timerShort.isExpired());
	TEST_ASSERT_FALSE(timerShort.isExpired());

	TEST_ASSERT_TRUE(timer.isExpired());
	TEST_ASSERT_FALSE(timer.isExpired());

	TEST_ASSERT_TRUE(timerShort.hasFired());
	TEST_ASSERT_TRUE(timer.hasFired());

	// not yet
	xpcc::ClockDummy::setTime(159);
	TEST_ASSERT_FALSE(timerShort.isExpired());
	TEST_ASSERT_FALSE(timer.isExpired());

	TEST_ASSERT_TRUE(timerShort.hasFired());
	TEST_ASSERT_TRUE(timer.hasFired());

	// now it should expire
	xpcc::ClockDummy::setTime(160);
	TEST_ASSERT_FALSE(timerShort.hasFired());
	TEST_ASSERT_FALSE(timer.hasFired());

	TEST_ASSERT_TRUE(timerShort.isExpired());
	TEST_ASSERT_FALSE(timerShort.isExpired());

	TEST_ASSERT_TRUE(timer.isExpired());
	TEST_ASSERT_FALSE(timer.isExpired());

	// it should not fire at 165ms
	xpcc::ClockDummy::setTime(165);
	TEST_ASSERT_FALSE(timerShort.isExpired());
	TEST_ASSERT_FALSE(timer.isExpired());

	TEST_ASSERT_TRUE(timerShort.hasFired());
	TEST_ASSERT_TRUE(timer.hasFired());
}

void
PeriodicTimerTest::testRestart()
{
	xpcc::GenericPeriodicTimer<xpcc::ClockDummy, xpcc::ShortTimestamp> timerShort(10);
	xpcc::GenericPeriodicTimer<xpcc::ClockDummy, xpcc::ShortTimestamp> timer(10);

	TEST_ASSERT_FALSE(timerShort.hasFired());
	TEST_ASSERT_FALSE(timer.hasFired());

	TEST_ASSERT_FALSE(timerShort.isStopped());
	TEST_ASSERT_FALSE(timer.isStopped());

	TEST_ASSERT_FALSE(timerShort.isExpired());
	TEST_ASSERT_FALSE(timer.isExpired());


	timerShort.stop();
	timer.stop();

	TEST_ASSERT_FALSE(timerShort.hasFired());
	TEST_ASSERT_FALSE(timer.hasFired());

	TEST_ASSERT_TRUE(timerShort.isStopped());
	TEST_ASSERT_TRUE(timer.isStopped());


	timerShort.restart(5);
	timer.restart(5);

	TEST_ASSERT_FALSE(timerShort.isStopped());
	TEST_ASSERT_FALSE(timer.isStopped());

	TEST_ASSERT_FALSE(timerShort.hasFired());
	TEST_ASSERT_FALSE(timer.hasFired());

	TEST_ASSERT_FALSE(timerShort.isExpired());
	TEST_ASSERT_FALSE(timer.isExpired());


	xpcc::ClockDummy::setTime(5);
	TEST_ASSERT_TRUE(timerShort.isExpired());
	TEST_ASSERT_FALSE(timerShort.isExpired());

	TEST_ASSERT_TRUE(timer.isExpired());
	TEST_ASSERT_FALSE(timer.isExpired());
}
