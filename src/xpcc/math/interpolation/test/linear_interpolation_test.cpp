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
 *
 * $Id$
 */
// ----------------------------------------------------------------------------

#include <xpcc/math/interpolation/linear.hpp>

#include "linear_interpolation_test.hpp"

void
LinearInterpolationTest::testInterpolationRam()
{
	typedef xpcc::Pair<int16_t, uint16_t> Point;
	
	Point points[3] =
	{
		{ -10, 50 },
		{  50, 10 },
		{ 100,  0 }
	};
	
	xpcc::interpolation::Linear<Point> value(points, 3);
	
	TEST_ASSERT_EQUALS(value.interpolate(-20), 50U);
	TEST_ASSERT_EQUALS(value.interpolate( 10), 37U);
	TEST_ASSERT_EQUALS(value.interpolate( 30), 24U);
	TEST_ASSERT_EQUALS(value.interpolate( 50), 10U);
	TEST_ASSERT_EQUALS(value.interpolate(150),  0U);
}

typedef xpcc::Pair<uint8_t, int16_t> MyPair;

FLASH(MyPair flashValues[6]) =
{
	{ 30, -200 },
	{ 50, 0 },
	{ 90, 50 },
	{ 150, 2050 },
	{ 200, 3000 },
	{ 220, 20000 }
};

void 
LinearInterpolationTest::testInterpolationFlash()
{
	xpcc::interpolation::Linear<MyPair, xpcc::accessor::Flash> \
		value(xpcc::accessor::asFlash(flashValues), 6);
	
	TEST_ASSERT_EQUALS(value.interpolate(  0),  -200);
	TEST_ASSERT_EQUALS(value.interpolate( 10),  -200);
	TEST_ASSERT_EQUALS(value.interpolate( 30),  -200);
	TEST_ASSERT_EQUALS(value.interpolate( 32),  -180);
	TEST_ASSERT_EQUALS(value.interpolate( 40),  -100);
	TEST_ASSERT_EQUALS(value.interpolate( 90),    50);
	TEST_ASSERT_EQUALS(value.interpolate(100),   383);
	TEST_ASSERT_EQUALS(value.interpolate(110),   716);
	TEST_ASSERT_EQUALS(value.interpolate(120),  1050);
	TEST_ASSERT_EQUALS(value.interpolate(130),  1383);
	TEST_ASSERT_EQUALS(value.interpolate(140),  1716);
	TEST_ASSERT_EQUALS(value.interpolate(150),  2050);
	TEST_ASSERT_EQUALS(value.interpolate(200),  3000);
	TEST_ASSERT_EQUALS(value.interpolate(201),  3850);
	TEST_ASSERT_EQUALS(value.interpolate(203),  5550);
	TEST_ASSERT_EQUALS(value.interpolate(219), 19150);
	TEST_ASSERT_EQUALS(value.interpolate(220), 20000);
	TEST_ASSERT_EQUALS(value.interpolate(230), 20000);
	TEST_ASSERT_EQUALS(value.interpolate(250), 20000);
}
