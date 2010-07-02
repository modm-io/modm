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
 * $Id: vector_2d_test.cpp -1   $
 */
// ----------------------------------------------------------------------------

#include <cmath>
#include <xpcc/math/geometry/angle.hpp>
#include <xpcc/math/geometry/vector_2d.hpp>

#include "vector_2d_test.hpp"

void
Vector2DTest::testConstructor()
{
	xpcc::Vector2D<int16_t> a;
	
	TEST_ASSERT_EQUALS(a.getX(), 0);
	TEST_ASSERT_EQUALS(a.getY(), 0);
	
	a.setX(100);
	TEST_ASSERT_EQUALS(a.getX(), 100);
	
	a.setY(100);
	TEST_ASSERT_EQUALS(a.getY(), 100);
	
	a.set(10, -30);
	TEST_ASSERT_EQUALS(a.getX(), 10);
	TEST_ASSERT_EQUALS(a.getY(), -30);
	
	
	xpcc::Vector2D<int16_t> b(-20, -10);
	
	TEST_ASSERT_EQUALS(b.getX(), -20);
	TEST_ASSERT_EQUALS(b.getY(), -10);
}

void
Vector2DTest::testLength()
{
	xpcc::Vector2D<int16_t> a;
	
	a.set(100, 100);
	TEST_ASSERT_EQUALS(a.getLength(), 141);
	TEST_ASSERT_EQUALS(a.getLengthSquared(), 20000);
	
	a.set(-100, -100);
	TEST_ASSERT_EQUALS(a.getLength(), 141);
	TEST_ASSERT_EQUALS(a.getLengthSquared(), 20000);
	
	a.set(0, 100);
	TEST_ASSERT_EQUALS(a.getLength(), 100);
	TEST_ASSERT_EQUALS(a.getLengthSquared(), 10000);
	
	a.set(-20, 300);
	TEST_ASSERT_EQUALS(a.getLength(), 301);
	TEST_ASSERT_EQUALS(a.getLengthSquared(), 90400);
}

void
Vector2DTest::testLengthFloat()
{
	xpcc::Vector2D<float> a;
	
	a.set(100, 100);
	TEST_ASSERT_EQUALS_FLOAT(a.getLength(), 141.4213562f);
	TEST_ASSERT_EQUALS_FLOAT(a.getLengthSquared(), 20000);
	
	a.set(-20, 300);
	TEST_ASSERT_EQUALS_FLOAT(a.getLength(), 300.6659276f);
	TEST_ASSERT_EQUALS_FLOAT(a.getLengthSquared(), 90400);
}

void
Vector2DTest::testAngle()
{
	xpcc::Vector2D<int16_t> a;
	
	a.set(100, 100);
	TEST_ASSERT_EQUALS_FLOAT(a.getAngle(), M_PI / 4);
	
	a.set(-100, -100);
	TEST_ASSERT_EQUALS_FLOAT(a.getAngle(), - 3* M_PI / 4);
	
	a.set(0, 100);
	TEST_ASSERT_EQUALS_FLOAT(a.getAngle(), M_PI / 2);
}

void
Vector2DTest::testNormalize()
{
	xpcc::Vector2D<float> a(100, 100);
	
	TEST_ASSERT_EQUALS_FLOAT(a.getAngle(), M_PI / 4);
	
	a.normalize();
	
	TEST_ASSERT_EQUALS_FLOAT(a.getLength(), 1.f);
	TEST_ASSERT_EQUALS_FLOAT(a.getAngle(), M_PI / 4);
}

void
Vector2DTest::testScale()
{
	xpcc::Vector2D<int16_t> a(100, 200);
	
	a.scale(0.15f);
	
	TEST_ASSERT_EQUALS(a.getX(), 15);
	TEST_ASSERT_EQUALS(a.getY(), 30);
	
	a.scale(static_cast<int16_t>(7));
	
	TEST_ASSERT_EQUALS(a.getX(), 105);
	TEST_ASSERT_EQUALS(a.getY(), 210);
}

void
Vector2DTest::testRotate()
{
	xpcc::Vector2D<int16_t> a(100, 200);
	
	a.rotate(M_PI / 2);
	
	TEST_ASSERT_EQUALS(a.getX(), -200);
	TEST_ASSERT_EQUALS(a.getY(), 100);
	
	a.rotate(-M_PI);
	
	TEST_ASSERT_EQUALS(a.getX(), 200);
	TEST_ASSERT_EQUALS(a.getY(), -100);
	
	a.set(100, 100);
	a.rotate(xpcc::Angle::toRadian(20));
	
	TEST_ASSERT_EQUALS(a.getX(), 60);
	TEST_ASSERT_EQUALS(a.getY(), 128);
	
	a.set(20, 10);
	a.rotate(-M_PI / 2);
	
	TEST_ASSERT_EQUALS(a.getX(), 10);
	TEST_ASSERT_EQUALS(a.getY(), -20);
}

void
Vector2DTest::testRotateFloat()
{
	xpcc::Vector2D<float> a(100, 100);
	
	a.rotate(xpcc::Angle::toRadian(20));
	
	TEST_ASSERT_EQUALS_FLOAT(a.getX(), 59.76724775f);
	TEST_ASSERT_EQUALS_FLOAT(a.getY(), 128.1712764f);
}

void
Vector2DTest::testConversion()
{
	xpcc::Vector2D<float> a(12.763f, -13.3123f);
	
	TEST_ASSERT_EQUALS(a.getX(), 12.763f);
	TEST_ASSERT_EQUALS(a.getY(), -13.3123f);
	
	xpcc::Vector2D<int16_t> b = a.convert<int16_t>();
	
	TEST_ASSERT_EQUALS(b.getX(), 13);
	TEST_ASSERT_EQUALS(b.getY(), -13);
	
	xpcc::Point2D<int16_t> c = b.toPoint();
	
	TEST_ASSERT_EQUALS(c.getX(), 13);
	TEST_ASSERT_EQUALS(c.getY(), -13);
}

void
Vector2DTest::testArithmetics()
{
	xpcc::Vector2D<int16_t> x(100, 100);
	xpcc::Vector2D<int16_t> y(100, 100);
	xpcc::Vector2D<int16_t> z;
	
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
	
	z = -y;
	
	TEST_ASSERT_EQUALS(y.getX(), 10);
	TEST_ASSERT_EQUALS(y.getY(), 30);
	
	TEST_ASSERT_EQUALS(z.getX(), -10);
	TEST_ASSERT_EQUALS(z.getY(), -30);
}

void
Vector2DTest::testComparision()
{
	xpcc::Vector2D<int16_t> a(10, 10);	
	xpcc::Vector2D<int16_t> b(10, -10);
	xpcc::Vector2D<int16_t> c(10, 10);
	
	TEST_ASSERT_TRUE(a != b);
	TEST_ASSERT_FALSE(a == b);
	
	TEST_ASSERT_TRUE(a == c);
	TEST_ASSERT_FALSE(a != c);
}

void
Vector2DTest::testDotProduct()
{
	xpcc::Vector2D<int16_t> a(1000, 1000);	
	xpcc::Vector2D<int16_t> b(1000, 0);
	xpcc::Vector2D<int16_t> c(1000, -1000);
	
	TEST_ASSERT_EQUALS(xpcc::Vector2D<int16_t>::dotProduct(a, b), 1000000);
	TEST_ASSERT_EQUALS(xpcc::Vector2D<int16_t>::dotProduct(a, c), 0);
	TEST_ASSERT_EQUALS(xpcc::Vector2D<int16_t>::dotProduct(b, c), 1000000);
}
