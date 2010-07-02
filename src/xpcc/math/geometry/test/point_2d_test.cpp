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

#include <cmath>
#include <xpcc/math/geometry/point_2d.hpp>

#include "point_2d_test.hpp"

void
Point2DTest::testDefaultConstructor()
{
	xpcc::Point2D<int16_t> a;
	
	TEST_ASSERT_EQUALS(a.getX(), 0);
	TEST_ASSERT_EQUALS(a.getY(), 0);
	
	a.setX(100);
	TEST_ASSERT_EQUALS(a.getX(), 100);
	
	a.setY(100);
	TEST_ASSERT_EQUALS(a.getY(), 100);
	
	a.set(10, -30);
	TEST_ASSERT_EQUALS(a.getX(), 10);
	TEST_ASSERT_EQUALS(a.getY(), -30);
}

void
Point2DTest::testConstructor()
{
	xpcc::Point2D<int16_t> a(-20, -10);
	
	TEST_ASSERT_EQUALS(a.getX(), -20);
	TEST_ASSERT_EQUALS(a.getY(), -10);
}

void
Point2DTest::testDistance()
{
	xpcc::Point2D<int16_t> a(10, 10);
	xpcc::Point2D<int16_t> b(20, 20);
	
	TEST_ASSERT_EQUALS(a.getDistanceTo(b), 14);
	
	xpcc::Point2D<int16_t> c(40, 20);
	
	TEST_ASSERT_EQUALS(a.getDistanceTo(c), 32);
}

void
Point2DTest::testTranslate()
{
	xpcc::Point2D<int16_t> a(10, 10);
	xpcc::Vector2D<int16_t> b(20, -20);
	
	a.translate(b);
	
	TEST_ASSERT_EQUALS(a.getX(), 30);
	TEST_ASSERT_EQUALS(a.getY(), -10);
}

void
Point2DTest::testConversion()
{
	xpcc::Point2D<float> a(-10.49913f, 20.7521f);
	
	xpcc::Point2D<int16_t> b = a.convert<int16_t>();
	
	TEST_ASSERT_EQUALS(b.getX(), -10);
	TEST_ASSERT_EQUALS(b.getY(), 21);
	
	xpcc::Point2D<float> c = a.convert<float>();
	
	TEST_ASSERT_EQUALS_FLOAT(c.getX(), -10.f);
	TEST_ASSERT_EQUALS_FLOAT(c.getY(), 21.f);
	
	xpcc::Vector2D<int16_t> d = b.toVector();
	
	TEST_ASSERT_EQUALS(d.getX(), -10);
	TEST_ASSERT_EQUALS(d.getY(), 21);
}

void
Point2DTest::testComparision()
{
	xpcc::Point2D<int16_t> a(10, 10);	
	xpcc::Point2D<int16_t> b(10, -10);
	xpcc::Point2D<int16_t> c(10, 10);
	
	TEST_ASSERT_TRUE(a != b);
	TEST_ASSERT_FALSE(a == b);
	
	TEST_ASSERT_TRUE(a == c);
	TEST_ASSERT_FALSE(a != c);
}

void
Point2DTest::testCCW()
{
	TEST_FAIL("TODO");
}
