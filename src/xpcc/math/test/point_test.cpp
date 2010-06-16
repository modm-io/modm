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

#include <math.h>
#include <xpcc/math/point.hpp>

#include "point_test.hpp"

void
PointTest::testConstructor()
{
	xpcc::Point<int16_t> coord;
	
	TEST_ASSERT_EQUALS(coord.getX(), 0);
	TEST_ASSERT_EQUALS(coord.getY(), 0);
	
	coord.setX(100);
	TEST_ASSERT_EQUALS(coord.getX(), 100);
	
	coord.setY(100);
	TEST_ASSERT_EQUALS(coord.getY(), 100);
	
	coord.set(10, -30);
	TEST_ASSERT_EQUALS(coord.getX(), 10);
	TEST_ASSERT_EQUALS(coord.getY(), -30);
}

void
PointTest::testLengthAndAngle()
{
	xpcc::Angle angle;
	xpcc::Point<int16_t> coord(100, 100);
	
	TEST_ASSERT_EQUALS(coord.getLength(), 141);
	angle = coord.getAngle();
	TEST_ASSERT_EQUALS_FLOAT(angle.toFloat(), M_PI / 4);
	
	coord.set(-100, -100);
	
	TEST_ASSERT_EQUALS(coord.getLength(), 141);
	angle = coord.getAngle();
	TEST_ASSERT_EQUALS_FLOAT(angle.toFloat(), - 3* M_PI / 4);
	
	coord.set(0, 100);
	TEST_ASSERT_EQUALS(coord.getLength(), 100);
	angle = coord.getAngle();
	TEST_ASSERT_EQUALS_FLOAT(angle.toFloat(), M_PI / 2);
	
}

void
PointTest::testRotation()
{
	xpcc::Point<int16_t> coord(100, 200);
	
	coord.rotate(M_PI / 2);
	
	TEST_ASSERT_EQUALS(coord.getX(), -200);
	TEST_ASSERT_EQUALS(coord.getY(), 100);
	
	coord.rotate(-M_PI);
	
	TEST_ASSERT_EQUALS(coord.getX(), 200);
	TEST_ASSERT_EQUALS(coord.getY(), -100);
}

void
PointTest::testArithmetics()
{
	xpcc::Point<int16_t> x(100, 100);
	xpcc::Point<int16_t> y(100, 100);
	xpcc::Point<int16_t> z;
	
	x += y;
	
	TEST_ASSERT_EQUALS(x.getX(), 200);
	TEST_ASSERT_EQUALS(x.getY(), 200);
	
	TEST_ASSERT_EQUALS(y.getX(), 100);
	TEST_ASSERT_EQUALS(y.getY(), 100);
	
	y.set(10, 30);
	
	x -= y;
	
	TEST_ASSERT_EQUALS(x.getX(), 190);
	TEST_ASSERT_EQUALS(x.getY(), 170);
	
	TEST_ASSERT_EQUALS(y.getX(), 10);
	TEST_ASSERT_EQUALS(y.getY(), 30);
	
	z = x - y;
	
	TEST_ASSERT_EQUALS(z.getX(), 180);
	TEST_ASSERT_EQUALS(z.getY(), 140);
	
	TEST_ASSERT_EQUALS(x.getX(), 190);
	TEST_ASSERT_EQUALS(x.getY(), 170);
	
	TEST_ASSERT_EQUALS(y.getX(), 10);
	TEST_ASSERT_EQUALS(y.getY(), 30);
	
	z = x + y;
	
	TEST_ASSERT_EQUALS(z.getX(), 200);
	TEST_ASSERT_EQUALS(z.getX(), 200);
	
	TEST_ASSERT_EQUALS(x.getX(), 190);
	TEST_ASSERT_EQUALS(x.getY(), 170);
	
	TEST_ASSERT_EQUALS(y.getX(), 10);
	TEST_ASSERT_EQUALS(y.getY(), 30);
}

