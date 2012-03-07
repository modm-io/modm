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
 * $Id: point_set_2d_test.cpp 607 2011-09-13 19:51:03Z dergraaf $
 */
// ----------------------------------------------------------------------------

#include <xpcc/math/geometry/point_set_2d.hpp>

#include "point_set_2d_test.hpp"

void
PointSet2DTest::testConstructor()
{
	xpcc::PointSet2D<int16_t> set(5);
	
	TEST_ASSERT_EQUALS(set.getNumberOfPoints(), 0U);
}

void
PointSet2DTest::testAppendAndAccess()
{
	xpcc::PointSet2D<int16_t> set(5);
	
	set.append(xpcc::Vector2i(10, 20));
	
	TEST_ASSERT_EQUALS(set.getNumberOfPoints(), 1U);
	TEST_ASSERT_EQUALS(set[0], xpcc::Vector2i(10, 20));
	
	set.append(xpcc::Vector2i(30, 40));
	
	TEST_ASSERT_EQUALS(set.getNumberOfPoints(), 2U);
	TEST_ASSERT_EQUALS(set[0], xpcc::Vector2i(10, 20));
	TEST_ASSERT_EQUALS(set[1], xpcc::Vector2i(30, 40));
	
	set[0] = xpcc::Vector2i(50, 60);
	
	TEST_ASSERT_EQUALS(set[0], xpcc::Vector2i(50, 60));
}

void
PointSet2DTest::testIterator()
{
	xpcc::PointSet2D<int16_t> set(3);
	set.append(xpcc::Vector2i(10, 20));
	set.append(xpcc::Vector2i(20, 30));
	set.append(xpcc::Vector2i(30, 40));
	
	xpcc::PointSet2D<int16_t>::const_iterator it;
	
	int count;
	for (it = set.begin(), count = 0; it != set.end(); ++it, ++count) {
		TEST_ASSERT_EQUALS(set[count], (*it));
	}
	
	TEST_ASSERT_EQUALS(count, 3);
}
