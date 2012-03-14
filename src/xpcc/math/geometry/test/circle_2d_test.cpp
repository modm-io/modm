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
 */
// ----------------------------------------------------------------------------

#include <xpcc/math/geometry/circle_2d.hpp>

#include "circle_2d_test.hpp"

void
Circle2DTest::testDefaultConstructor()
{
	xpcc::Circle2D<int16_t> circle;
	
	TEST_ASSERT_EQUALS(circle.getRadius(), 0);
	TEST_ASSERT_EQUALS(circle.getCenter(), xpcc::Vector2i(0, 0));
}

void
Circle2DTest::testConstructor()
{
	xpcc::Circle2D<int16_t> circle(
			xpcc::Vector2i(-10, -20),
			45);
	
	TEST_ASSERT_EQUALS(circle.getRadius(), 45);
	TEST_ASSERT_EQUALS(circle.getCenter(), xpcc::Vector2i(-10, -20));
	
	circle.setCenter(xpcc::Vector2i(20, 30));
	circle.setRadius(70);
	
	TEST_ASSERT_EQUALS(circle.getRadius(), 70);
	TEST_ASSERT_EQUALS(circle.getCenter(), xpcc::Vector2i(20, 30));
}

void
Circle2DTest::testIntersectionCircle()
{
	xpcc::Circle2D<int16_t> circle1(
			xpcc::Vector2i(),
			10);
	
	xpcc::Circle2D<int16_t> circle2(
			xpcc::Vector2i(30, 0),
			20);
	
	xpcc::PointSet2D<int16_t> points;
	
	// circle touch each other with one point
	TEST_ASSERT_TRUE(circle1.getIntersections(circle2, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 1U);
	
	TEST_ASSERT_EQUALS(points[0].getX(), 10);
	TEST_ASSERT_EQUALS(points[0].getY(), 0);
	
	points.removeAll();
	
	// no intersection
	circle2.setRadius(10);
	TEST_ASSERT_FALSE(circle1.getIntersections(circle2, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 0U);
	
	// 2 intersection points (15, -25.98) and (15, 25.98)
	circle1.setRadius(30);
	circle2.setRadius(30);
	TEST_ASSERT_TRUE(circle1.getIntersections(circle2, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 2U);
	
	TEST_ASSERT_EQUALS(points[0].getX(), 15);
	TEST_ASSERT_EQUALS(points[0].getY(), -26);
	
	TEST_ASSERT_EQUALS(points[1].getX(), 15);
	TEST_ASSERT_EQUALS(points[1].getY(), 26);
	
	points.removeAll();
	
	// circle 1 is contained inside circle 2
	circle1.setRadius(10);
	circle2.setCenter(xpcc::Vector2i(0, 0));
	TEST_ASSERT_FALSE(circle1.getIntersections(circle2, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 0U);
}
