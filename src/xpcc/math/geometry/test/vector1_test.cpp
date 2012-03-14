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
 */
// ----------------------------------------------------------------------------

#include <xpcc/math/geometry/vector.hpp>
#include "vector1_test.hpp"

void
Vector1Test::testConstructor()
{
	xpcc::Vector1i a;
	TEST_ASSERT_EQUALS(a.x, 0);
	
	xpcc::Vector1i b(20);
	TEST_ASSERT_EQUALS(b.x, 20);
	
	a.x = 100;
	TEST_ASSERT_EQUALS(a.x, 100);
	
	xpcc::Vector1i c(a);
	TEST_ASSERT_EQUALS(c.x, 100);
	
	int16_t array[1] = {-4};
	xpcc::Matrix<int16_t, 1, 1> m(array);
	
	xpcc::Vector1i d(m);
	TEST_ASSERT_EQUALS(d.x, -4);
}

void
Vector1Test::testAssign()
{
	xpcc::Vector1i a(42);
	int16_t array[1] = {-42};
	xpcc::Matrix<int16_t, 1, 1> m(array);
	
	xpcc::Vector1i b;
	
	b = a;
	TEST_ASSERT_EQUALS(b.x, 42);
	
	b = m;
	TEST_ASSERT_EQUALS(b.x, -42);
}

void
Vector1Test::testCompare()
{
	xpcc::Vector1i a(2);
	xpcc::Vector1i b(4);
	xpcc::Vector1i c(4);
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
Vector1Test::testRawDataAccess()
{
	xpcc::Vector1i a(2);
	int16_t *pointer = a.ptr();
	
	TEST_ASSERT_EQUALS(a[0], 2);
	TEST_ASSERT_EQUALS(pointer[0], 2);
}

void
Vector1Test::testOperators()
{
	xpcc::Vector1i a(7);
	xpcc::Vector1i b(-18);
	
	TEST_ASSERT_EQUALS((a + b).x, 7-18);
	TEST_ASSERT_EQUALS((a - b).x, 7-(-18));
	TEST_ASSERT_EQUALS((a * b), -7*18);
	TEST_ASSERT_EQUALS((a * 3).x, 7*3);
	TEST_ASSERT_EQUALS((3 * a).x, 3*7);
	TEST_ASSERT_EQUALS((b / 2).x, -18/2);
	
	-b;
	TEST_ASSERT_EQUALS(b.x, -18);
	b = -b;
	TEST_ASSERT_EQUALS(b.x, 18);
	a += b;
	TEST_ASSERT_EQUALS(a.x, 7+18);
	a -= b;
	TEST_ASSERT_EQUALS(a.x, 7+18-18);
	a *= 2;
	TEST_ASSERT_EQUALS(a.x, 7*2);
	b /= 2;
	TEST_ASSERT_EQUALS(b.x, 18/2);
	
	// test division of floats
	xpcc::Vector1f c(-18.7f);
	TEST_ASSERT_EQUALS_FLOAT((c / 2.4f).x, -7.7916666667);
	c /= 7.5f;
	TEST_ASSERT_EQUALS_FLOAT(c.x, -2.4933333333);
}

void
Vector1Test::testLength()
{
	xpcc::Vector1i a(7);
	
	TEST_ASSERT_EQUALS(a.getLength(), 7);
	TEST_ASSERT_EQUALS(a.getLengthSquared(), 7*7);
}

void
Vector1Test::testMathDefs()
{
	xpcc::Vector1i a(7);
	xpcc::Vector1f b(NAN);
	xpcc::Vector1f c(INFINITY);
	
	TEST_ASSERT_FALSE(a.hasNan());
	TEST_ASSERT_FALSE(a.hasInf());
	
	TEST_ASSERT_TRUE(b.hasNan());
	TEST_ASSERT_TRUE(c.hasInf());
}
