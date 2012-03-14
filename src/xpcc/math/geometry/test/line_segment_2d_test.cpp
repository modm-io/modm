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

#include <xpcc/math/geometry/line_segment_2d.hpp>

#include "line_segment_2d_test.hpp"

void
LineSegment2DTest::testDefaultConstructor()
{
	xpcc::LineSegment2D<int16_t> line;
	
	TEST_ASSERT_EQUALS(line.getStartPoint(), xpcc::Vector2i(0, 0));
	TEST_ASSERT_EQUALS(line.getEndPoint(), xpcc::Vector2i(0, 0));
}

void
LineSegment2DTest::testConstructor()
{
	xpcc::LineSegment2D<int16_t> line(
			xpcc::Vector2i(-20, -60),
			xpcc::Vector2i(70, 30));
	
	TEST_ASSERT_EQUALS(line.getStartPoint(), xpcc::Vector2i(-20, -60));
	TEST_ASSERT_EQUALS(line.getEndPoint(), xpcc::Vector2i(70, 30));
}

void
LineSegment2DTest::testAccessor()
{
	xpcc::LineSegment2D<int16_t> line;
	
	line.setStartPoint(xpcc::Vector2i(30, 40));
	
	TEST_ASSERT_EQUALS(line.getStartPoint(), xpcc::Vector2i(30, 40));
	TEST_ASSERT_EQUALS(line.getEndPoint(), xpcc::Vector2i(0, 0));
	
	line.setEndPoint(xpcc::Vector2i(50, 60));
	
	TEST_ASSERT_EQUALS(line.getStartPoint(), xpcc::Vector2i(30, 40));
	TEST_ASSERT_EQUALS(line.getEndPoint(), xpcc::Vector2i(50, 60));
	
	line.set(xpcc::Vector2i(-60, -40),
			xpcc::Vector2i(70, 80));
	
	TEST_ASSERT_EQUALS(line.getStartPoint(), xpcc::Vector2i(-60, -40));
	TEST_ASSERT_EQUALS(line.getEndPoint(), xpcc::Vector2i(70, 80));
}

void
LineSegment2DTest::testTranslation()
{
	xpcc::LineSegment2D<int16_t> line(
			xpcc::Vector2i(12, 34),
			xpcc::Vector2i(56, 78));
	
	line.translate(xpcc::Vector2i(10, 20));
	
	TEST_ASSERT_EQUALS(line.getStartPoint(), xpcc::Vector2i(22, 54));
	TEST_ASSERT_EQUALS(line.getEndPoint(), xpcc::Vector2i(66, 98));
}

void
LineSegment2DTest::testComparision()
{
	xpcc::LineSegment2D<int16_t> a(
			xpcc::Vector2i(12, 34),
			xpcc::Vector2i(56, 78));
	
	xpcc::LineSegment2D<int16_t> b(
			xpcc::Vector2i(12, 34),
			xpcc::Vector2i(0, 0));
	
	xpcc::LineSegment2D<int16_t> c(
			xpcc::Vector2i(12, 34),
			xpcc::Vector2i(56, 78));
	
	TEST_ASSERT_FALSE(a == b);
	TEST_ASSERT_TRUE(a != b);
	
	TEST_ASSERT_TRUE(a == c);
	TEST_ASSERT_FALSE(a != c);
}

void
LineSegment2DTest::testLength()
{
	xpcc::LineSegment2D<int16_t> line(
			xpcc::Vector2i(0, 0),
			xpcc::Vector2i(70, 0));
	
	TEST_ASSERT_EQUALS(line.getLength(), 70);
	
	line.set(xpcc::Vector2i(-20, -60),
			xpcc::Vector2i(70, 30));
	
	TEST_ASSERT_EQUALS(line.getLength(), 127);
}

void
LineSegment2DTest::testLengthFloat()
{
	xpcc::LineSegment2D<float> line(
			xpcc::Vector<float, 2>(0, 0),
			xpcc::Vector<float, 2>(70, 10));
	
	TEST_ASSERT_EQUALS_FLOAT(line.getLength(), 70.71067812f);
}

void
LineSegment2DTest::testDistanceToPoint()
{
	xpcc::LineSegment2D<int16_t> line(
			xpcc::Vector2i(-20, -60),
			xpcc::Vector2i(50, 10));
	
	int16_t distance = line.getDistanceTo(xpcc::Vector2i(10, 30));
	
	// closest point is (40, 0) => sqrt(30^2 + 30^2) = 42.4264..
	TEST_ASSERT_EQUALS(distance, 42);
	
	TEST_ASSERT_EQUALS(line.getStartPoint(), xpcc::Vector2i(-20, -60));
	TEST_ASSERT_EQUALS(line.getEndPoint(), xpcc::Vector2i(50, 10));
	
	int16_t distance2 = line.getDistanceTo(xpcc::Vector2i(-50, -70));
	
	// closest point of the line is before the start point
	// => closest point is (-20, -60) => sqrt(30^2 + 10^2) = 31.6227766..
	TEST_ASSERT_EQUALS(distance2, 32);
	
	int16_t distance3 = line.getDistanceTo(xpcc::Vector2i(100, 20));
	
	// closest point of the line is after the end point
	// closest point is (50, 10) => sqrt(50^2 + 10^2) = 50.9901951..
	TEST_ASSERT_EQUALS(distance3, 51);
}

void
LineSegment2DTest::testIntersectsLineSegment()
{
	xpcc::LineSegment2D<int16_t> line1(
			xpcc::Vector2i(-20, -60),
			xpcc::Vector2i(50, 10));
	
	xpcc::LineSegment2D<int16_t> line2(
			xpcc::Vector2i(0, 0),
			xpcc::Vector2i(90, 30));
	
	xpcc::LineSegment2D<int16_t> line3(
			xpcc::Vector2i(10, -60),
			xpcc::Vector2i(40, -30));
	
	xpcc::LineSegment2D<int16_t> line4(
			xpcc::Vector2i(50, 10),
			xpcc::Vector2i(50, 50));
	
	xpcc::LineSegment2D<int16_t> line5(
			xpcc::Vector2i(10, 30),
			xpcc::Vector2i(70, -30));
	
	xpcc::LineSegment2D<int16_t> line6(
			xpcc::Vector2i(30, -40),
			xpcc::Vector2i(60, 50));
	
	TEST_ASSERT_FALSE(line1.intersects(line2));
	TEST_ASSERT_FALSE(line1.intersects(line3));
	TEST_ASSERT_TRUE(line1.intersects(line4));
	TEST_ASSERT_TRUE(line1.intersects(line5));
	TEST_ASSERT_TRUE(line1.intersects(line6));
	
	TEST_ASSERT_FALSE(line2.intersects(line3));
	TEST_ASSERT_TRUE(line2.intersects(line4));
	TEST_ASSERT_TRUE(line2.intersects(line5));
	TEST_ASSERT_TRUE(line2.intersects(line6));
	
	TEST_ASSERT_FALSE(line3.intersects(line4));
	TEST_ASSERT_FALSE(line3.intersects(line5));
	TEST_ASSERT_TRUE(line3.intersects(line6));
	
	TEST_ASSERT_FALSE(line4.intersects(line5));
	TEST_ASSERT_TRUE(line4.intersects(line6));
	
	TEST_ASSERT_TRUE(line5.intersects(line6));
}

void
LineSegment2DTest::testIntersectionPointsLineSegment()
{
	xpcc::LineSegment2D<int16_t> line1(
			xpcc::Vector2i(-20, -60),
			xpcc::Vector2i(50, 10));
	
	xpcc::LineSegment2D<int16_t> line2(
			xpcc::Vector2i(0, 0),
			xpcc::Vector2i(90, 30));
	
	xpcc::LineSegment2D<int16_t> line3(
			xpcc::Vector2i(10, -60),
			xpcc::Vector2i(40, -30));
	
	xpcc::LineSegment2D<int16_t> line4(
			xpcc::Vector2i(50, 10),
			xpcc::Vector2i(50, 50));
	
	xpcc::LineSegment2D<int16_t> line5(
			xpcc::Vector2i(10, 30),
			xpcc::Vector2i(70, -30));
	
	xpcc::PointSet2D<int16_t> points;
	
	// line1 - line2
	TEST_ASSERT_FALSE(line1.getIntersections(line2, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 0U);
	
	// line1 - line3
	TEST_ASSERT_FALSE(line1.getIntersections(line3, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 0U);
	
	// line1 - line4
	TEST_ASSERT_TRUE(line1.getIntersections(line4, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 1U);
	
	TEST_ASSERT_EQUALS(points[0].getX(), 50);
	TEST_ASSERT_EQUALS(points[0].getY(), 10);
	
	points.removeAll();
	
	// line1 - line5
	TEST_ASSERT_TRUE(line1.getIntersections(line5, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 1U);
	
	TEST_ASSERT_EQUALS(points[0].getX(), 40);
	TEST_ASSERT_EQUALS(points[0].getY(), 0);
	
	points.removeAll();
	
	// line2 - line3
	TEST_ASSERT_FALSE(line2.getIntersections(line3, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 0U);
	
	// line2 - line4
	TEST_ASSERT_TRUE(line2.getIntersections(line4, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 1U);
	
	TEST_ASSERT_EQUALS(points[0].getX(), 50);
	TEST_ASSERT_EQUALS(points[0].getY(), 17);
	
	points.removeAll();
	
	// line2 - line5
	TEST_ASSERT_TRUE(line2.getIntersections(line5, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 1U);
	
	TEST_ASSERT_EQUALS(points[0].getX(), 30);
	TEST_ASSERT_EQUALS(points[0].getY(), 10);
	
	points.removeAll();
	
	// line3 - line4
	TEST_ASSERT_FALSE(line3.getIntersections(line4, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 0U);
	
	// line3 - line5
	TEST_ASSERT_FALSE(line3.getIntersections(line5, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 0U);
	
	// line4 - line5
	TEST_ASSERT_FALSE(line4.getIntersections(line5, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 0U);
}

void
LineSegment2DTest::testIntersectionPointsCircle()
{
	xpcc::LineSegment2D<int16_t> line(
			xpcc::Vector2i(-10, -10),
			xpcc::Vector2i(10, 10));
	
	xpcc::Circle2D<int16_t> circle(
			xpcc::Vector2i(0, 0),
			20);
	
	xpcc::PointSet2D<int16_t> points;
	
	// line is contained inside the circle
	TEST_ASSERT_FALSE(line.getIntersections(circle, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 0U);
	
	points.removeAll();
	
	// end point inside the circle
	line.set(xpcc::Vector2i(-30, -30),
			xpcc::Vector2i(10, 10));
	
	TEST_ASSERT_TRUE(line.getIntersections(circle, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 1U);
	
	TEST_ASSERT_EQUALS(points[0].getX(), -14);
	TEST_ASSERT_EQUALS(points[0].getY(), -14);
	
	points.removeAll();
	
	// start point inside the circle
	line.set(xpcc::Vector2i(-10, -10),
			xpcc::Vector2i(30, 30));
	
	TEST_ASSERT_TRUE(line.getIntersections(circle, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 1U);
	
	TEST_ASSERT_EQUALS(points[0].getX(), 14);
	TEST_ASSERT_EQUALS(points[0].getY(), 14);
	
	points.removeAll();
	
	// two intersections
	line.set(xpcc::Vector2i(30, -30),
			xpcc::Vector2i(-30, 30));
	
	TEST_ASSERT_TRUE(line.getIntersections(circle, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 2U);
	
	TEST_ASSERT_EQUALS(points[0].getX(), 14);
	TEST_ASSERT_EQUALS(points[0].getY(), -14);
	
	TEST_ASSERT_EQUALS(points[1].getX(), -14);
	TEST_ASSERT_EQUALS(points[1].getY(), 14);
	
	points.removeAll();
	
	// line is a tangent
	line.set(xpcc::Vector2i(20, 0),
			xpcc::Vector2i(20, 30));
	
	TEST_ASSERT_TRUE(line.getIntersections(circle, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 1U);
	
	TEST_ASSERT_EQUALS(points[0].getX(), 20);
	TEST_ASSERT_EQUALS(points[0].getY(), 0);
	
	points.removeAll();
	
	// line ends before the circle
	line.set(xpcc::Vector2i(20, 20),
			xpcc::Vector2i(30, 30));
	
	TEST_ASSERT_FALSE(line.getIntersections(circle, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 0U);
	
	points.removeAll();
}
