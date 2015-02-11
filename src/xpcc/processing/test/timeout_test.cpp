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

#include <xpcc/processing/timeout.hpp>
#include <xpcc/architecture/driver/clock_dummy.hpp>

#include "timeout_test.hpp"

// ----------------------------------------------------------------------------
void
TimeoutTest::setUp()
{
	xpcc::ClockDummy::setTime(0);
}

void
TimeoutTest::testBasics()
{
	xpcc::GenericTimeout<xpcc::ClockDummy, xpcc::ShortTimestamp> timeoutShort(10);
	xpcc::GenericTimeout<xpcc::ClockDummy, xpcc::Timestamp> timeout(10);

	TEST_ASSERT_FALSE(timeoutShort.isExpired());
	TEST_ASSERT_FALSE(timeout.isExpired());

	int i;
	for (i = 0; i < 9; ++i) {
		xpcc::ClockDummy::setTime(i);
		TEST_ASSERT_FALSE(timeoutShort.isExpired());
		TEST_ASSERT_FALSE(timeout.isExpired());
	}

	TEST_ASSERT_FALSE(timeoutShort.hasFired());
	TEST_ASSERT_FALSE(timeout.hasFired());

	xpcc::ClockDummy::setTime(10);
	TEST_ASSERT_FALSE(timeoutShort.hasFired());
	TEST_ASSERT_FALSE(timeout.hasFired());

	TEST_ASSERT_TRUE(timeoutShort.isExpired());
	TEST_ASSERT_TRUE(timeout.isExpired());

	TEST_ASSERT_TRUE(timeoutShort.hasFired());
	TEST_ASSERT_TRUE(timeout.hasFired());

	// check that the class does not hold the state
	xpcc::ClockDummy::setTime(11);
	TEST_ASSERT_FALSE(timeoutShort.isExpired());
	TEST_ASSERT_FALSE(timeout.isExpired());

	// but hasFired remains
	TEST_ASSERT_TRUE(timeoutShort.hasFired());
	TEST_ASSERT_TRUE(timeout.hasFired());

	timeoutShort.stop();
	timeout.stop();

	TEST_ASSERT_TRUE(timeoutShort.isStopped());
	TEST_ASSERT_TRUE(timeout.isStopped());

	TEST_ASSERT_FALSE(timeoutShort.isExpired());
	TEST_ASSERT_FALSE(timeout.isExpired());

	TEST_ASSERT_TRUE(timeoutShort.hasFired());
	TEST_ASSERT_TRUE(timeout.hasFired());
}

void
TimeoutTest::testDefaultConstructor()
{
	xpcc::GenericTimeout<xpcc::ClockDummy, xpcc::ShortTimestamp> timeoutShort;
	xpcc::GenericTimeout<xpcc::ClockDummy, xpcc::Timestamp> timeout;

	TEST_ASSERT_TRUE(timeoutShort.isStopped());
	TEST_ASSERT_TRUE(timeout.isStopped());

	TEST_ASSERT_FALSE(timeoutShort.hasFired());
	TEST_ASSERT_FALSE(timeout.hasFired());

	TEST_ASSERT_FALSE(timeoutShort.isExpired());
	TEST_ASSERT_FALSE(timeout.isExpired());
}

void
TimeoutTest::testTimeOverflow()
{
	xpcc::ShortTimestamp::Type time = xpcc::ArithmeticTraits<xpcc::ShortTimestamp::Type>::max;
	TEST_ASSERT_EQUALS(time, 65535);

	// overflow after 65535 for uint16_t => 32767+100 = 32867
	xpcc::ClockDummy::setTime(time / 2 + 100);
	TEST_ASSERT_EQUALS((time / 2 + 100), 32867);

	xpcc::GenericTimeout<xpcc::ClockDummy, xpcc::ShortTimestamp> timeoutShort(time / 2 - 1);	//=> 32867 + 32766 = 97
	TEST_ASSERT_EQUALS((time / 2 - 1), 32766);

	TEST_ASSERT_FALSE(timeoutShort.isExpired());

	xpcc::ClockDummy::setTime(time);
	TEST_ASSERT_FALSE(timeoutShort.isExpired());

	xpcc::ClockDummy::setTime(0);
	TEST_ASSERT_FALSE(timeoutShort.isExpired());

	// Overflow happened. This needs to be avoided by the user!
	xpcc::ClockDummy::setTime(100);
	TEST_ASSERT_TRUE(timeoutShort.isExpired());
}

void
TimeoutTest::testRestart()
{
	xpcc::GenericTimeout<xpcc::ClockDummy, xpcc::ShortTimestamp> timeoutShort;
	xpcc::GenericTimeout<xpcc::ClockDummy, xpcc::Timestamp> timeout;

	TEST_ASSERT_FALSE(timeoutShort.isExpired());
	TEST_ASSERT_FALSE(timeout.isExpired());

	timeoutShort.restart(42);
	timeout.restart(42);
	TEST_ASSERT_FALSE(timeoutShort.isExpired());
	TEST_ASSERT_FALSE(timeout.isExpired());

	TEST_ASSERT_FALSE(timeoutShort.hasFired());
	TEST_ASSERT_FALSE(timeout.hasFired());

	xpcc::ClockDummy::setTime(10);
	TEST_ASSERT_FALSE(timeoutShort.isExpired());
	TEST_ASSERT_FALSE(timeout.isExpired());

	xpcc::ClockDummy::setTime(50);
	TEST_ASSERT_TRUE(timeoutShort.isExpired());
	TEST_ASSERT_TRUE(timeout.isExpired());

	TEST_ASSERT_FALSE(timeoutShort.isExpired());
	TEST_ASSERT_FALSE(timeout.isExpired());

	TEST_ASSERT_TRUE(timeoutShort.hasFired());
	TEST_ASSERT_TRUE(timeout.hasFired());

	timeoutShort.restart(60);
	timeout.restart(60);

	TEST_ASSERT_FALSE(timeoutShort.isExpired());
	TEST_ASSERT_FALSE(timeout.isExpired());

	TEST_ASSERT_FALSE(timeoutShort.hasFired());
	TEST_ASSERT_FALSE(timeout.hasFired());

	xpcc::ClockDummy::setTime(150);

	TEST_ASSERT_FALSE(timeoutShort.hasFired());
	TEST_ASSERT_FALSE(timeout.hasFired());

	TEST_ASSERT_TRUE(timeoutShort.isExpired());
	TEST_ASSERT_TRUE(timeout.isExpired());

	TEST_ASSERT_FALSE(timeoutShort.isExpired());
	TEST_ASSERT_FALSE(timeout.isExpired());

	TEST_ASSERT_TRUE(timeoutShort.hasFired());
	TEST_ASSERT_TRUE(timeout.hasFired());

}
