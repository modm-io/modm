// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2013, Roboterclub Aachen e.V.
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

#include <xpcc/math/geometry/ray_2d.hpp>

#include "ray_2d_test.hpp"

void
Ray2DTest::testDefaultConstructor()
{
	xpcc::Ray2D<int16_t> ray;
	
	TEST_ASSERT_EQUALS(ray.getStartPoint(), xpcc::Vector2i(0, 0));
	TEST_ASSERT_EQUALS(ray.getDirectionVector(), xpcc::Vector2i(0, 0));
}

void
Ray2DTest::testConstructor()
{
	xpcc::Ray2D<int16_t> ray(
			xpcc::Vector2i(-20, -60),
			xpcc::Vector2i(90, 80));
	
	TEST_ASSERT_EQUALS(ray.getStartPoint(), xpcc::Vector2i(-20, -60));
	TEST_ASSERT_EQUALS(ray.getDirectionVector(), xpcc::Vector2i(90, 80));
}

void
Ray2DTest::testCcwDetection()
{
	xpcc::Ray2D<int16_t> ray(
				xpcc::Vector2i(0, 0),
				xpcc::Vector2i(10, 10));
	
	TEST_ASSERT_TRUE(ray.ccw(xpcc::Vector2i(0, 20)));
	TEST_ASSERT_FALSE(ray.ccw(xpcc::Vector2i(20, 0)));
}

void
Ray2DTest::testLineIntersection()
{
	xpcc::Ray2D<int16_t> ray(
			xpcc::Vector2i(1, 1),
			xpcc::Vector2i(10, 10));
	
	xpcc::LineSegment2D<int16_t> line(
			xpcc::Vector2i(-2, 0),
			xpcc::Vector2i(10, 0));
	
	xpcc::LineSegment2D<int16_t> line2(
			xpcc::Vector2i(5, 0),
			xpcc::Vector2i(0, 5));
	
	xpcc::LineSegment2D<int16_t> line3(
			xpcc::Vector2i(0, 6),
			xpcc::Vector2i(10, 6));
	
	xpcc::LineSegment2D<int16_t> line4(
			xpcc::Vector2i(2, 3),
			xpcc::Vector2i(6, 7));
	
	xpcc::LineSegment2D<int16_t> line5(
			xpcc::Vector2i(2, 1),
			xpcc::Vector2i(6, 5));
	
	xpcc::LineSegment2D<int16_t> line6(
			xpcc::Vector2i(8, 0),
			xpcc::Vector2i(8, 10));
	
	xpcc::LineSegment2D<int16_t> line7(
			xpcc::Vector2i(1, 0),
			xpcc::Vector2i(0, 1));
	
	TEST_ASSERT_FALSE(ray.intersects(line));
	TEST_ASSERT_TRUE(ray.intersects(line2));
	TEST_ASSERT_TRUE(ray.intersects(line3));
	TEST_ASSERT_FALSE(ray.intersects(line4));
	TEST_ASSERT_FALSE(ray.intersects(line5));
	TEST_ASSERT_TRUE(ray.intersects(line6));
	TEST_ASSERT_FALSE(ray.intersects(line7));
}
