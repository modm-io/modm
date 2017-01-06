/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009, Thorsten Lajewski
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/math/geometry/line_segment_2d.hpp>

#include "line_segment_2d_test.hpp"

void
LineSegment2DTest::testDefaultConstructor()
{
	modm::LineSegment2D<int16_t> line;
	
	TEST_ASSERT_EQUALS(line.getStartPoint(), modm::Vector2i(0, 0));
	TEST_ASSERT_EQUALS(line.getEndPoint(), modm::Vector2i(0, 0));
}

void
LineSegment2DTest::testConstructor()
{
	modm::LineSegment2D<int16_t> line(
			modm::Vector2i(-20, -60),
			modm::Vector2i(70, 30));
	
	TEST_ASSERT_EQUALS(line.getStartPoint(), modm::Vector2i(-20, -60));
	TEST_ASSERT_EQUALS(line.getEndPoint(), modm::Vector2i(70, 30));
}

void
LineSegment2DTest::testAccessor()
{
	modm::LineSegment2D<int16_t> line;
	
	line.setStartPoint(modm::Vector2i(30, 40));
	
	TEST_ASSERT_EQUALS(line.getStartPoint(), modm::Vector2i(30, 40));
	TEST_ASSERT_EQUALS(line.getEndPoint(), modm::Vector2i(0, 0));
	
	line.setEndPoint(modm::Vector2i(50, 60));
	
	TEST_ASSERT_EQUALS(line.getStartPoint(), modm::Vector2i(30, 40));
	TEST_ASSERT_EQUALS(line.getEndPoint(), modm::Vector2i(50, 60));
	
	line.set(modm::Vector2i(-60, -40),
			modm::Vector2i(70, 80));
	
	TEST_ASSERT_EQUALS(line.getStartPoint(), modm::Vector2i(-60, -40));
	TEST_ASSERT_EQUALS(line.getEndPoint(), modm::Vector2i(70, 80));
}

void
LineSegment2DTest::testTranslation()
{
	modm::LineSegment2D<int16_t> line(
			modm::Vector2i(12, 34),
			modm::Vector2i(56, 78));
	
	line.translate(modm::Vector2i(10, 20));
	
	TEST_ASSERT_EQUALS(line.getStartPoint(), modm::Vector2i(22, 54));
	TEST_ASSERT_EQUALS(line.getEndPoint(), modm::Vector2i(66, 98));
}

void
LineSegment2DTest::testComparision()
{
	modm::LineSegment2D<int16_t> a(
			modm::Vector2i(12, 34),
			modm::Vector2i(56, 78));
	
	modm::LineSegment2D<int16_t> b(
			modm::Vector2i(12, 34),
			modm::Vector2i(0, 0));
	
	modm::LineSegment2D<int16_t> c(
			modm::Vector2i(12, 34),
			modm::Vector2i(56, 78));
	
	TEST_ASSERT_FALSE(a == b);
	TEST_ASSERT_TRUE(a != b);
	
	TEST_ASSERT_TRUE(a == c);
	TEST_ASSERT_FALSE(a != c);
}

void
LineSegment2DTest::testLength()
{
	modm::LineSegment2D<int16_t> line(
			modm::Vector2i(0, 0),
			modm::Vector2i(70, 0));
	
	TEST_ASSERT_EQUALS(line.getLength(), 70);
	
	line.set(modm::Vector2i(-20, -60),
			modm::Vector2i(70, 30));
	
	TEST_ASSERT_EQUALS(line.getLength(), 127);
}

void
LineSegment2DTest::testLengthFloat()
{
	modm::LineSegment2D<float> line(
			modm::Vector<float, 2>(0, 0),
			modm::Vector<float, 2>(70, 10));
	
	TEST_ASSERT_EQUALS_FLOAT(line.getLength(), 70.71067812f);
}

void
LineSegment2DTest::testDistanceToPoint()
{
	modm::LineSegment2D<int16_t> line(
			modm::Vector2i(-20, -60),
			modm::Vector2i(50, 10));
	
	int16_t distance = line.getDistanceTo(modm::Vector2i(10, 30));
	
	// closest point is (40, 0) => sqrt(30^2 + 30^2) = 42.4264..
	TEST_ASSERT_EQUALS(distance, 42);
	
	TEST_ASSERT_EQUALS(line.getStartPoint(), modm::Vector2i(-20, -60));
	TEST_ASSERT_EQUALS(line.getEndPoint(), modm::Vector2i(50, 10));
	
	int16_t distance2 = line.getDistanceTo(modm::Vector2i(-50, -70));
	
	// closest point of the line is before the start point
	// => closest point is (-20, -60) => sqrt(30^2 + 10^2) = 31.6227766..
	TEST_ASSERT_EQUALS(distance2, 32);
	
	int16_t distance3 = line.getDistanceTo(modm::Vector2i(100, 20));
	
	// closest point of the line is after the end point
	// closest point is (50, 10) => sqrt(50^2 + 10^2) = 50.9901951..
	TEST_ASSERT_EQUALS(distance3, 51);
}

void
LineSegment2DTest::testIntersectsLineSegment()
{
	modm::LineSegment2D<int16_t> line1(
			modm::Vector2i(-20, -60),
			modm::Vector2i(50, 10));
	
	modm::LineSegment2D<int16_t> line2(
			modm::Vector2i(0, 0),
			modm::Vector2i(90, 30));
	
	modm::LineSegment2D<int16_t> line3(
			modm::Vector2i(10, -60),
			modm::Vector2i(40, -30));
	
	modm::LineSegment2D<int16_t> line4(
			modm::Vector2i(50, 10),
			modm::Vector2i(50, 50));
	
	modm::LineSegment2D<int16_t> line5(
			modm::Vector2i(10, 30),
			modm::Vector2i(70, -30));
	
	modm::LineSegment2D<int16_t> line6(
			modm::Vector2i(30, -40),
			modm::Vector2i(60, 50));
	
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
	modm::LineSegment2D<int16_t> line1(
			modm::Vector2i(-20, -60),
			modm::Vector2i(50, 10));
	
	modm::LineSegment2D<int16_t> line2(
			modm::Vector2i(0, 0),
			modm::Vector2i(90, 30));
	
	modm::LineSegment2D<int16_t> line3(
			modm::Vector2i(10, -60),
			modm::Vector2i(40, -30));
	
	modm::LineSegment2D<int16_t> line4(
			modm::Vector2i(50, 10),
			modm::Vector2i(50, 50));
	
	modm::LineSegment2D<int16_t> line5(
			modm::Vector2i(10, 30),
			modm::Vector2i(70, -30));
	
	modm::PointSet2D<int16_t> points;
	
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
	modm::LineSegment2D<int16_t> line(
			modm::Vector2i(-10, -10),
			modm::Vector2i(10, 10));
	
	modm::Circle2D<int16_t> circle(
			modm::Vector2i(0, 0),
			20);
	
	modm::PointSet2D<int16_t> points;
	
	// line is contained inside the circle
	TEST_ASSERT_FALSE(line.getIntersections(circle, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 0U);
	
	points.removeAll();
	
	// end point inside the circle
	line.set(modm::Vector2i(-30, -30),
			modm::Vector2i(10, 10));
	
	TEST_ASSERT_TRUE(line.getIntersections(circle, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 1U);
	
	TEST_ASSERT_EQUALS(points[0].getX(), -14);
	TEST_ASSERT_EQUALS(points[0].getY(), -14);
	
	points.removeAll();
	
	// start point inside the circle
	line.set(modm::Vector2i(-10, -10),
			modm::Vector2i(30, 30));
	
	TEST_ASSERT_TRUE(line.getIntersections(circle, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 1U);
	
	TEST_ASSERT_EQUALS(points[0].getX(), 14);
	TEST_ASSERT_EQUALS(points[0].getY(), 14);
	
	points.removeAll();
	
	// two intersections
	line.set(modm::Vector2i(30, -30),
			modm::Vector2i(-30, 30));
	
	TEST_ASSERT_TRUE(line.getIntersections(circle, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 2U);
	
	TEST_ASSERT_EQUALS(points[0].getX(), 14);
	TEST_ASSERT_EQUALS(points[0].getY(), -14);
	
	TEST_ASSERT_EQUALS(points[1].getX(), -14);
	TEST_ASSERT_EQUALS(points[1].getY(), 14);
	
	points.removeAll();
	
	// line is a tangent
	line.set(modm::Vector2i(20, 0),
			modm::Vector2i(20, 30));
	
	TEST_ASSERT_TRUE(line.getIntersections(circle, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 1U);
	
	TEST_ASSERT_EQUALS(points[0].getX(), 20);
	TEST_ASSERT_EQUALS(points[0].getY(), 0);
	
	points.removeAll();
	
	// line ends before the circle
	line.set(modm::Vector2i(20, 20),
			modm::Vector2i(30, 30));
	
	TEST_ASSERT_FALSE(line.getIntersections(circle, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 0U);
	
	points.removeAll();
}
