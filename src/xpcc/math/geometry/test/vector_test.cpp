// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
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

#include <xpcc/math/geometry/vector.hpp>
#include "vector_test.hpp"

void
VectorTest::testConstructor()
{
	int16_t array[2] = {1, 2};
	xpcc::Matrix<int16_t, 1, 2> m(array);
	
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
	xpcc::Matrix<int16_t, 1, 4> m(array2);
	
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
