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

#include <xpcc/workflow/periodic_timer.hpp>
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
	xpcc::PeriodicTimer<xpcc::ClockDummy> timer(10);
	
	TEST_ASSERT_TRUE(timer.isRunning());
	TEST_ASSERT_FALSE(timer.isExpired());
}

void
PeriodicTimerTest::testTimer()
{
	xpcc::PeriodicTimer<xpcc::ClockDummy> timer(10);
	
	TEST_ASSERT_FALSE(timer.isExpired());
	
	int i;
	for (i = 0; i < 9; ++i) {
		xpcc::ClockDummy::setTime(i);
		TEST_ASSERT_FALSE(timer.isExpired());
	}
	
	xpcc::ClockDummy::setTime(10);
	TEST_ASSERT_TRUE(timer.isExpired());
	TEST_ASSERT_FALSE(timer.isExpired());
	
	xpcc::ClockDummy::setTime(20);
	TEST_ASSERT_TRUE(timer.isExpired());
	TEST_ASSERT_FALSE(timer.isExpired());
	
	xpcc::ClockDummy::setTime(100);
	TEST_ASSERT_TRUE(timer.isExpired());
	TEST_ASSERT_FALSE(timer.isExpired());
}

void
PeriodicTimerTest::testRestart()
{
	xpcc::PeriodicTimer<xpcc::ClockDummy> timer(10);
	
	TEST_ASSERT_TRUE(timer.isRunning());
	TEST_ASSERT_FALSE(timer.isExpired());
	
	timer.stop();
	
	TEST_ASSERT_FALSE(timer.isRunning());
	
	timer.restart(5);
	
	TEST_ASSERT_TRUE(timer.isRunning());
	TEST_ASSERT_FALSE(timer.isExpired());
	
	xpcc::ClockDummy::setTime(5);
	TEST_ASSERT_TRUE(timer.isExpired());
	TEST_ASSERT_FALSE(timer.isExpired());
}
