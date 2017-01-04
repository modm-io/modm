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

#include <modm/math/geometry/line_2d.hpp>

#include "line_2d_test.hpp"

void
Line2DTest::testDefaultConstructor()
{
	xpcc::Line2D<int16_t> line;
	
	TEST_ASSERT_EQUALS(line.getPoint(), xpcc::Vector2i(0, 0));
	TEST_ASSERT_EQUALS(line.getDirectionVector(), xpcc::Vector2i(0, 0));
}

void
Line2DTest::testConstructor()
{
	xpcc::Line2D<int16_t> line(
			xpcc::Vector2i(-20, -60),
			xpcc::Vector2i(90, 80));
	
	TEST_ASSERT_EQUALS(line.getPoint(), xpcc::Vector2i(-20, -60));
	TEST_ASSERT_EQUALS(line.getDirectionVector(), xpcc::Vector2i(90, 80));
}

void
Line2DTest::testAccessors()
{
	xpcc::Line2D<int16_t> line;
	
	line.setPoint(xpcc::Vector2i(40, 780));
	line.setDirectionVector(xpcc::Vector2i(-50, 16));
	
	TEST_ASSERT_EQUALS(line.getPoint(), xpcc::Vector2i(40, 780));
	TEST_ASSERT_EQUALS(line.getDirectionVector(), xpcc::Vector2i(-50, 16));
}

void
Line2DTest::testDistanceToPoint()
{
	xpcc::Line2D<int16_t> line(
			xpcc::Vector2i(-20, -60),
			xpcc::Vector2i(10, 10));
	
	int16_t distance = line.getDistanceTo(xpcc::Vector2i(10, 30));
	
	// closest point is (40, 0) => sqrt(30^2 + 30^2) = 42.4264..
	TEST_ASSERT_EQUALS(distance, 42);
	
	TEST_ASSERT_EQUALS(line.getPoint(), xpcc::Vector2i(-20, -60));
	TEST_ASSERT_EQUALS(line.getDirectionVector(), xpcc::Vector2i(10, 10));
	
	int16_t distance2 = line.getDistanceTo(xpcc::Vector2i(-50, -50));
	
	// closest point is (-30, -70) => sqrt(20^2 + 20^2) = 28.28427..
	TEST_ASSERT_EQUALS(distance2, 28);
	
	int16_t distance3 = line.getDistanceTo(xpcc::Vector2i(70, -70));
	
	// closest point is (20, -20) => sqrt(50^2 + 50^2) = 70.7106..
	TEST_ASSERT_EQUALS(distance3, 71);
}

void
Line2DTest::testIntersectionPointsLine()
{
	xpcc::Line2D<int16_t> line1(
			xpcc::Vector2i(0, 10),
			xpcc::Vector2i(10, 10));
	
	xpcc::Line2D<int16_t> line2(
			xpcc::Vector2i(10, 0),
			xpcc::Vector2i(20, 20));
	
	xpcc::PointSet2D<int16_t> points;
	
	// two parallel line
	TEST_ASSERT_FALSE(line1.getIntersections(line2, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 0U);
	
	// intersection at (10, 20)
	line2.setDirectionVector(xpcc::Vector2i(0, 10));
	TEST_ASSERT_TRUE(line1.getIntersections(line2, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 1U);
	
	TEST_ASSERT_EQUALS(points[0].getX(), 10);
	TEST_ASSERT_EQUALS(points[0].getY(), 20);
	
	points.removeAll();
	
	// intersection at (10, -30)
	line1.setDirectionVector(xpcc::Vector2i(-10, 30));
	TEST_ASSERT_TRUE(line1.getIntersections(line2, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 1U);
	
	TEST_ASSERT_EQUALS(points[0].getX(), 10);
	TEST_ASSERT_EQUALS(points[0].getY(), -20);
}

void
Line2DTest::testIntersectionPointsCircle()
{
	xpcc::Line2D<int16_t> line(
			xpcc::Vector2i(-10, -10),
			xpcc::Vector2i(10, 10));
	
	xpcc::Circle2D<int16_t> circle(
			xpcc::Vector2i(0, 0),
			20);
	
	xpcc::PointSet2D<int16_t> points;
	
	// two intersections
	TEST_ASSERT_TRUE(line.getIntersections(circle, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 2U);
	
	TEST_ASSERT_EQUALS(points[0].getX(), -14);
	TEST_ASSERT_EQUALS(points[0].getY(), -14);
	
	TEST_ASSERT_EQUALS(points[1].getX(), 14);
	TEST_ASSERT_EQUALS(points[1].getY(), 14);
	
	points.removeAll();
	
	// line is a tangent
	line.set(xpcc::Vector2i(20, 0),
			xpcc::Vector2i(0, -10));
	
	TEST_ASSERT_TRUE(line.getIntersections(circle, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 1U);
	
	TEST_ASSERT_EQUALS(points[0].getX(), 20);
	TEST_ASSERT_EQUALS(points[0].getY(), 0);
	
	points.removeAll();
	
	// no intersections
	line.set(xpcc::Vector2i(20, 20),
			xpcc::Vector2i(10, -10));
	
	TEST_ASSERT_FALSE(line.getIntersections(circle, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 0U);
	
	points.removeAll();
}
