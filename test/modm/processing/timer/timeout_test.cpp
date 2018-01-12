/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2010, 2012, 2018, Fabian Greif
 * Copyright (c) 2012, 2014-2015, 2017, Niklas Hauser
 * Copyright (c) 2013, 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/processing/timer.hpp>

#include "clock_dummy.hpp"
#include "timeout_test.hpp"

// ----------------------------------------------------------------------------
void
TimeoutTest::setUp()
{
	modm::ClockDummy::setTime(0);
}

void
TimeoutTest::testDefaultConstructor()
{
	modm::GenericTimeout<modm::ClockDummy, modm::ShortTimestamp> timeoutShort;
	modm::GenericTimeout<modm::ClockDummy, modm::Timestamp> timeout;

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
	modm::GenericTimeout<modm::ClockDummy, modm::ShortTimestamp> timeoutShort(10);
	modm::GenericTimeout<modm::ClockDummy, modm::Timestamp> timeout(10);

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
		modm::ClockDummy::setTime(i);
		TEST_ASSERT_FALSE(timeoutShort.execute());
		TEST_ASSERT_FALSE(timeout.execute());

		TEST_ASSERT_FALSE(timeoutShort.isExpired());
		TEST_ASSERT_FALSE(timeout.isExpired());

		TEST_ASSERT_EQUALS(timeoutShort.remaining(), (10l-i));
		TEST_ASSERT_EQUALS(timeout.remaining(), (10l-i));
	}

	modm::ClockDummy::setTime(10);
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
	modm::ClockDummy::setTime(11);
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
	modm::ShortTimestamp::Type time = modm::ArithmeticTraits<modm::ShortTimestamp::Type>::max;
	TEST_ASSERT_EQUALS(time, 65535);

	// overflow after 65535 for uint16_t => 32767+100 = 32867
	modm::ClockDummy::setTime(time / 2 + 100);
	TEST_ASSERT_EQUALS((time / 2 + 100), 32867);

	modm::GenericTimeout<modm::ClockDummy, modm::ShortTimestamp> timeoutShort(time / 2 - 1);	//=> 32867 + 32766 = 97
	TEST_ASSERT_EQUALS(((signed)(time / 2) - 1), 32766);

	TEST_ASSERT_FALSE(timeoutShort.execute());

	modm::ClockDummy::setTime(time);
	TEST_ASSERT_FALSE(timeoutShort.execute());

	modm::ClockDummy::setTime(0);
	TEST_ASSERT_FALSE(timeoutShort.execute());

	// Overflow happened. This needs to be avoided by the user!
	modm::ClockDummy::setTime(100);
	TEST_ASSERT_TRUE(timeoutShort.execute());
}

void
TimeoutTest::testRestart()
{
	modm::GenericTimeout<modm::ClockDummy, modm::ShortTimestamp> timeoutShort;
	modm::GenericTimeout<modm::ClockDummy, modm::Timestamp> timeout;

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


	modm::ClockDummy::setTime(10);
	TEST_ASSERT_FALSE(timeoutShort.execute());
	TEST_ASSERT_FALSE(timeout.execute());

	TEST_ASSERT_TRUE(timeoutShort.isArmed());
	TEST_ASSERT_TRUE(timeout.isArmed());

	TEST_ASSERT_FALSE(timeoutShort.isExpired());
	TEST_ASSERT_FALSE(timeout.isExpired());

	TEST_ASSERT_EQUALS(timeoutShort.remaining(), 32l);
	TEST_ASSERT_EQUALS(timeout.remaining(), 32l);

	modm::ClockDummy::setTime(50);
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


	modm::ClockDummy::setTime(150);

	TEST_ASSERT_TRUE(timeoutShort.execute());
	TEST_ASSERT_TRUE(timeout.execute());

	TEST_ASSERT_FALSE(timeoutShort.execute());
	TEST_ASSERT_FALSE(timeout.execute());

	TEST_ASSERT_TRUE(timeoutShort.isExpired());
	TEST_ASSERT_TRUE(timeout.isExpired());

	TEST_ASSERT_EQUALS(timeoutShort.remaining(), -40l);
	TEST_ASSERT_EQUALS(timeout.remaining(), -40l);
}
