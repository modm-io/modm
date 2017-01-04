/*
 * Copyright (c) 2011, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/math/geometry/vector.hpp>
#include "vector2_test.hpp"

void
Vector2Test::testConstructor()
{
	xpcc::Vector2i a;
	TEST_ASSERT_EQUALS(a.getX(), 0);
	TEST_ASSERT_EQUALS(a.getY(), 0);
	
	a.setX(100);
	a.setY(9);
	TEST_ASSERT_EQUALS(a.getX(), 100);
	TEST_ASSERT_EQUALS(a.getY(), 9);
	
	xpcc::Vector2i b(20);
	TEST_ASSERT_EQUALS(b.getX(), 20);
	TEST_ASSERT_EQUALS(b.getY(), 20);
	
	xpcc::Vector2i c(20,30);
	TEST_ASSERT_EQUALS(c.getX(), 20);
	TEST_ASSERT_EQUALS(c.getY(), 30);
	
	int16_t array[2] = {-4,5};
	xpcc::Matrix<int16_t, 2, 1> m(array);
	xpcc::Vector2i d(m);
	TEST_ASSERT_EQUALS(d.getX(), -4);
	TEST_ASSERT_EQUALS(d.getY(), 5);
	
	xpcc::Vector2i e(a);
	TEST_ASSERT_EQUALS(e.getX(), 100);
	TEST_ASSERT_EQUALS(e.getY(), 9);
	
	xpcc::Vector1i f(4);
	
	xpcc::Vector2i g(1,f);
	TEST_ASSERT_EQUALS(g.getX(), 1);
	TEST_ASSERT_EQUALS(g.getY(), 4);
	xpcc::Vector2i h(f,5);
	TEST_ASSERT_EQUALS(h.getX(), 4);
	TEST_ASSERT_EQUALS(h.getY(), 5);
	xpcc::Vector2i i(f,f);
	TEST_ASSERT_EQUALS(i.getX(), 4);
	TEST_ASSERT_EQUALS(i.getY(), 4);
}

void
Vector2Test::testAssign()
{
	xpcc::Vector2i a(42,-4);
	
	int16_t array[2] = {-26,9};
	xpcc::Matrix<int16_t, 2, 1> m(array);
	
	xpcc::Vector2i b;
	
	b = a;
	TEST_ASSERT_EQUALS(b.getX(), 42);
	TEST_ASSERT_EQUALS(b.getY(), -4);
	
	b = m;
	TEST_ASSERT_EQUALS(b.getX(), -26);
	TEST_ASSERT_EQUALS(b.getY(), 9);
}

void
Vector2Test::testCompare()
{
	xpcc::Vector2i a(4,3);
	xpcc::Vector2i b(4,5);
	xpcc::Vector2i c(4,5);
	// ==
	TEST_ASSERT_TRUE(b == c);
	TEST_ASSERT_FALSE(a == c);
	// !=
	TEST_ASSERT_TRUE(a != c);
	TEST_ASSERT_FALSE(b != c);
	// <
	TEST_ASSERT_TRUE(a < c);
	TEST_ASSERT_FALSE(b < c);
	TEST_ASSERT_FALSE(c < a);
	// <=
	TEST_ASSERT_TRUE(a <= c);
	TEST_ASSERT_TRUE(b <= c);
	TEST_ASSERT_FALSE(c <= a);
	// >
	TEST_ASSERT_TRUE(c > a);
	TEST_ASSERT_FALSE(b > c);
	TEST_ASSERT_FALSE(a > c);
	// >=
	TEST_ASSERT_TRUE(c >= a);
	TEST_ASSERT_TRUE(b >= c);
	TEST_ASSERT_FALSE(a >= c);
}

void
Vector2Test::testRawDataAccess()
{
	xpcc::Vector2i a(2,5);
	int16_t *pointer = a.ptr();
	
	TEST_ASSERT_EQUALS(a[0], 2);
	TEST_ASSERT_EQUALS(a[1], 5);
	TEST_ASSERT_EQUALS(pointer[0], 2);
	TEST_ASSERT_EQUALS(pointer[1], 5);
}

void
Vector2Test::testOperators()
{
	xpcc::Vector2i a(7,5);
	xpcc::Vector2i b(-18,7);
	xpcc::Vector2i c;
	
	TEST_ASSERT_EQUALS((a + b).getX(), 7-18);
	TEST_ASSERT_EQUALS((a + b).getY(), 5+7);
	TEST_ASSERT_EQUALS((a - b).getX(), 7-(-18));
	TEST_ASSERT_EQUALS((a - b).getY(), 5-7);
	TEST_ASSERT_EQUALS((a * b), 7*(-18)+5*7);
	TEST_ASSERT_EQUALS((a ^ b), 7*7-(-18)*5);
	TEST_ASSERT_EQUALS((a * 3).getX(), 7*3);
	TEST_ASSERT_EQUALS((a * 3).getY(), 5*3);
	TEST_ASSERT_EQUALS((3 * a).getX(), 3*7);
	TEST_ASSERT_EQUALS((3 * a).getY(), 3*5);
	TEST_ASSERT_EQUALS((b / 2).getX(), -18/2);
	TEST_ASSERT_EQUALS((b / 2).getY(), 4);	// 3.5 -> rounded 4
	
	-b;
	TEST_ASSERT_EQUALS(b.getX(), -18);
	TEST_ASSERT_EQUALS(b.getY(),   7);
	b = -b;
	TEST_ASSERT_EQUALS(b.getX(), 18);
	TEST_ASSERT_EQUALS(b.getY(), -7);
	a += b;
	TEST_ASSERT_EQUALS(a.getX(), 7+18);
	TEST_ASSERT_EQUALS(a.getY(), 5-7);
	a -= b;
	TEST_ASSERT_EQUALS(a.getX(), 7+18-18);
	TEST_ASSERT_EQUALS(a.getY(), 5-7-(-7));
	
	c = a - b;
	TEST_ASSERT_EQUALS(a.getX(), 7);
	TEST_ASSERT_EQUALS(a.getY(), 5);
	TEST_ASSERT_EQUALS(b.getX(), 18);
	TEST_ASSERT_EQUALS(b.getY(), -7);
	TEST_ASSERT_EQUALS(c.getX(), -11);
	TEST_ASSERT_EQUALS(c.getY(), 12);
	
	c = a + b;
	TEST_ASSERT_EQUALS(a.getX(), 7);
	TEST_ASSERT_EQUALS(a.getY(), 5);
	TEST_ASSERT_EQUALS(b.getX(), 18);
	TEST_ASSERT_EQUALS(b.getY(), -7);
	TEST_ASSERT_EQUALS(c.getX(), 25);
	TEST_ASSERT_EQUALS(c.getY(), -2);
	
	a *= 2;
	TEST_ASSERT_EQUALS(a.getX(), 7*2);
	TEST_ASSERT_EQUALS(a.getY(), 5*2);
	b /= 2;
	TEST_ASSERT_EQUALS(b.getX(), 18/2);
	TEST_ASSERT_EQUALS(b.getY(), -7/2);
	~b;
	TEST_ASSERT_EQUALS(b.getX(), -7/2);
	TEST_ASSERT_EQUALS(b.getY(), -18/2);
	
	// test division of floats
	xpcc::Vector2f d(-18.7f,5.5f);
	TEST_ASSERT_EQUALS_FLOAT((d / 2.4f).getX(), -7.7916666667);
	TEST_ASSERT_EQUALS_FLOAT((d / 2.4f).getY(), 2.2916666667);
	d /= 7.5f;
	TEST_ASSERT_EQUALS_FLOAT(d.getX(), -2.4933333333);
	TEST_ASSERT_EQUALS_FLOAT(d.getY(), 0.7333333333);
}

void
Vector2Test::testLengthInteger()
{
	xpcc::Vector2i a;
	
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
Vector2Test::testLength()
{
	xpcc::Vector2f a(3.f,4.f);
	
	TEST_ASSERT_EQUALS_FLOAT(a.getLengthSquared(), 3.f*3.f+4.f*4.f);
	TEST_ASSERT_EQUALS_FLOAT(a.getLength(), 5.f);
	
	TEST_ASSERT_EQUALS_FLOAT(a.scaled(2.5f).getX(), 1.5f);
	TEST_ASSERT_EQUALS_FLOAT(a.scaled(2.5f).getY(), 2.f);
	a.scale(2.f);
	TEST_ASSERT_EQUALS_FLOAT(a.getX(), 1.2f);
	TEST_ASSERT_EQUALS_FLOAT(a.getY(), 1.6f);
	
	TEST_ASSERT_EQUALS_FLOAT(a.normalized().getX(), 0.6f);
	TEST_ASSERT_EQUALS_FLOAT(a.normalized().getY(), 0.8f);
	a.normalize();
	TEST_ASSERT_EQUALS_FLOAT(a.getX(), 0.6f);
	TEST_ASSERT_EQUALS_FLOAT(a.getY(), 0.8f);
	
	xpcc::Vector2f b(a.perpendicular());
	TEST_ASSERT_EQUALS_FLOAT(b.getX(), 0.8f);
	TEST_ASSERT_EQUALS_FLOAT(b.getY(), -0.6f);
}

void
Vector2Test::testNormalize()
{
	xpcc::Vector<float, 2> a(100, 100);
	
	TEST_ASSERT_EQUALS_FLOAT(a.getAngle(), M_PI / 4);
	
	a.normalize();
	
	TEST_ASSERT_EQUALS_FLOAT(a.getLength(), 1.f);
	TEST_ASSERT_EQUALS_FLOAT(a.getAngle(), M_PI / 4);
}

void
Vector2Test::testScale()
{
	xpcc::Vector2i a(100, 200);
	
	a.scale(100);
	
	TEST_ASSERT_EQUALS(a.getX(), 44);
	TEST_ASSERT_EQUALS(a.getY(), 89);
	
	a.scale(static_cast<int16_t>(30));
	
	TEST_ASSERT_EQUALS(a.getX(), 13);
	TEST_ASSERT_EQUALS(a.getY(), 26);
}

void
Vector2Test::testRotate()
{
	xpcc::Vector2i a(100, 200);
	
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
Vector2Test::testRotateFloat()
{
	xpcc::Vector<float, 2> a(100, 100);
	
	a.rotate(xpcc::Angle::toRadian(20));
	
	TEST_ASSERT_EQUALS_FLOAT(a.getX(), 59.76724775f);
	TEST_ASSERT_EQUALS_FLOAT(a.getY(), 128.1712764f);
}

void
Vector2Test::testTranslate()
{
	xpcc::Vector2i a(10, 10);
	xpcc::Vector2i b(20, -20);
	
	a.translate(b);
	
	TEST_ASSERT_EQUALS(a.getX(), 30);
	TEST_ASSERT_EQUALS(a.getY(), -10);
}

void
Vector2Test::testConversion()
{
	xpcc::Vector<float, 2> a(12.763f, -13.3123f);
	
	TEST_ASSERT_EQUALS(a.getX(), 12.763f);
	TEST_ASSERT_EQUALS(a.getY(), -13.3123f);
	
	xpcc::Vector2i b = a.convert<int16_t>();
	
	TEST_ASSERT_EQUALS(b.getX(), 13);
	TEST_ASSERT_EQUALS(b.getY(), -13);
}

void
Vector2Test::testMathDefs()
{
	xpcc::Vector2i a(7,0);
	xpcc::Vector2f b(INFINITY,NAN);
	
	TEST_ASSERT_FALSE(a.hasNan());
	TEST_ASSERT_FALSE(a.hasInf());
	
	TEST_ASSERT_TRUE(b.hasNan());
	TEST_ASSERT_TRUE(b.hasInf());
}

void
Vector2Test::testAngle()
{
	xpcc::Vector2i a;
	
	a.set(100, 100);
	TEST_ASSERT_EQUALS_FLOAT(a.getAngle(), M_PI / 4);
	
	a.set(-100, -100);
	TEST_ASSERT_EQUALS_FLOAT(a.getAngle(), - 3* M_PI / 4);
	
	a.set(0, 100);
	TEST_ASSERT_EQUALS_FLOAT(a.getAngle(), M_PI / 2);
}

void
Vector2Test::testDotProduct()
{
	xpcc::Vector2i a(1000, 1000);	
	xpcc::Vector2i b(1000, 0);
	xpcc::Vector2i c(1000, -1000);
	
	TEST_ASSERT_EQUALS(a.dot(b), 1000000);
	TEST_ASSERT_EQUALS(a.dot(c), 0);
	TEST_ASSERT_EQUALS(b.dot(c), 1000000);
}

void
Vector2Test::testCrossProduct()
{
	xpcc::Vector2i a(10, 20);	
	xpcc::Vector2i b(10, 0);
	xpcc::Vector2i c(15, -30);
	
	TEST_ASSERT_EQUALS(a.cross(b), -200);
	TEST_ASSERT_EQUALS(a.cross(c), -600);
	TEST_ASSERT_EQUALS(b.cross(c), -300);
}

void
Vector2Test::testDistance()
{
	xpcc::Vector2i a(10, 10);
	xpcc::Vector2i b(20, 20);
	
	TEST_ASSERT_EQUALS(a.getDistanceTo(b), 14);
	
	xpcc::Vector2i c(40, 20);
	
	TEST_ASSERT_EQUALS(a.getDistanceTo(c), 32);
}

void
Vector2Test::testCCW()
{
	xpcc::Vector2i a(0, 0);
	xpcc::Vector2i b(30, 30);
	xpcc::Vector2i c(40, 40);
	xpcc::Vector2i d(0, 40);
	
	TEST_ASSERT_EQUALS(xpcc::Vector2i::ccw(a, b, d), 1);
	TEST_ASSERT_EQUALS(xpcc::Vector2i::ccw(b, d, a), 1);
	TEST_ASSERT_EQUALS(xpcc::Vector2i::ccw(b, a, d), -1);
	TEST_ASSERT_EQUALS(xpcc::Vector2i::ccw(a, d, b), -1);
	
	// three points in a strait row
	TEST_ASSERT_EQUALS(xpcc::Vector2i::ccw(a, b, c), 1);
	
	// last point between the two other
	TEST_ASSERT_EQUALS(xpcc::Vector2i::ccw(a, c, b), 0);
	
	// last point before the first
	TEST_ASSERT_EQUALS(xpcc::Vector2i::ccw(b, c, a), -1);
}

