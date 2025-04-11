/*
 * Copyright (c) 2024, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "datetime_test.hpp"
#include <modm/math/calendar/date_time.hpp>

using namespace std::chrono_literals;

void
DateTimeTest::testConversion()
{
	const auto dt1 = modm::DateTime(1970, 1, 1);
	TEST_ASSERT_EQUALS(dt1.day_of_year().count(), 0l);
	TEST_ASSERT_EQUALS(dt1.time_t(), 0l);
	TEST_ASSERT_EQUALS(dt1.year(), 1970y);
	TEST_ASSERT_EQUALS(dt1.month(), std::chrono::January);
	TEST_ASSERT_EQUALS(dt1.day(), 1d);
	TEST_ASSERT_EQUALS(dt1.hours(), 0h);
	TEST_ASSERT_EQUALS(dt1.minutes(), 0min);
	TEST_ASSERT_EQUALS(dt1.seconds(), 0s);
	TEST_ASSERT_EQUALS(dt1.subseconds(), 0ms);

	const auto dt2 = modm::DateTime(1970, 1, 1, 0, 0, 1, 42, 2);
	TEST_ASSERT_EQUALS(dt2.day_of_year().count(), 0l);
	TEST_ASSERT_EQUALS(dt2.time_t(), 1);
	TEST_ASSERT_EQUALS(dt2.time_since_epoch(), 1042ms);
	TEST_ASSERT_TRUE(dt1 < dt2);
	TEST_ASSERT_TRUE(dt1 <= dt2);
	TEST_ASSERT_FALSE(dt1 == dt2);
	TEST_ASSERT_FALSE(dt1 >= dt2);
	TEST_ASSERT_FALSE(dt1 > dt2);

	// first leap year since epoch
	const auto dt3 = modm::DateTime(1972, 3, 1, 0, 0, 0);
	TEST_ASSERT_EQUALS(dt3.day_of_year().count(), 31+29);
	TEST_ASSERT_EQUALS(dt3.time_t(), 24*60*60*(365*2+31+29));
	TEST_ASSERT_TRUE(dt1 < dt3);
	TEST_ASSERT_TRUE(dt2 < dt3);

	const auto dt4 = modm::DateTime(2024, 12, 24, 12, 24, 12);
	TEST_ASSERT_EQUALS(dt4.day_of_year().count(), 358);
	TEST_ASSERT_EQUALS(dt4.time_t(), 1735043052);
	TEST_ASSERT_TRUE(dt1 < dt4);
	TEST_ASSERT_TRUE(dt2 < dt4);
	TEST_ASSERT_TRUE(dt3 < dt4);
	TEST_ASSERT_EQUALS(dt4, dt4);

	const auto dt5 = modm::DateTime(2010, 11, 12, 13, 14, 15, 16, 4);
	TEST_ASSERT_EQUALS(dt5.time_t(), 1289567655);
	TEST_ASSERT_EQUALS(dt5.day_of_year().count(), 315);
	TEST_ASSERT_EQUALS(dt5.year(), 2010y);
	TEST_ASSERT_EQUALS(dt5.month(), std::chrono::November);
	TEST_ASSERT_EQUALS(dt5.day(), 12d);
	TEST_ASSERT_EQUALS(dt5.weekday(), std::chrono::weekday{4});
	TEST_ASSERT_EQUALS(dt5.weekday(), std::chrono::Thursday);
	TEST_ASSERT_EQUALS(dt5.hours(), 13h);
	TEST_ASSERT_EQUALS(dt5.minutes(), 14min);
	TEST_ASSERT_EQUALS(dt5.seconds(), 15s);
	TEST_ASSERT_EQUALS(dt5.subseconds(), 16ms);

	const auto tm5 = dt5.tm();
	TEST_ASSERT_EQUALS(tm5.tm_year, 110); // epoch=1900
	TEST_ASSERT_EQUALS(tm5.tm_mon, 10); // 0-indexed!
	TEST_ASSERT_EQUALS(tm5.tm_mday, 12);
	TEST_ASSERT_EQUALS(tm5.tm_hour, 13);
	TEST_ASSERT_EQUALS(tm5.tm_min, 14);
	TEST_ASSERT_EQUALS(tm5.tm_sec, 15);
	TEST_ASSERT_EQUALS(tm5.tm_wday, 4);
	TEST_ASSERT_EQUALS(tm5.tm_yday, 315);
	TEST_ASSERT_EQUALS(tm5.tm_isdst, 0);

	const auto tv5 = dt5.timeval();
	TEST_ASSERT_EQUALS(tv5.tv_sec, 1289567655);
	TEST_ASSERT_EQUALS(tv5.tv_usec, 16000);

	const auto dt6 = modm::DateTime::from_tm(tm5);
	TEST_ASSERT_EQUALS(dt5.time_t(), dt6.time_t());
	TEST_ASSERT_EQUALS(dt5.weekday(), dt6.weekday());

	const auto dt7 = modm::DateTime::from_timeval(tv5);
	TEST_ASSERT_EQUALS(dt5, dt7);

	const auto dt8 = modm::DateTime(2012y / std::chrono::April / 15d);
	TEST_ASSERT_EQUALS(dt8.time_t(), 1334448000);
	TEST_ASSERT_EQUALS(dt8.day_of_year().count(), 105);
	TEST_ASSERT_EQUALS(dt8.year(), 2012y);
	TEST_ASSERT_EQUALS(dt8.month(), std::chrono::April);
	TEST_ASSERT_EQUALS(dt8.day(), 15d);
	TEST_ASSERT_EQUALS(dt8.weekday(), std::chrono::Sunday);
	TEST_ASSERT_EQUALS(dt8.hours(), 0h);
	TEST_ASSERT_EQUALS(dt8.minutes(), 0min);
	TEST_ASSERT_EQUALS(dt8.seconds(), 0s);
	TEST_ASSERT_EQUALS(dt8.subseconds(), 0ms);

	const auto dt9 = modm::DateTime(25d / std::chrono::July / 2015y, 15h, 25min, 23s, 780ms);
	TEST_ASSERT_EQUALS(dt9.time_t(), 1437837923);
	TEST_ASSERT_EQUALS(dt9.day_of_year().count(), 205);
	TEST_ASSERT_EQUALS(dt9.year(), 2015y);
	TEST_ASSERT_EQUALS(dt9.month(), std::chrono::July);
	TEST_ASSERT_EQUALS(dt9.day(), 25d);
	TEST_ASSERT_EQUALS(dt9.weekday(), std::chrono::Saturday);
	TEST_ASSERT_EQUALS(dt9.hours(), 15h);
	TEST_ASSERT_EQUALS(dt9.minutes(), 25min);
	TEST_ASSERT_EQUALS(dt9.seconds(), 23s);
	TEST_ASSERT_EQUALS(dt9.subseconds(), 780ms);

	const auto dt10 = modm::DateTime::from_time_t(1181611501);
	TEST_ASSERT_EQUALS(dt10.time_t(), 1181611501);
	TEST_ASSERT_EQUALS(dt10.day_of_year().count(), 162);
	TEST_ASSERT_EQUALS(dt10.year(), 2007y);
	TEST_ASSERT_EQUALS(dt10.month(), std::chrono::June);
	TEST_ASSERT_EQUALS(dt10.day(), 12d);
	TEST_ASSERT_EQUALS(dt10.weekday(), std::chrono::Tuesday);
	TEST_ASSERT_EQUALS(dt10.hours(), 01h);
	TEST_ASSERT_EQUALS(dt10.minutes(), 25min);
	TEST_ASSERT_EQUALS(dt10.seconds(), 01s);
	TEST_ASSERT_EQUALS(dt10.subseconds(), 0ms);
}
