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
#include "vector5_test.hpp"

void
Vector5Test::testConstructor()
{
	int16_t array[5] = {1, 2, 3, 4, 5};

	modm::Vector<int16_t, 5> a;
	TEST_ASSERT_EQUALS(a[0], 0);
	TEST_ASSERT_EQUALS(a[1], 0);
	TEST_ASSERT_EQUALS(a[2], 0);
	TEST_ASSERT_EQUALS(a[3], 0);
	TEST_ASSERT_EQUALS(a[4], 0);

	modm::Vector<int16_t, 5> b(array);
	TEST_ASSERT_EQUALS(b[0], 1);
	TEST_ASSERT_EQUALS(b[1], 2);
	TEST_ASSERT_EQUALS(b[2], 3);
	TEST_ASSERT_EQUALS(b[3], 4);
	TEST_ASSERT_EQUALS(b[4], 5);

	modm::Vector<int16_t, 5> c(7);
	TEST_ASSERT_EQUALS(c[0], 7);
	TEST_ASSERT_EQUALS(c[1], 7);
	TEST_ASSERT_EQUALS(c[2], 7);
	TEST_ASSERT_EQUALS(c[3], 7);
	TEST_ASSERT_EQUALS(c[4], 7);

	modm::Vector<int16_t, 5> d(3.6);
	TEST_ASSERT_EQUALS(d[0], 4);
	TEST_ASSERT_EQUALS(d[1], 4);
	TEST_ASSERT_EQUALS(d[2], 4);
	TEST_ASSERT_EQUALS(d[3], 4);
	TEST_ASSERT_EQUALS(d[4], 4);
}

// void
// Vector5Test::testAssign()
// {
// 	modm::Vector4i a(1,2,3,4);

// 	int16_t array[4] = {5,6,7,8};
// 	modm::Matrix<int16_t, 4, 1> m(array);

// 	modm::Vector4i b;

// 	b = a;
// 	TEST_ASSERT_EQUALS(b.x, 1);
// 	TEST_ASSERT_EQUALS(b.y, 2);
// 	TEST_ASSERT_EQUALS(b.z, 3);
// 	TEST_ASSERT_EQUALS(b.w, 4);

// 	b = m;
// 	TEST_ASSERT_EQUALS(b.x, 5);
// 	TEST_ASSERT_EQUALS(b.y, 6);
// 	TEST_ASSERT_EQUALS(b.z, 7);
// 	TEST_ASSERT_EQUALS(b.w, 8);
// }

// void
// Vector5Test::testCompare()
// {
// 	modm::Vector4i a(4,5,2,1);
// 	modm::Vector4i b(4,5,2,3);
// 	modm::Vector4i c(4,5,2,3);
// 	// ==
// 	TEST_ASSERT_TRUE(b == c);
// 	TEST_ASSERT_FALSE(a == c);
// 	// !=
// 	TEST_ASSERT_TRUE(a != c);
// 	TEST_ASSERT_FALSE(b != c);
// 	// <
// 	TEST_ASSERT_TRUE(a < c);
// 	TEST_ASSERT_FALSE(b < c);
// 	TEST_ASSERT_FALSE(c < a);
// 	// <=
// 	TEST_ASSERT_TRUE(a <= c);
// 	TEST_ASSERT_TRUE(b <= c);
// 	TEST_ASSERT_FALSE(c <= a);
// 	// >
// 	TEST_ASSERT_TRUE(c > a);
// 	TEST_ASSERT_FALSE(b > c);
// 	TEST_ASSERT_FALSE(a > c);
// 	// >=
// 	TEST_ASSERT_TRUE(c >= a);
// 	TEST_ASSERT_TRUE(b >= c);
// 	TEST_ASSERT_FALSE(a >= c);
// }

// void
// Vector5Test::testRawDataAccess()
// {
// 	modm::Vector4i a(0,1,2,3);
// 	int16_t *pointer = a.ptr();

// 	TEST_ASSERT_EQUALS(a[0], 0);
// 	TEST_ASSERT_EQUALS(a[1], 1);
// 	TEST_ASSERT_EQUALS(a[2], 2);
// 	TEST_ASSERT_EQUALS(a[3], 3);
// 	TEST_ASSERT_EQUALS(pointer[0], 0);
// 	TEST_ASSERT_EQUALS(pointer[1], 1);
// 	TEST_ASSERT_EQUALS(pointer[2], 2);
// 	TEST_ASSERT_EQUALS(pointer[3], 3);
// }

void
Vector5Test::testOperators()
{
	modm::Vector<int16_t, 5> a({1, 2, 3, 4, 5});
	modm::Vector<int16_t, 5> b({4, 5, 6, 7, 8});

	TEST_ASSERT_EQUALS((a + b)[0], 1+4);
	TEST_ASSERT_EQUALS((a + b)[1], 2+5);
	TEST_ASSERT_EQUALS((a + b)[2], 3+6);
	TEST_ASSERT_EQUALS((a + b)[3], 4+7);
	TEST_ASSERT_EQUALS((a + b)[4], 5+8);

	TEST_ASSERT_EQUALS((a - b)[0], 1-4);
	TEST_ASSERT_EQUALS((a - b)[1], 2-5);
	TEST_ASSERT_EQUALS((a - b)[2], 3-6);
	TEST_ASSERT_EQUALS((a - b)[3], 4-7);
	TEST_ASSERT_EQUALS((a - b)[4], 5-8);

	TEST_ASSERT_EQUALS((a * b), 1*4+2*5+3*6+4*7+5*8);

	TEST_ASSERT_EQUALS((a * 3)[0], 1*3);
	TEST_ASSERT_EQUALS((a * 3)[1], 2*3);
	TEST_ASSERT_EQUALS((a * 3)[2], 3*3);
	TEST_ASSERT_EQUALS((a * 3)[3], 4*3);
	TEST_ASSERT_EQUALS((a * 3)[4], 5*3);

	// TODO
	// TEST_ASSERT_EQUALS((3 * a)[0], 3*1);
	// TEST_ASSERT_EQUALS((3 * a)[1], 3*2);
	// TEST_ASSERT_EQUALS((3 * a)[2], 3*3);
	// TEST_ASSERT_EQUALS((3 * a)[3], 3*4);
	// TEST_ASSERT_EQUALS((3 * a)[4], 3*5);

	TEST_ASSERT_EQUALS((b / 2)[0], 4/2);
	TEST_ASSERT_EQUALS((b / 2)[1], 5/2);
	TEST_ASSERT_EQUALS((b / 2)[2], 6/2);
	TEST_ASSERT_EQUALS((b / 2)[3], 7/2);
	TEST_ASSERT_EQUALS((b / 2)[4], 8/2);

	-b;
	TEST_ASSERT_EQUALS(b[0], 4);
	TEST_ASSERT_EQUALS(b[1], 5);
	TEST_ASSERT_EQUALS(b[2], 6);
	TEST_ASSERT_EQUALS(b[3], 7);
	TEST_ASSERT_EQUALS(b[4], 8);
	b = -b;
	TEST_ASSERT_EQUALS(b[0], -4);
	TEST_ASSERT_EQUALS(b[1], -5);
	TEST_ASSERT_EQUALS(b[2], -6);
	TEST_ASSERT_EQUALS(b[3], -7);
	TEST_ASSERT_EQUALS(b[4], -8);
	a += b;
	TEST_ASSERT_EQUALS(a[0], 1-4);
	TEST_ASSERT_EQUALS(a[1], 2-5);
	TEST_ASSERT_EQUALS(a[2], 3-6);
	TEST_ASSERT_EQUALS(a[3], 4-7);
	TEST_ASSERT_EQUALS(a[4], 5-8);
	a -= b;
	TEST_ASSERT_EQUALS(a[0], 1);
	TEST_ASSERT_EQUALS(a[1], 2);
	TEST_ASSERT_EQUALS(a[2], 3);
	TEST_ASSERT_EQUALS(a[3], 4);
	TEST_ASSERT_EQUALS(a[4], 5);
	a *= 2;
	TEST_ASSERT_EQUALS(a[0], 1*2);
	TEST_ASSERT_EQUALS(a[1], 2*2);
	TEST_ASSERT_EQUALS(a[2], 3*2);
	TEST_ASSERT_EQUALS(a[3], 4*2);
	TEST_ASSERT_EQUALS(a[4], 5*2);
	b /= 2;
	TEST_ASSERT_EQUALS(b[0], -4/2);
	TEST_ASSERT_EQUALS(b[1], -5/2);
	TEST_ASSERT_EQUALS(b[2], -6/2);
	TEST_ASSERT_EQUALS(b[3], -7/2);
	TEST_ASSERT_EQUALS(b[4], -8/2);
}

void
Vector5Test::testLength()
{
	modm::Vector<float, 5> a({1.f,2.f,3.f,4.f,5.f});

	TEST_ASSERT_EQUALS_FLOAT(a.getLengthSquared(), 1.f*1.f+2.f*2.f+3.f*3.f+4.f*4.f+5.f*5.f);
	// TEST_ASSERT_EQUALS_FLOAT(a.getLength(), 5.477225575);

	// TEST_ASSERT_EQUALS_FLOAT(a.scaled(2.f).x, 0.3651483717);
	// TEST_ASSERT_EQUALS_FLOAT(a.scaled(2.f).y, 0.7302967433);
	// TEST_ASSERT_EQUALS_FLOAT(a.scaled(2.f).z, 1.095445115);
	// TEST_ASSERT_EQUALS_FLOAT(a.scaled(2.f).w, 1.4605934867);
	// a.scale(2.f);
	// TEST_ASSERT_EQUALS_FLOAT(a.x, 0.3651483717);
	// TEST_ASSERT_EQUALS_FLOAT(a.y, 0.7302967433);
	// TEST_ASSERT_EQUALS_FLOAT(a.z, 1.095445115);
	// TEST_ASSERT_EQUALS_FLOAT(a.w, 1.4605934867);

	// TEST_ASSERT_EQUALS_FLOAT(a.normalized().x, 0.1825741858);
	// TEST_ASSERT_EQUALS_FLOAT(a.normalized().y, 0.3651483717);
	// TEST_ASSERT_EQUALS_FLOAT(a.normalized().z, 0.5477225575);
	// TEST_ASSERT_EQUALS_FLOAT(a.normalized().w, 0.7302967433);
	// a.normalize();
	// TEST_ASSERT_EQUALS_FLOAT(a.x, 0.1825741858);
	// TEST_ASSERT_EQUALS_FLOAT(a.y, 0.3651483717);
	// TEST_ASSERT_EQUALS_FLOAT(a.z, 0.5477225575);
	// TEST_ASSERT_EQUALS_FLOAT(a.w, 0.7302967433);
}
