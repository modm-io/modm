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

#include "point2_test.hpp"

void
Point2Test::testConstructor()
{
	xpcc::Point2i a;
	TEST_ASSERT_EQUALS(a.x, 0);
	TEST_ASSERT_EQUALS(a.y, 0);
	
	a.x = 100;
	a.y = 9;
	TEST_ASSERT_EQUALS(a.x, 100);
	TEST_ASSERT_EQUALS(a.y, 9);
	
	xpcc::Point2i b(20);
	TEST_ASSERT_EQUALS(b.x, 20);
	TEST_ASSERT_EQUALS(b.y, 20);
	
	xpcc::Point2i c(20,30);
	TEST_ASSERT_EQUALS(c.x, 20);
	TEST_ASSERT_EQUALS(c.y, 30);
	
	int16_t array[2] = {-4,5};
	xpcc::Matrix<int16_t,1,2> m(array);
	xpcc::Point2i d(m);
	TEST_ASSERT_EQUALS(d.x, -4);
	TEST_ASSERT_EQUALS(d.y, 5);
	
	xpcc::Point2i e(a);
	TEST_ASSERT_EQUALS(e.x, 100);
	TEST_ASSERT_EQUALS(e.y, 9);
	
	xpcc::Point1i f(4);
	
	xpcc::Point2i g(1,f);
	TEST_ASSERT_EQUALS(g.x, 1);
	TEST_ASSERT_EQUALS(g.y, 4);
	xpcc::Point2i h(f,5);
	TEST_ASSERT_EQUALS(h.x, 4);
	TEST_ASSERT_EQUALS(h.y, 5);
	xpcc::Point2i i(f,f);
	TEST_ASSERT_EQUALS(i.x, 4);
	TEST_ASSERT_EQUALS(i.y, 4);
}

void
Point2Test::testAssign()
{
	xpcc::Point2i a(42,-4);
	int16_t array[2] = {-26,9};
	xpcc::Matrix<int16_t, 1, 2> m(array);
	
	xpcc::Point2i b;
	
	b = a;
	TEST_ASSERT_EQUALS(b.x, 42);
	TEST_ASSERT_EQUALS(b.y, -4);
	
	b = m;
	TEST_ASSERT_EQUALS(b.x, -26);
	TEST_ASSERT_EQUALS(b.y, 9);
}

void
Point2Test::testCompare()
{
	xpcc::Point2i a(4,3);
	xpcc::Point2i b(4,5);
	xpcc::Point2i c(4,5);
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
Point2Test::testRawDataAccess()
{
	xpcc::Point2i a(2,5);
	int16_t *pointer = a.ptr();
	
	TEST_ASSERT_EQUALS(a[0], 2);
	TEST_ASSERT_EQUALS(a[1], 5);
	TEST_ASSERT_EQUALS(pointer[0], 2);
	TEST_ASSERT_EQUALS(pointer[1], 5);
}

void
Point2Test::testOperators()
{
	xpcc::Point2i a(7,5);
	xpcc::Point2i b(-18,7);
	
	TEST_ASSERT_EQUALS((a + b).x, 7-18);
	TEST_ASSERT_EQUALS((a + b).y, 5+7);
	TEST_ASSERT_EQUALS((a - b).x, 7-(-18));
	TEST_ASSERT_EQUALS((a - b).y, 5-7);
	TEST_ASSERT_EQUALS((a * b), 7*(-18)+5*7);
	TEST_ASSERT_EQUALS((a ^ b), 7*7-(-18)*5);
	TEST_ASSERT_EQUALS((a * 3).x, 7*3);
	TEST_ASSERT_EQUALS((a * 3).y, 5*3);
	TEST_ASSERT_EQUALS((3 * a).x, 3*7);
	TEST_ASSERT_EQUALS((3 * a).y, 3*5);
	TEST_ASSERT_EQUALS((b / 2).x, -18/2);
	TEST_ASSERT_EQUALS((b / 2).y, 7/2);
	
	-b;
	TEST_ASSERT_EQUALS(b.x, 18);
	TEST_ASSERT_EQUALS(b.y, -7);
	a += b;
	TEST_ASSERT_EQUALS(a.x, 7+18);
	TEST_ASSERT_EQUALS(a.y, 5-7);
	a -= b;
	TEST_ASSERT_EQUALS(a.x, 7+18-18);
	TEST_ASSERT_EQUALS(a.y, 5-7-(-7));
	a *= 2;
	TEST_ASSERT_EQUALS(a.x, 7*2);
	TEST_ASSERT_EQUALS(a.y, 5*2);
	b /= 2;
	TEST_ASSERT_EQUALS(b.x, 18/2);
	TEST_ASSERT_EQUALS(b.y, -7/2);
	~b;
	TEST_ASSERT_EQUALS(b.x, 7/2);
	TEST_ASSERT_EQUALS(b.y, 18/2);
	
	// test division of floats
	xpcc::Point2f c(-18.7f,5.5f);
	TEST_ASSERT_EQUALS_FLOAT((c / 2.4f).x, -7.7916666667);
	TEST_ASSERT_EQUALS_FLOAT((c / 2.4f).y, 2.2916666667);
	c /= 7.5f;
	TEST_ASSERT_EQUALS_FLOAT(c.x, -2.4933333333);
	TEST_ASSERT_EQUALS_FLOAT(c.y, 0.7333333333);
}

void
Point2Test::testLength()
{
	xpcc::Point2f a(3.f,4.f);
	
	TEST_ASSERT_EQUALS_FLOAT(a.length2(), 3.f*3.f+4.f*4.f);
	TEST_ASSERT_EQUALS_FLOAT(a.length(), 5.f);
	
	TEST_ASSERT_EQUALS_FLOAT(a.scaled(2.5f).x, 1.5f);
	TEST_ASSERT_EQUALS_FLOAT(a.scaled(2.5f).y, 2.f);
	a.scale(2.f);
	TEST_ASSERT_EQUALS_FLOAT(a.x, 1.2f);
	TEST_ASSERT_EQUALS_FLOAT(a.y, 1.6f);
	
	TEST_ASSERT_EQUALS_FLOAT(a.normalized().x, 0.6f);
	TEST_ASSERT_EQUALS_FLOAT(a.normalized().y, 0.8f);
	a.normalize();
	TEST_ASSERT_EQUALS_FLOAT(a.x, 0.6f);
	TEST_ASSERT_EQUALS_FLOAT(a.y, 0.8f);
	
	xpcc::Point2f b(a.perpendicular());
	TEST_ASSERT_EQUALS_FLOAT(b.x, -0.8f);
	TEST_ASSERT_EQUALS_FLOAT(b.y, 0.6f);
}

void
Point2Test::testMathDefs()
{
	xpcc::Point2i a(7,0);
	xpcc::Point2f b(INFINITY,NAN);
	
	TEST_ASSERT_FALSE(a.hasNan());
	TEST_ASSERT_FALSE(a.hasInf());
	
	TEST_ASSERT_TRUE(b.hasNan());
	TEST_ASSERT_TRUE(b.hasInf());
}
