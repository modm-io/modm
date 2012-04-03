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

#include <xpcc/driver/time/time.hpp>

#include "time_test.hpp"

// ----------------------------------------------------------------------------
void
TimeTest::testConversionToUnixTime()
{
	xpcc::Date date;
	
	// 01:46:40 UTC on 9 September 2001
	date.second = 40;
	date.minute = 46;
	date.hour = 1;
	date.day = 9;
	date.month = 8;
	date.year = 2001 - 1900;
	date.dayOfTheWeek = 0;
	date.dayOfTheYear = 251;
	
	TEST_ASSERT_EQUALS(date.toUnixTimestamp(), 1000000000UL);
}

// ----------------------------------------------------------------------------
void
TimeTest::testConversionToUnixTime2()
{
	xpcc::Date date;
	
	//13:37:04 UTC on 10 January 2004
	date.second = 4;
	date.minute = 37;
	date.hour = 13;
	date.day = 10;
	date.month = 0;
	date.year = 2004 - 1900;
	date.dayOfTheWeek = 6;
	date.dayOfTheYear = 9;
	
	TEST_ASSERT_EQUALS(date.toUnixTimestamp(), 1073741824UL);
}

// ----------------------------------------------------------------------------
void
TimeTest::testConversionToUnixTime3()
{
	xpcc::Date date;
	
	// 03:42:01 UTC on 27 July 2011
	date.second = 1;
	date.minute = 42;
	date.hour = 3;
	date.day = 27;
	date.month = 6;
	date.year = 2011 - 1900;
	date.dayOfTheWeek = 3;
	date.dayOfTheYear = 207;
	
	TEST_ASSERT_EQUALS(date.toUnixTimestamp(), 1311738121UL);
}

// ----------------------------------------------------------------------------
void
TimeTest::testConversionToUnixTime4()
{
	xpcc::Date date;
	
	date.second = 44;
	date.minute = 14;
	date.hour = 1;
	date.day = 2;
	date.month = 3;
	date.year = 2012 - 1900;
	date.dayOfTheWeek = 1;
	date.dayOfTheYear = 92;
	
	TEST_ASSERT_EQUALS(date.toUnixTimestamp(), 1333329284UL);
}

// ----------------------------------------------------------------------------
void
TimeTest::testConversionToUnixTime5()
{
	xpcc::Date date;
	
	// 00:37:33 UTC on 21 July 2069
	date.second = 33;
	date.minute = 37;
	date.hour = 0;
	date.day = 21;
	date.month = 6;
	date.year = 2069 - 1900;
	date.dayOfTheWeek = 0;
	date.dayOfTheYear = 201;
	
	TEST_ASSERT_EQUALS(date.toUnixTimestamp(), 3141592653UL);
}

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
void
TimeTest::testConversionToDate()
{
	xpcc::Date date;
	xpcc::UnixTime(1000000000UL).toDate(&date);
	
	TEST_ASSERT_EQUALS(date.second, 40);
	TEST_ASSERT_EQUALS(date.minute, 46);
	TEST_ASSERT_EQUALS(date.hour, 1);
	TEST_ASSERT_EQUALS(date.day, 9);
	TEST_ASSERT_EQUALS(date.month, 8);
	TEST_ASSERT_EQUALS(date.year, 2001 - 1900);
	TEST_ASSERT_EQUALS(date.dayOfTheWeek, 0);
	TEST_ASSERT_EQUALS(date.dayOfTheYear, 251);
}

// ----------------------------------------------------------------------------
void
TimeTest::testConversionToDate2()
{
	xpcc::Date date;
	xpcc::UnixTime(1073741824UL).toDate(&date);
	
	TEST_ASSERT_EQUALS(date.second, 4);
	TEST_ASSERT_EQUALS(date.minute, 37);
	TEST_ASSERT_EQUALS(date.hour, 13);
	TEST_ASSERT_EQUALS(date.day, 10);
	TEST_ASSERT_EQUALS(date.month, 0);
	TEST_ASSERT_EQUALS(date.year, 2004 - 1900);
	TEST_ASSERT_EQUALS(date.dayOfTheWeek, 6);
	TEST_ASSERT_EQUALS(date.dayOfTheYear, 9);
}

// ----------------------------------------------------------------------------
void
TimeTest::testConversionToDate3()
{
	xpcc::Date date;
	xpcc::UnixTime(1311738121UL).toDate(&date);
	
	TEST_ASSERT_EQUALS(date.second, 1);
	TEST_ASSERT_EQUALS(date.minute, 42);
	TEST_ASSERT_EQUALS(date.hour, 3);
	TEST_ASSERT_EQUALS(date.day, 27);
	TEST_ASSERT_EQUALS(date.month, 6);
	TEST_ASSERT_EQUALS(date.year, 2011 - 1900);
	TEST_ASSERT_EQUALS(date.dayOfTheWeek, 3);
	TEST_ASSERT_EQUALS(date.dayOfTheYear, 207);
}

// ----------------------------------------------------------------------------
void
TimeTest::testConversionToDate4()
{
	xpcc::Date date;
	xpcc::UnixTime(1333329284UL).toDate(&date);
	
	TEST_ASSERT_EQUALS(date.second, 44);
	TEST_ASSERT_EQUALS(date.minute, 14);
	TEST_ASSERT_EQUALS(date.hour, 1);
	TEST_ASSERT_EQUALS(date.day, 2);
	TEST_ASSERT_EQUALS(date.month, 3);
	TEST_ASSERT_EQUALS(date.year, 2012 - 1900);
	TEST_ASSERT_EQUALS(date.dayOfTheWeek, 1);
	TEST_ASSERT_EQUALS(date.dayOfTheYear, 92);
}

// ----------------------------------------------------------------------------
void
TimeTest::testConversionToDate5()
{
	xpcc::Date date;
	xpcc::UnixTime(3141592653UL).toDate(&date);
	
	// 00:37:33 UTC on 21 July 2069
	TEST_ASSERT_EQUALS(date.second, 33);
	TEST_ASSERT_EQUALS(date.minute, 37);
	TEST_ASSERT_EQUALS(date.hour, 0);
	TEST_ASSERT_EQUALS(date.day, 21);
	TEST_ASSERT_EQUALS(date.month, 6);
	TEST_ASSERT_EQUALS(date.year, 2069 - 1900);
	TEST_ASSERT_EQUALS(date.dayOfTheWeek, 0);
	TEST_ASSERT_EQUALS(date.dayOfTheYear, 201);
}
