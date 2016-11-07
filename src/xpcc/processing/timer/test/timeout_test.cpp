/*
 * Copyright (c) 2009-2010, 2012, Fabian Greif
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2012, 2014-2015, Niklas Hauser
 * Copyright (c) 2013, 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <xpcc/processing/timer.hpp>
#include <xpcc/architecture/driver/clock_dummy.hpp>

#include "timeout_test.hpp"

// ----------------------------------------------------------------------------
void
TimeoutTest::setUp()
{
	xpcc::ClockDummy::setTime(0);
}

void
TimeoutTest::testDefaultConstructor()
{
	xpcc::GenericTimeout<xpcc::ClockDummy, xpcc::ShortTimestamp> timeoutShort;
	xpcc::GenericTimeout<xpcc::ClockDummy, xpcc::Timestamp> timeout;

	TEST_ASSERT_EQUALS(timeoutShort.remaining(), 0l);
	TEST_ASSERT_EQUALS(timeout.remaining(), 0l);

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

	TEST_ASSERT_EQUALS(timeoutShort.remaining(), 0l);
	TEST_ASSERT_EQUALS(timeout.remaining(), 0l);
}

void
TimeoutTest::testBasics()
{
	xpcc::GenericTimeout<xpcc::ClockDummy, xpcc::ShortTimestamp> timeoutShort(10);
	xpcc::GenericTimeout<xpcc::ClockDummy, xpcc::Timestamp> timeout(10);

	TEST_ASSERT_EQUALS(timeoutShort.remaining(), 10l);
	TEST_ASSERT_EQUALS(timeout.remaining(), 10l);

	TEST_ASSERT_FALSE(timeoutShort.execute());
	TEST_ASSERT_FALSE(timeout.execute());

	TEST_ASSERT_TRUE(timeoutShort.isArmed());
	TEST_ASSERT_TRUE(timeout.isArmed());

	TEST_ASSERT_FALSE(timeoutShort.isExpired());
	TEST_ASSERT_FALSE(timeout.isExpired());

	int i;
	for (i = 0; i < 9; ++i) {
		xpcc::ClockDummy::setTime(i);
		TEST_ASSERT_FALSE(timeoutShort.execute());
		TEST_ASSERT_FALSE(timeout.execute());

		TEST_ASSERT_FALSE(timeoutShort.isExpired());
		TEST_ASSERT_FALSE(timeout.isExpired());

		TEST_ASSERT_EQUALS(timeoutShort.remaining(), (10l-i));
		TEST_ASSERT_EQUALS(timeout.remaining(), (10l-i));
	}

	xpcc::ClockDummy::setTime(10);
	TEST_ASSERT_TRUE(timeoutShort.isExpired());
	TEST_ASSERT_TRUE(timeout.isExpired());

	TEST_ASSERT_FALSE(timeoutShort.isArmed());
	TEST_ASSERT_FALSE(timeout.isArmed());

	TEST_ASSERT_EQUALS(timeoutShort.remaining(), 0l);
	TEST_ASSERT_EQUALS(timeout.remaining(), 0l);

	TEST_ASSERT_TRUE(timeoutShort.execute());
	TEST_ASSERT_TRUE(timeout.execute());

	TEST_ASSERT_FALSE(timeoutShort.execute());
	TEST_ASSERT_FALSE(timeout.execute());

	TEST_ASSERT_TRUE(timeoutShort.isExpired());
	TEST_ASSERT_TRUE(timeout.isExpired());

	TEST_ASSERT_FALSE(timeoutShort.isArmed());
	TEST_ASSERT_FALSE(timeout.isArmed());

	TEST_ASSERT_EQUALS(timeoutShort.remaining(), 0l);
	TEST_ASSERT_EQUALS(timeout.remaining(), 0l);


	// check that the class does not hold the state
	xpcc::ClockDummy::setTime(11);
	TEST_ASSERT_FALSE(timeoutShort.execute());
	TEST_ASSERT_FALSE(timeout.execute());

	TEST_ASSERT_TRUE(timeoutShort.isExpired());
	TEST_ASSERT_TRUE(timeout.isExpired());

	TEST_ASSERT_FALSE(timeoutShort.isArmed());
	TEST_ASSERT_FALSE(timeout.isArmed());

	TEST_ASSERT_EQUALS(timeoutShort.remaining(), -1l);
	TEST_ASSERT_EQUALS(timeout.remaining(), -1l);

	timeoutShort.stop();
	timeout.stop();

	TEST_ASSERT_TRUE(timeoutShort.isStopped());
	TEST_ASSERT_TRUE(timeout.isStopped());

	TEST_ASSERT_FALSE(timeoutShort.execute());
	TEST_ASSERT_FALSE(timeout.execute());

	TEST_ASSERT_FALSE(timeoutShort.isArmed());
	TEST_ASSERT_FALSE(timeout.isArmed());

	TEST_ASSERT_EQUALS(timeoutShort.remaining(), 0l);
	TEST_ASSERT_EQUALS(timeout.remaining(), 0l);
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
	TEST_ASSERT_EQUALS(((signed)(time / 2) - 1), 32766);

	TEST_ASSERT_FALSE(timeoutShort.execute());

	xpcc::ClockDummy::setTime(time);
	TEST_ASSERT_FALSE(timeoutShort.execute());

	xpcc::ClockDummy::setTime(0);
	TEST_ASSERT_FALSE(timeoutShort.execute());

	// Overflow happened. This needs to be avoided by the user!
	xpcc::ClockDummy::setTime(100);
	TEST_ASSERT_TRUE(timeoutShort.execute());
}

void
TimeoutTest::testRestart()
{
	xpcc::GenericTimeout<xpcc::ClockDummy, xpcc::ShortTimestamp> timeoutShort;
	xpcc::GenericTimeout<xpcc::ClockDummy, xpcc::Timestamp> timeout;

	TEST_ASSERT_FALSE(timeoutShort.execute());
	TEST_ASSERT_FALSE(timeout.execute());

	TEST_ASSERT_FALSE(timeoutShort.isArmed());
	TEST_ASSERT_FALSE(timeout.isArmed());

	TEST_ASSERT_FALSE(timeoutShort.isExpired());
	TEST_ASSERT_FALSE(timeout.isExpired());

	TEST_ASSERT_EQUALS(timeoutShort.remaining(), 0l);
	TEST_ASSERT_EQUALS(timeout.remaining(), 0l);


	timeoutShort.restart(42);
	timeout.restart(42);

	TEST_ASSERT_EQUALS(timeoutShort.remaining(), 42l);
	TEST_ASSERT_EQUALS(timeout.remaining(), 42l);

	TEST_ASSERT_FALSE(timeoutShort.execute());
	TEST_ASSERT_FALSE(timeout.execute());

	TEST_ASSERT_TRUE(timeoutShort.isArmed());
	TEST_ASSERT_TRUE(timeout.isArmed());

	TEST_ASSERT_FALSE(timeoutShort.isExpired());
	TEST_ASSERT_FALSE(timeout.isExpired());


	xpcc::ClockDummy::setTime(10);
	TEST_ASSERT_FALSE(timeoutShort.execute());
	TEST_ASSERT_FALSE(timeout.execute());

	TEST_ASSERT_TRUE(timeoutShort.isArmed());
	TEST_ASSERT_TRUE(timeout.isArmed());

	TEST_ASSERT_FALSE(timeoutShort.isExpired());
	TEST_ASSERT_FALSE(timeout.isExpired());

	TEST_ASSERT_EQUALS(timeoutShort.remaining(), 32l);
	TEST_ASSERT_EQUALS(timeout.remaining(), 32l);

	xpcc::ClockDummy::setTime(50);
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

	TEST_ASSERT_EQUALS(timeoutShort.remaining(), -8l);
	TEST_ASSERT_EQUALS(timeout.remaining(), -8l);


	timeoutShort.restart(60);
	timeout.restart(60);

	TEST_ASSERT_TRUE(timeoutShort.isArmed());
	TEST_ASSERT_TRUE(timeout.isArmed());

	TEST_ASSERT_FALSE(timeoutShort.execute());
	TEST_ASSERT_FALSE(timeout.execute());

	TEST_ASSERT_FALSE(timeoutShort.isExpired());
	TEST_ASSERT_FALSE(timeout.isExpired());

	TEST_ASSERT_EQUALS(timeoutShort.remaining(), 60l);
	TEST_ASSERT_EQUALS(timeout.remaining(), 60l);


	xpcc::ClockDummy::setTime(150);

	TEST_ASSERT_TRUE(timeoutShort.execute());
	TEST_ASSERT_TRUE(timeout.execute());

	TEST_ASSERT_FALSE(timeoutShort.execute());
	TEST_ASSERT_FALSE(timeout.execute());

	TEST_ASSERT_TRUE(timeoutShort.isExpired());
	TEST_ASSERT_TRUE(timeout.isExpired());

	TEST_ASSERT_EQUALS(timeoutShort.remaining(), -40l);
	TEST_ASSERT_EQUALS(timeout.remaining(), -40l);
}
