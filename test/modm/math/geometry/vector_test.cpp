/*
 * Copyright (c) 2011, Fabian Greif
 * Copyright (c) 2011-2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/math/geometry/vector.hpp>
#include "vector_test.hpp"

void
VectorTest::testConstructor()
{
	int16_t array[2] = {1, 2};
/* 	modm::Matrix<int16_t, 2, 1> m(array); */

	modm::Vector<int16_t, 2> a;
	TEST_ASSERT_EQUALS(a[0], 0);
	TEST_ASSERT_EQUALS(a[1], 0);

	a[0] = 1;
	a[1] = 2;
	TEST_ASSERT_EQUALS(a[0], 1);
	TEST_ASSERT_EQUALS(a[1], 2);

	modm::Vector<int16_t, 2> c(array);
	TEST_ASSERT_EQUALS(c[0], 1);
	TEST_ASSERT_EQUALS(c[1], 2);

	modm::Vector<int16_t, 2> p(a);
	TEST_ASSERT_EQUALS(p[0], 1);
	TEST_ASSERT_EQUALS(p[1], 2);

/* 	modm::Vector<int16_t, 2> q(m);
	TEST_ASSERT_EQUALS(q[0], 1);
	TEST_ASSERT_EQUALS(q[1], 2); */
}

void
VectorTest::testAssign()
{;
	modm::Vector<int16_t, 4> a = {1, 2, 3, 4};

	/* int16_t array2[4] = {5, 6, 7, 8};
	modm::Matrix<int16_t, 4, 1> m(array2); */

	modm::Vector<int16_t, 4> b;

	b = a;
	TEST_ASSERT_EQUALS(b[0], 1);
	TEST_ASSERT_EQUALS(b[1], 2);
	TEST_ASSERT_EQUALS(b[2], 3);
	TEST_ASSERT_EQUALS(b[3], 4);

	/* b = m;
	TEST_ASSERT_EQUALS(b[0], 5);
	TEST_ASSERT_EQUALS(b[1], 6);
	TEST_ASSERT_EQUALS(b[2], 7);
	TEST_ASSERT_EQUALS(b[3], 8); */
}

void
VectorTest::testCompare()
{
	int16_t array1[4] = {4, 5, 2, 1};
	modm::Vector<int16_t, 4> a(array1);

	int16_t array2[4] = {4, 5, 2, 3};
	modm::Vector<int16_t, 4> b(array2);
	modm::Vector<int16_t, 4> c(array2);

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

// TODO I think this is no more required
void
VectorTest::testRawDataAccess()
{
	int16_t array[4] = {0, 1, 2, 3};
	modm::Vector<int16_t,4> a(array);
	int16_t *pointer = a.data();

	TEST_ASSERT_EQUALS(a[0], 0);
	TEST_ASSERT_EQUALS(a[1], 1);
	TEST_ASSERT_EQUALS(a[2], 2);
	TEST_ASSERT_EQUALS(a[3], 3);
	TEST_ASSERT_EQUALS(pointer[0], 0);
	TEST_ASSERT_EQUALS(pointer[1], 1);
	TEST_ASSERT_EQUALS(pointer[2], 2);
	TEST_ASSERT_EQUALS(pointer[3], 3);
}

void
VectorTest::testOperators()
{
	modm::Vector<int16_t,4> a({1, 2, 3, 4});

	modm::Vector<int16_t,4> b({4, 5, 6, 7});

	TEST_ASSERT_EQUALS((a + b)[0], 1+4);
	TEST_ASSERT_EQUALS((a + b)[1], 2+5);
	TEST_ASSERT_EQUALS((a + b)[2], 3+6);
	TEST_ASSERT_EQUALS((a + b)[3], 4+7);

	TEST_ASSERT_EQUALS((a - b)[0], 1-4);
	TEST_ASSERT_EQUALS((a - b)[1], 2-5);
	TEST_ASSERT_EQUALS((a - b)[2], 3-6);
	TEST_ASSERT_EQUALS((a - b)[3], 4-7);

	TEST_ASSERT_EQUALS((a.dot(b)), 1*4 + 2*5 + 3*6 + 4*7);

	TEST_ASSERT_EQUALS((a * 3)[0], 1*3);
	TEST_ASSERT_EQUALS((a * 3)[1], 2*3);
	TEST_ASSERT_EQUALS((a * 3)[2], 3*3);
	TEST_ASSERT_EQUALS((a * 3)[3], 4*3);

	//	TEST_ASSERT_EQUALS((3 * a)[0], 3*1);
	//	TEST_ASSERT_EQUALS((3 * a)[1], 3*2);
	//	TEST_ASSERT_EQUALS((3 * a)[2], 3*3);
	//	TEST_ASSERT_EQUALS((3 * a)[3], 3*4);

	TEST_ASSERT_EQUALS((b / 2)[0], 4/2);
	TEST_ASSERT_EQUALS((b / 2)[1], 5/2);
	TEST_ASSERT_EQUALS((b / 2)[2], 6/2);
	TEST_ASSERT_EQUALS((b / 2)[3], 7/2);

	-b;
	TEST_ASSERT_EQUALS(b[0], 4);
	TEST_ASSERT_EQUALS(b[1], 5);
	TEST_ASSERT_EQUALS(b[2], 6);
	TEST_ASSERT_EQUALS(b[3], 7);
	b = -b;
	TEST_ASSERT_EQUALS(b[0], -4);
	TEST_ASSERT_EQUALS(b[1], -5);
	TEST_ASSERT_EQUALS(b[2], -6);
	TEST_ASSERT_EQUALS(b[3], -7);
	a += b;
	TEST_ASSERT_EQUALS(a[0], 1-4);
	TEST_ASSERT_EQUALS(a[1], 2-5);
	TEST_ASSERT_EQUALS(a[2], 3-6);
	TEST_ASSERT_EQUALS(a[3], 4-7);
	a -= b;
	TEST_ASSERT_EQUALS(a[0], 1);
	TEST_ASSERT_EQUALS(a[1], 2);
	TEST_ASSERT_EQUALS(a[2], 3);
	TEST_ASSERT_EQUALS(a[3], 4);
	a *= 2;
	TEST_ASSERT_EQUALS(a[0], 1*2);
	TEST_ASSERT_EQUALS(a[1], 2*2);
	TEST_ASSERT_EQUALS(a[2], 3*2);
	TEST_ASSERT_EQUALS(a[3], 4*2);
	b /= 2;
	TEST_ASSERT_EQUALS(b[0], -4/2);
	TEST_ASSERT_EQUALS(b[1], -5/2);
	TEST_ASSERT_EQUALS(b[2], -6/2);
	TEST_ASSERT_EQUALS(b[3], -7/2);
}

void
VectorTest::testLength()
{
	float array[4] = {1.f, 2.f, 3.f, 4.f};
	modm::Vector<float, 4> a(array);

	TEST_ASSERT_EQUALS_FLOAT(a.getLengthSquared(), 1.f*1.f + 2.f*2.f + 3.f*3.f + 4.f*4.f);
	TEST_ASSERT_EQUALS_FLOAT(a.getLength(), 5.477225575);
}

void VectorTest::testConvert()
{
	modm::Vector<float, 2> f(1.3, 2.7);
	modm::Vector<double, 2> d(1.3, 2.7);

	// convert() is deprecated but we still test for b.c.
	modm::Vector<double, 2> d2 = f.convert<double>();
	TEST_ASSERT_EQUALS_FLOAT(d2.x(), 1.3);
	TEST_ASSERT_EQUALS_FLOAT(d2.y(), 2.7);
	// convert() is deprecated but we still test for b.c.
	modm::Vector<float, 2> f2 = d.convert<float>();
	TEST_ASSERT_EQUALS_FLOAT(f2.x(), 1.3);
	TEST_ASSERT_EQUALS_FLOAT(f2.y(), 2.7);

	// Conversion constructor replaces convert()
	modm::Vector<double, 2> d3(f);
	TEST_ASSERT_EQUALS_FLOAT(d3.x(), 1.3);
	TEST_ASSERT_EQUALS_FLOAT(d3.y(), 2.7);
	// Conversion constructor replaces convert()
	modm::Vector<float, 2> f3(d);
	TEST_ASSERT_EQUALS_FLOAT(f3.x(), 1.3);
	TEST_ASSERT_EQUALS_FLOAT(f3.y(), 2.7);

	modm::Vector<uint8_t, 2> u8(1.3, 2.7);
	TEST_ASSERT_EQUALS(u8.x(), 1);
	TEST_ASSERT_EQUALS(u8.y(), 3);

	modm::Vector<int8_t, 2> i8(1.3, 2.7);
	TEST_ASSERT_EQUALS(i8.x(), 1);
	TEST_ASSERT_EQUALS(i8.y(), 3);

	modm::Vector<int8_t, 2> i82(f);
	TEST_ASSERT_EQUALS(i82.x(), 1);
	TEST_ASSERT_EQUALS(i82.y(), 3);
}
