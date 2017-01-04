/*
 * Copyright (c) 2013, Fabian Greif
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/math/geometry/ray_2d.hpp>

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
