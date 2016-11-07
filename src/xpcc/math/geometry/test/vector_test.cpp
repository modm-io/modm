/*
 * Copyright (c) 2011-2012, Niklas Hauser
 * Copyright (c) 2011, Fabian Greif
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <xpcc/math/geometry/vector.hpp>
#include "vector_test.hpp"

void
VectorTest::testConstructor()
{
	int16_t array[2] = {1, 2};
	xpcc::Matrix<int16_t, 2, 1> m(array);
	
	xpcc::Vector<int16_t, 2> a;
	TEST_ASSERT_EQUALS(a[0], 0);
	TEST_ASSERT_EQUALS(a[1], 0);
	
	a[0] = 1;
	a[1] = 2;
	TEST_ASSERT_EQUALS(a[0], 1);
	TEST_ASSERT_EQUALS(a[1], 2);
	
	xpcc::Vector<int16_t, 2> c(array);
	TEST_ASSERT_EQUALS(c[0], 1);
	TEST_ASSERT_EQUALS(c[1], 2);
	
	xpcc::Vector<int16_t, 2> p(a);
	TEST_ASSERT_EQUALS(p[0], 1);
	TEST_ASSERT_EQUALS(p[1], 2);
	
	xpcc::Vector<int16_t, 2> q(m);
	TEST_ASSERT_EQUALS(q[0], 1);
	TEST_ASSERT_EQUALS(q[1], 2);
}

void
VectorTest::testAssign()
{
	int16_t array1[4] = {1, 2, 3, 4};
	xpcc::Vector<int16_t, 4> a(array1);
	
	int16_t array2[4] = {5, 6, 7, 8};
	xpcc::Matrix<int16_t, 4, 1> m(array2);
	
	xpcc::Vector<int16_t, 4> b;
	
	b = a;
	TEST_ASSERT_EQUALS(b[0], 1);
	TEST_ASSERT_EQUALS(b[1], 2);
	TEST_ASSERT_EQUALS(b[2], 3);
	TEST_ASSERT_EQUALS(b[3], 4);
	
	b = m;
	TEST_ASSERT_EQUALS(b[0], 5);
	TEST_ASSERT_EQUALS(b[1], 6);
	TEST_ASSERT_EQUALS(b[2], 7);
	TEST_ASSERT_EQUALS(b[3], 8);
}

void
VectorTest::testCompare()
{
	int16_t array1[4] = {4, 5, 2, 1};
	xpcc::Vector<int16_t, 4> a(array1);
	
	int16_t array2[4] = {4, 5, 2, 3};
	xpcc::Vector<int16_t, 4> b(array2);
	xpcc::Vector<int16_t, 4> c(array2);
	
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
VectorTest::testRawDataAccess()
{
	int16_t array[4] = {0, 1, 2, 3};
	xpcc::Vector<int16_t,4> a(array);
	int16_t *pointer = a.ptr();
	
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
	int16_t array1[4] = {1, 2, 3, 4};
	xpcc::Vector<int16_t,4> a(array1);
	
	int16_t array2[4] = {4, 5, 6, 7};
	xpcc::Vector<int16_t,4> b(array2);
	
	TEST_ASSERT_EQUALS((a + b)[0], 1+4);
	TEST_ASSERT_EQUALS((a + b)[1], 2+5);
	TEST_ASSERT_EQUALS((a + b)[2], 3+6);
	TEST_ASSERT_EQUALS((a + b)[3], 4+7);
	
	TEST_ASSERT_EQUALS((a - b)[0], 1-4);
	TEST_ASSERT_EQUALS((a - b)[1], 2-5);
	TEST_ASSERT_EQUALS((a - b)[2], 3-6);
	TEST_ASSERT_EQUALS((a - b)[3], 4-7);
	
	TEST_ASSERT_EQUALS((a * b), 1*4 + 2*5 + 3*6 + 4*7);
	
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
	xpcc::Vector<float, 4> a(array);
	
	TEST_ASSERT_EQUALS_FLOAT(a.getLengthSquared(), 1.f*1.f + 2.f*2.f + 3.f*3.f + 4.f*4.f);
	TEST_ASSERT_EQUALS_FLOAT(a.getLength(), 5.477225575);
}
