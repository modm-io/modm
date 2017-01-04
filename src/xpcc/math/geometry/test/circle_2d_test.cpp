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

#include <modm/math/geometry/circle_2d.hpp>

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
