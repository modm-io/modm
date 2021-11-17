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
	modm::Vector2i a;
	TEST_ASSERT_EQUALS(a.x(), 0);
	TEST_ASSERT_EQUALS(a.y(), 0);

	a = {100, 9};
	TEST_ASSERT_EQUALS(a.x(), 100);
	TEST_ASSERT_EQUALS(a.y(), 9);

	modm::Vector2i b(20);
	TEST_ASSERT_EQUALS(b.x(), 20);
	TEST_ASSERT_EQUALS(b.y(), 20);

	modm::Vector2i c(20,30);
	TEST_ASSERT_EQUALS(c.x(), 20);
	TEST_ASSERT_EQUALS(c.y(), 30);

	const int16_t array[] = {-4,5};
	modm::Matrix<int16_t, 2, 1> m(array);

	/* modm::Vector2i d(m);
	TEST_ASSERT_EQUALS(d.x(), -4);
	TEST_ASSERT_EQUALS(d.y(), 5); */

	modm::Vector2i e(a);
	TEST_ASSERT_EQUALS(e.x(), 100);
	TEST_ASSERT_EQUALS(e.y(), 9);

	modm::Vector1i f(4);

	// TODO implement variadic Constructor
	// modm::Vector2i g(1,f);
	// TEST_ASSERT_EQUALS(g.x(), 1);
	// TEST_ASSERT_EQUALS(g.y(), 4);
	// modm::Vector2i h(f,5);
	// TEST_ASSERT_EQUALS(h.x(), 4);
	// TEST_ASSERT_EQUALS(h.y(), 5);
	// modm::Vector2i i(f,f);
	// TEST_ASSERT_EQUALS(i.x(), 4);
	// TEST_ASSERT_EQUALS(i.y(), 4);
}

void
Vector2Test::testAssign()
{
	modm::Vector2i a(42,-4);

	const int16_t array[] = {-26,9};
	modm::Matrix<int16_t, 2, 1> m(array);

	modm::Vector2i b;

	b = a;
	TEST_ASSERT_EQUALS(b.x(), 42);
	TEST_ASSERT_EQUALS(b.y(), -4);

	/* b = m;
	TEST_ASSERT_EQUALS(b.x(), -26);
	TEST_ASSERT_EQUALS(b.y(), 9); */
}

void
Vector2Test::testCompare()
{
	modm::Vector2i a(4,3);
	modm::Vector2i b(4,5);
	modm::Vector2i c(4,5);
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
	modm::Vector2i a(2,5);
	int16_t *pointer = a.data();

	TEST_ASSERT_EQUALS(a[0], 2);
	TEST_ASSERT_EQUALS(a[1], 5);
	TEST_ASSERT_EQUALS(pointer[0], 2);
	TEST_ASSERT_EQUALS(pointer[1], 5);
}

void
Vector2Test::testOperators()
{
	modm::Vector2i a(7,5);
	modm::Vector2i b(-18,7);
	modm::Vector2i c;

	TEST_ASSERT_EQUALS((a + b).x(), 7-18);
	TEST_ASSERT_EQUALS((a + b).y(), 5+7);
	TEST_ASSERT_EQUALS((a - b).x(), 7-(-18));
	TEST_ASSERT_EQUALS((a - b).y(), 5-7);
	TEST_ASSERT_EQUALS((a * b), 7*(-18)+5*7);
	TEST_ASSERT_EQUALS((a ^ b), 7*7-(-18)*5);
	TEST_ASSERT_EQUALS((a * 3).x(), 7*3);
	TEST_ASSERT_EQUALS((a * 3).y(), 5*3);
	TEST_ASSERT_EQUALS((3 * a).x(), 3*7);
	TEST_ASSERT_EQUALS((3 * a).y(), 3*5);
	TEST_ASSERT_EQUALS((b / 2.0).x(), -18/2);
	TEST_ASSERT_EQUALS((b / 2.0).y(), 4);	// 3.5 -> rounded 4

	-b;
	TEST_ASSERT_EQUALS(b.x(), -18);
	TEST_ASSERT_EQUALS(b.y(),   7);
	b = -b;
	TEST_ASSERT_EQUALS(b.x(), 18);
	TEST_ASSERT_EQUALS(b.y(), -7);
	a += b;
	TEST_ASSERT_EQUALS(a.x(), 7+18);
	TEST_ASSERT_EQUALS(a.y(), 5-7);
	a -= b;
	TEST_ASSERT_EQUALS(a.x(), 7+18-18);
	TEST_ASSERT_EQUALS(a.y(), 5-7-(-7));

	c = a - b;
	TEST_ASSERT_EQUALS(a.x(), 7);
	TEST_ASSERT_EQUALS(a.y(), 5);
	TEST_ASSERT_EQUALS(b.x(), 18);
	TEST_ASSERT_EQUALS(b.y(), -7);
	TEST_ASSERT_EQUALS(c.x(), -11);
	TEST_ASSERT_EQUALS(c.y(), 12);

	c = a + b;
	TEST_ASSERT_EQUALS(a.x(), 7);
	TEST_ASSERT_EQUALS(a.y(), 5);
	TEST_ASSERT_EQUALS(b.x(), 18);
	TEST_ASSERT_EQUALS(b.y(), -7);
	TEST_ASSERT_EQUALS(c.x(), 25);
	TEST_ASSERT_EQUALS(c.y(), -2);

	a *= 2;
	TEST_ASSERT_EQUALS(a.x(), 7*2);
	TEST_ASSERT_EQUALS(a.y(), 5*2);
	b /= 2;
	TEST_ASSERT_EQUALS(b.x(), 18/2);
	TEST_ASSERT_EQUALS(b.y(), -7/2);
	b = ~b;
	TEST_ASSERT_EQUALS(b.x(), -7/2);
	TEST_ASSERT_EQUALS(b.y(), -18/2);

	// test division of floats
	modm::Vector2f d(-18.7f,5.5f);
	TEST_ASSERT_EQUALS_FLOAT((d / 2.4f).x(), -7.7916666667);
	TEST_ASSERT_EQUALS_FLOAT((d / 2.4f).y(), 2.2916666667);
	d /= 7.5f;
	TEST_ASSERT_EQUALS_FLOAT(d.x(), -2.4933333333);
	TEST_ASSERT_EQUALS_FLOAT(d.y(), 0.7333333333);
}

void
Vector2Test::testLengthInteger()
{
	modm::Vector2i a;

	a = {100, 100};
	TEST_ASSERT_EQUALS(a.getLength(), 141);
	TEST_ASSERT_EQUALS(a.getLengthSquared(), 20000);

	a = {-100, -100};
	TEST_ASSERT_EQUALS(a.getLength(), 141);
	TEST_ASSERT_EQUALS(a.getLengthSquared(), 20000);

	a = {0, 100};
	TEST_ASSERT_EQUALS(a.getLength(), 100);
	TEST_ASSERT_EQUALS(a.getLengthSquared(), 10000);

	a = {-20, 300};
	TEST_ASSERT_EQUALS(a.getLength(), 301);
	TEST_ASSERT_EQUALS(a.getLengthSquared(), 90400);
}

void
Vector2Test::testLength()
{
	modm::Vector2f a(3.f,4.f);

	TEST_ASSERT_EQUALS_FLOAT(a.getLengthSquared(), 3.f*3.f+4.f*4.f);
	TEST_ASSERT_EQUALS_FLOAT(a.getLength(), 5.f);

	TEST_ASSERT_EQUALS_FLOAT(a.scaled(2.5f).x(), 1.5f);
	TEST_ASSERT_EQUALS_FLOAT(a.scaled(2.5f).y(), 2.f);

	a.scale(2.f);
	TEST_ASSERT_EQUALS_FLOAT(a.x(), 1.2f);
	TEST_ASSERT_EQUALS_FLOAT(a.y(), 1.6f);

	TEST_ASSERT_EQUALS_FLOAT(a.normalized().x(), 0.6f);
	TEST_ASSERT_EQUALS_FLOAT(a.normalized().y(), 0.8f);

	a.normalize();
	TEST_ASSERT_EQUALS_FLOAT(a.x(), 0.6f);
	TEST_ASSERT_EQUALS_FLOAT(a.y(), 0.8f);

	modm::Vector2f b(a.perpendicular());
	TEST_ASSERT_EQUALS_FLOAT(b.x(), 0.8f);
	TEST_ASSERT_EQUALS_FLOAT(b.y(), -0.6f);
}

void
Vector2Test::testNormalize()
{
	modm::Vector<float, 2> a(100, 100);

	TEST_ASSERT_EQUALS_FLOAT(a.getAngle(), M_PI / 4);

	a.normalize();

	TEST_ASSERT_EQUALS_FLOAT(a.getLength(), 1.f);
	TEST_ASSERT_EQUALS_FLOAT(a.getAngle(), M_PI / 4);
}

void
Vector2Test::testScale()
{
	modm::Vector2i a(100, 200);

	a.scale(100);

	TEST_ASSERT_EQUALS(a.x(), 44);
	TEST_ASSERT_EQUALS(a.y(), 89);

	a.scale(static_cast<int16_t>(30));

	TEST_ASSERT_EQUALS(a.x(), 13);
	TEST_ASSERT_EQUALS(a.y(), 26);
}

void
Vector2Test::testRotate()
{
	modm::Vector2i a(100, 200);

	a.rotate(M_PI / 2);

	TEST_ASSERT_EQUALS(a.x(), -200);
	TEST_ASSERT_EQUALS(a.y(), 100);

	a.rotate(-M_PI);

	TEST_ASSERT_EQUALS(a.x(), 200);
	TEST_ASSERT_EQUALS(a.y(), -100);

	a = {100, 100};
	a.rotate(modm::Angle::toRadian(20));

	TEST_ASSERT_EQUALS(a.x(), 60);
	TEST_ASSERT_EQUALS(a.y(), 128);

	a = {20, 10};
	a.rotate(-M_PI / 2);

	TEST_ASSERT_EQUALS(a.x(), 10);
	TEST_ASSERT_EQUALS(a.y(), -20);
}

void
Vector2Test::testRotateFloat()
{
	modm::Vector<float, 2> a(100, 100);

	a.rotate(modm::Angle::toRadian(20));

	TEST_ASSERT_EQUALS_FLOAT(a.x(), 59.767247746f);
#ifndef MODM_OS_WIN32 // FIXME: Windows has some unknown accuracy issue here
	TEST_ASSERT_EQUALS_FLOAT(a.y(), 128.1712764112f);
#endif
}

void
Vector2Test::testTranslate()
{
	modm::Vector2i a(10, 10);
	modm::Vector2i b(20, -20);

	// a.translate(b);
	a += b;

	TEST_ASSERT_EQUALS(a.x(), 30);
	TEST_ASSERT_EQUALS(a.y(), -10);
}

void
Vector2Test::testConversion()
{
	modm::Vector<float, 2> a(12.763f, -13.3123f);

	TEST_ASSERT_EQUALS(a.x(), 12.763f);
	TEST_ASSERT_EQUALS(a.y(), -13.3123f);

	modm::Vector2i b(a);

	TEST_ASSERT_EQUALS(b.x(), 13);
	TEST_ASSERT_EQUALS(b.y(), -13);
}

void
Vector2Test::testMathDefs()
{
	modm::Vector2i a(7,0);
	modm::Vector2f b(INFINITY,NAN);

	TEST_ASSERT_FALSE(a.hasNan());
	TEST_ASSERT_FALSE(a.hasInf());

	TEST_ASSERT_TRUE(b.hasNan());
	TEST_ASSERT_TRUE(b.hasInf());
}

void
Vector2Test::testAngle()
{
	modm::Vector2i a;

	a = {100, 100};
	TEST_ASSERT_EQUALS_FLOAT(a.getAngle(), M_PI / 4);

	a = {-100, -100};
	TEST_ASSERT_EQUALS_FLOAT(a.getAngle(), - 3* M_PI / 4);

	a = {0, 100};
	TEST_ASSERT_EQUALS_FLOAT(a.getAngle(), M_PI / 2);
}

void
Vector2Test::testDotProduct()
{
	modm::Vector2i a(1000, 1000);
	modm::Vector2i b(1000, 0);
	modm::Vector2i c(1000, -1000);

	TEST_ASSERT_EQUALS(a.dot(b), 1000000);
	TEST_ASSERT_EQUALS(a.dot(c), 0);
	TEST_ASSERT_EQUALS(b.dot(c), 1000000);
}

void
Vector2Test::testCrossProduct()
{
	modm::Vector2i a(10, 20);
	modm::Vector2i b(10, 0);
	modm::Vector2i c(15, -30);

	TEST_ASSERT_EQUALS(a.cross(b), -200);
	TEST_ASSERT_EQUALS(a.cross(c), -600);
	TEST_ASSERT_EQUALS(b.cross(c), -300);
}

void
Vector2Test::testDistance()
{
	modm::Vector2i a(10, 10);
	modm::Vector2i b(20, 20);

	TEST_ASSERT_EQUALS(a.getDistanceTo(b), 14);

	modm::Vector2i c(40, 20);

	TEST_ASSERT_EQUALS(a.getDistanceTo(c), 32);
}

void
Vector2Test::testCCW()
{
	modm::Vector2i a(0, 0);
	modm::Vector2i b(30, 30);
	modm::Vector2i c(40, 40);
	modm::Vector2i d(0, 40);

	TEST_ASSERT_EQUALS(modm::ccw(a, b, d), 1);
	TEST_ASSERT_EQUALS(modm::ccw(b, d, a), 1);
	TEST_ASSERT_EQUALS(modm::ccw(b, a, d), -1);
	TEST_ASSERT_EQUALS(modm::ccw(a, d, b), -1);

	// three points in a strait row
	TEST_ASSERT_EQUALS(modm::ccw(a, b, c), 1);

	// last point between the two other
	TEST_ASSERT_EQUALS(modm::ccw(a, c, b), 0);

	// last point before the first
	TEST_ASSERT_EQUALS(modm::ccw(b, c, a), -1);
}

