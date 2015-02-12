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

#include <xpcc/processing/timestamp.hpp>
#include <xpcc/utils/arithmetic_traits.hpp>

#include "timestamp_test.hpp"

// ----------------------------------------------------------------------------
void
TimestampTest::testConstructors()
{
	xpcc::ShortTimestamp t1Short;
	TEST_ASSERT_TRUE(t1Short == 0);

	xpcc::Timestamp t1;
	TEST_ASSERT_TRUE(t1 == 0);

	xpcc::ShortTimestamp t2Short(1000);
	TEST_ASSERT_TRUE(t2Short == 1000);

	xpcc::Timestamp t2(1000);
	TEST_ASSERT_TRUE(t2 == 1000);

	t1Short = 500;
	TEST_ASSERT_TRUE(t1Short == 500);

	t1 = 500;
	TEST_ASSERT_TRUE(t1 == 500);
}

void
TimestampTest::testArithmetics()
{
	xpcc::ShortTimestamp t1Short(200);
	xpcc::ShortTimestamp t2Short(500);
	xpcc::ShortTimestamp t3Short;

	t3Short = t1Short + t2Short;
	TEST_ASSERT_TRUE(t3Short == 700);

	xpcc::Timestamp t1(200);
	xpcc::Timestamp t2(500);
	xpcc::Timestamp t3;

	t3 = t1 + t2;
	TEST_ASSERT_TRUE(t3 == 700);
}

void
TimestampTest::testComparisons()
{
	xpcc::ShortTimestamp t1Short;
	xpcc::ShortTimestamp t2Short;
	xpcc::Timestamp t1;
	xpcc::Timestamp t2;

	TEST_ASSERT_TRUE(t1Short == t2Short);
	TEST_ASSERT_FALSE(t1Short != t2Short);
	TEST_ASSERT_TRUE(t1 == t2);
	TEST_ASSERT_FALSE(t1 != t2);

	t1Short = xpcc::ArithmeticTraits<xpcc::ShortTimestamp::Type>::max / 2;
	t1 = xpcc::ArithmeticTraits<xpcc::Timestamp::Type>::max / 2;

	TEST_ASSERT_FALSE(t1Short == t2Short);
	TEST_ASSERT_TRUE(t1Short != t2Short);
	TEST_ASSERT_FALSE(t1 == t2);
	TEST_ASSERT_TRUE(t1 != t2);

	TEST_ASSERT_TRUE(t1Short > t2Short);
	TEST_ASSERT_TRUE(t1Short >= t2Short);
	TEST_ASSERT_TRUE(t1 > t2);
	TEST_ASSERT_TRUE(t1 >= t2);

	t1Short = xpcc::ArithmeticTraits<xpcc::ShortTimestamp::Type>::max / 2 + 1;
	t1 = xpcc::ArithmeticTraits<xpcc::Timestamp::Type>::max / 2 + 1;

	TEST_ASSERT_TRUE(t1Short < t2Short);
	TEST_ASSERT_TRUE(t1Short <= t2Short);
	TEST_ASSERT_TRUE(t1 < t2);
	TEST_ASSERT_TRUE(t1 <= t2);
}
