/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009, Thorsten Lajewski
 * Copyright (c) 2009-2011, 2013, 2015, Fabian Greif
 * Copyright (c) 2012, 2015, Niklas Hauser
 * Copyright (c) 2015, Kevin Läufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/math/geometry/polygon_2d.hpp>

#include "polygon_2d_test.hpp"

void
Polygon2DTest::testConstructor()
{
	modm::Polygon2D<int16_t> polygon(5);

	TEST_ASSERT_EQUALS(polygon.getNumberOfPoints(), 0U);
}

void
Polygon2DTest::testInitializerListConstructor()
{
	modm::Polygon2D<int16_t> polygon {
		modm::Vector2i(1,35), modm::Vector2i(56,2), modm::Vector2i(3,76),
		modm::Vector2i(19,4), modm::Vector2i(93,5), modm::Vector2i(6,66) };

	TEST_ASSERT_EQUALS(polygon.getNumberOfPoints(), 6U);
	TEST_ASSERT_EQUALS(polygon[0], modm::Vector2i(1,35));
	TEST_ASSERT_EQUALS(polygon[1], modm::Vector2i(56,2));
	TEST_ASSERT_EQUALS(polygon[2], modm::Vector2i(3,76));
	TEST_ASSERT_EQUALS(polygon[3], modm::Vector2i(19,4));
	TEST_ASSERT_EQUALS(polygon[4], modm::Vector2i(93,5));
	TEST_ASSERT_EQUALS(polygon[5], modm::Vector2i(6,66));

}

void
Polygon2DTest::testAppendAndAccess()
{
	modm::Polygon2D<int16_t> polygon(5);

	polygon.append(modm::Vector2i(10, 20));

	TEST_ASSERT_EQUALS(polygon.getNumberOfPoints(), 1U);
	TEST_ASSERT_EQUALS(polygon[0], modm::Vector2i(10, 20));

	polygon.append(modm::Vector2i(30, 40));

	TEST_ASSERT_EQUALS(polygon.getNumberOfPoints(), 2U);
	TEST_ASSERT_EQUALS(polygon[0], modm::Vector2i(10, 20));
	TEST_ASSERT_EQUALS(polygon[1], modm::Vector2i(30, 40));

	polygon[0] = modm::Vector2i(50, 60);

	TEST_ASSERT_EQUALS(polygon[0], modm::Vector2i(50, 60));
}

void
Polygon2DTest::testShiftOperator()
{
	modm::Polygon2D<int16_t> polygon(2);

	polygon << modm::Vector2i(10, 20);

	TEST_ASSERT_EQUALS(polygon.getNumberOfPoints(), 1U);
	TEST_ASSERT_EQUALS(polygon[0], modm::Vector2i(10, 20));

	polygon << modm::Vector2i(30, 40);

	TEST_ASSERT_EQUALS(polygon.getNumberOfPoints(), 2U);
	TEST_ASSERT_EQUALS(polygon[0], modm::Vector2i(10, 20));
	TEST_ASSERT_EQUALS(polygon[1], modm::Vector2i(30, 40));

	polygon << modm::Vector2i(50, 60) << modm::Vector2i(70, 80);

	TEST_ASSERT_EQUALS(polygon.getNumberOfPoints(), 4U);
	TEST_ASSERT_EQUALS(polygon[0], modm::Vector2i(10, 20));
	TEST_ASSERT_EQUALS(polygon[1], modm::Vector2i(30, 40));
	TEST_ASSERT_EQUALS(polygon[2], modm::Vector2i(50, 60));
	TEST_ASSERT_EQUALS(polygon[3], modm::Vector2i(70, 80));
}

void
Polygon2DTest::testIntersectionPolygon()
{
	modm::Polygon2D<int16_t> polygon1(5);
	polygon1 << modm::Vector2i(0, 0)
			 << modm::Vector2i(10, 30)
			 << modm::Vector2i(50, 30)
			 << modm::Vector2i(30, 0)
			 << modm::Vector2i(60, -20);

	modm::Polygon2D<int16_t> polygon2(3);
	polygon2 << modm::Vector2i(40, 0)
			 << modm::Vector2i(70, 30)
			 << modm::Vector2i(80, -10);

	modm::Polygon2D<int16_t> polygon3(3);
	polygon3 << modm::Vector2i(50, 0)
			 << modm::Vector2i(20, -30)
			 << modm::Vector2i(60, -20);

	modm::Polygon2D<int16_t> polygon4(5);
	polygon4 << modm::Vector2i(20, -60)
			 << modm::Vector2i(50, -30)
			 << modm::Vector2i(30, -40)
			 << modm::Vector2i(30, -10)
			 << modm::Vector2i(-10, -10);

	TEST_ASSERT_FALSE(polygon1.intersects(polygon2));
	TEST_ASSERT_TRUE(polygon1.intersects(polygon3));
	TEST_ASSERT_TRUE(polygon1.intersects(polygon4));

	TEST_ASSERT_TRUE(polygon2.intersects(polygon3));
	TEST_ASSERT_FALSE(polygon2.intersects(polygon4));

	TEST_ASSERT_TRUE(polygon3.intersects(polygon4));
}

void
Polygon2DTest::testIntersectionCircle()
{
	modm::Polygon2D<int16_t> polygon(5);
	polygon << modm::Vector2i(0, 0)
			<< modm::Vector2i(10, 30)
			<< modm::Vector2i(50, 30)
			<< modm::Vector2i(30, 0)
			<< modm::Vector2i(60, -20);

	modm::Circle2D<int16_t> circle(modm::Vector2i(-20, 0), 20);

	TEST_ASSERT_TRUE(polygon.intersects(circle));

	// multiple intersections
	circle.setCenter(modm::Vector2i(20, 0));
	TEST_ASSERT_TRUE(polygon.intersects(circle));
	TEST_ASSERT_TRUE(circle.intersects(polygon));

	circle.setCenter(modm::Vector2i(70, -20));
	TEST_ASSERT_TRUE(polygon.intersects(circle));
	TEST_ASSERT_TRUE(circle.intersects(polygon));

	circle = modm::Circle2D<int16_t>(modm::Vector2i(40, 40), 10);

	TEST_ASSERT_TRUE(polygon.intersects(circle));
	TEST_ASSERT_TRUE(circle.intersects(polygon));

	circle.setCenter(modm::Vector2i(40, 10));
	TEST_ASSERT_TRUE(polygon.intersects(circle));
	TEST_ASSERT_TRUE(circle.intersects(polygon));

	// circle is inside the polygon
	circle.setCenter(modm::Vector2i(20, 10));
	TEST_ASSERT_FALSE(polygon.intersects(circle));
	TEST_ASSERT_FALSE(circle.intersects(polygon));

	// wide outside the polygon
	circle.setCenter(modm::Vector2i(100, 100));
	TEST_ASSERT_FALSE(polygon.intersects(circle));
	TEST_ASSERT_FALSE(circle.intersects(polygon));
}

void
Polygon2DTest::testIntersectionLineSegment()
{
	modm::Polygon2D<int16_t> polygon(5);
	polygon << modm::Vector2i(0, 0)
			<< modm::Vector2i(10, 30)
			<< modm::Vector2i(50, 30)
			<< modm::Vector2i(30, 0)
			<< modm::Vector2i(60, -20);

	// 1. outside the polygon
	modm::LineSegment2D<int16_t> line(modm::Vector2i(-20, 50),
									  modm::Vector2i(0, 30));
	TEST_ASSERT_FALSE(polygon.intersects(line));
	TEST_ASSERT_FALSE(line.intersects(polygon));

	// 2. line contained inside the polygon
	line = modm::LineSegment2D<int16_t>(modm::Vector2i(-20, 50),
										modm::Vector2i(0, 30));
	TEST_ASSERT_FALSE(polygon.intersects(line));
	TEST_ASSERT_FALSE(line.intersects(polygon));

	// 3.
	line = modm::LineSegment2D<int16_t>(modm::Vector2i(20, 0),
										modm::Vector2i(10, -30));
	TEST_ASSERT_TRUE(polygon.intersects(line));
	TEST_ASSERT_TRUE(line.intersects(polygon));

	// 4. intersection at (30, -10)
	line = modm::LineSegment2D<int16_t>(modm::Vector2i(30, -10),
										modm::Vector2i(30, -30));
	TEST_ASSERT_TRUE(polygon.intersects(line));
	TEST_ASSERT_TRUE(line.intersects(polygon));

	// 5.
	line = modm::LineSegment2D<int16_t>(modm::Vector2i(50, -40),
										modm::Vector2i(30, 40));
	TEST_ASSERT_TRUE(polygon.intersects(line));
	TEST_ASSERT_TRUE(line.intersects(polygon));

	// 6.
	line = modm::LineSegment2D<int16_t>(modm::Vector2i(40, -10),
										modm::Vector2i(30, 40));
	TEST_ASSERT_TRUE(polygon.intersects(line));
	TEST_ASSERT_TRUE(line.intersects(polygon));

	// 7.
	line = modm::LineSegment2D<int16_t>(modm::Vector2i(50, 30),
										modm::Vector2i(60, 10));
	TEST_ASSERT_TRUE(polygon.intersects(line));
	TEST_ASSERT_TRUE(line.intersects(polygon));
}

void
Polygon2DTest::testIntersectionPointsLineSegment()
{
	modm::Polygon2D<int16_t> polygon(5);
	polygon << modm::Vector2i(0, 0)
			<< modm::Vector2i(10, 30)
			<< modm::Vector2i(50, 30)
			<< modm::Vector2i(30, 0)
			<< modm::Vector2i(60, -20);

	modm::PointSet2D<int16_t> points(4);

	// 1. outside the polygon
	modm::LineSegment2D<int16_t> line(modm::Vector2i(-20, 50),
									  modm::Vector2i(0, 30));
	TEST_ASSERT_FALSE(polygon.getIntersections(line, points));

	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 0U);

	// 2. line contained inside the polygon
	line = modm::LineSegment2D<int16_t>(modm::Vector2i(-20, 50),
										modm::Vector2i(0, 30));
	TEST_ASSERT_FALSE(polygon.getIntersections(line, points));

	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 0U);

	// 3.
	line = modm::LineSegment2D<int16_t>(modm::Vector2i(20, 0),
										modm::Vector2i(10, -30));
	TEST_ASSERT_TRUE(polygon.getIntersections(line, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 1U);

	TEST_ASSERT_EQUALS(points[0].x(), 18);
	TEST_ASSERT_EQUALS(points[0].y(), -6);

	points.removeAll();

	// 4. intersection at (30, -10)
	line = modm::LineSegment2D<int16_t>(modm::Vector2i(30, -10),
										modm::Vector2i(30, -30));
	TEST_ASSERT_TRUE(polygon.getIntersections(line, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 1U);

	TEST_ASSERT_EQUALS(points[0].x(),  30);
	TEST_ASSERT_EQUALS(points[0].y(), -10);

	points.removeAll();

	// 5.
	line = modm::LineSegment2D<int16_t>(modm::Vector2i(50, -40),
										modm::Vector2i(30, 40));
	TEST_ASSERT_TRUE(polygon.getIntersections(line, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 4U);

	TEST_ASSERT_EQUALS(points[0].x(), 32);
	TEST_ASSERT_EQUALS(points[0].y(), 30);

	TEST_ASSERT_EQUALS(points[1].x(), 37);
	TEST_ASSERT_EQUALS(points[1].y(), 11);

	TEST_ASSERT_EQUALS(points[2].x(), 42);
	TEST_ASSERT_EQUALS(points[2].y(), -8);

	TEST_ASSERT_EQUALS(points[3].x(),  44);
	TEST_ASSERT_EQUALS(points[3].y(), -15);

	points.removeAll();

	// 6.
	line = modm::LineSegment2D<int16_t>(modm::Vector2i(40, -10),
										modm::Vector2i(30, 40));
	TEST_ASSERT_TRUE(polygon.getIntersections(line, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 3U);

	TEST_ASSERT_EQUALS(points[0].x(), 32);
	TEST_ASSERT_EQUALS(points[0].y(), 30);

	TEST_ASSERT_EQUALS(points[1].x(), 36);
	TEST_ASSERT_EQUALS(points[1].y(),  9);

	TEST_ASSERT_EQUALS(points[2].x(), 39);
	TEST_ASSERT_EQUALS(points[2].y(), -6);

	points.removeAll();

	// 7.
	line = modm::LineSegment2D<int16_t>(modm::Vector2i(50, 30),
										modm::Vector2i(60, 10));
	TEST_ASSERT_TRUE(polygon.getIntersections(line, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 2U);

	TEST_ASSERT_EQUALS(points[0].x(), 50);
	TEST_ASSERT_EQUALS(points[0].y(), 30);

	TEST_ASSERT_EQUALS(points[1].x(), 50);
	TEST_ASSERT_EQUALS(points[1].y(), 30);

	points.removeAll();
}

/**
 * Check if a point is contained inside the area of a polygon.
 *
 * See SVG image for a graphical representation of the polygon and the
 * test points.
 */
void
Polygon2DTest::testPointContainedCW()
{
	typedef int16_t Type;
	//typedef float Type;

	// this is a convex polygon
	modm::Polygon2D<Type> polygon(5);
	polygon << modm::Vector<Type, 2>(0, 0)
			<< modm::Vector<Type, 2>(10, 30)
			<< modm::Vector<Type, 2>(50, 30)
			// Change this to 30,0 to generate a concave polygon, the test
			// can be kept the same. Requires a different algorithm.
			<< modm::Vector<Type, 2>(55, 0)
			<< modm::Vector<Type, 2>(60, -50);

	TEST_ASSERT_TRUE(polygon.isInside(modm::Vector<Type, 2>(0, 0)));
	TEST_ASSERT_TRUE(polygon.isInside(modm::Vector<Type, 2>(5, 0)));
	TEST_ASSERT_FALSE(polygon.isInside(modm::Vector<Type, 2>(5, 30)));

	TEST_ASSERT_TRUE(polygon.isInside(modm::Vector<Type, 2>(30, 29)));
	TEST_ASSERT_FALSE(polygon.isInside(modm::Vector<Type, 2>(15, 35)));
	TEST_ASSERT_TRUE(polygon.isInside(modm::Vector<Type, 2>(40, 28)));
	TEST_ASSERT_TRUE(polygon.isInside(modm::Vector<Type, 2>(40, 20)));
	TEST_ASSERT_TRUE(polygon.isInside(modm::Vector<Type, 2>(60, -50)));
	TEST_ASSERT_TRUE(polygon.isInside(modm::Vector<Type, 2>(45, -33)));
	TEST_ASSERT_FALSE(polygon.isInside(modm::Vector<Type, 2>(70, -40)));
	TEST_ASSERT_FALSE(polygon.isInside(modm::Vector<Type, 2>(30, -40)));
	TEST_ASSERT_FALSE(polygon.isInside(modm::Vector<Type, 2>(-1, 0)));
}

void
Polygon2DTest::testPointContainedCCW()
{
	modm::Polygon2D<int16_t> polygon(5);
		polygon << modm::Vector2i(0, 0)
				<< modm::Vector2i(60, -50)
				<< modm::Vector2i(30, 0)
				<< modm::Vector2i(50, 30)
				<< modm::Vector2i(10, 30);

	TEST_ASSERT_TRUE(polygon.isInside(modm::Vector<int16_t, 2>(0, 0)));
	TEST_ASSERT_TRUE(polygon.isInside(modm::Vector<int16_t, 2>(5, 0)));
	TEST_ASSERT_FALSE(polygon.isInside(modm::Vector<int16_t, 2>(5, 30)));
	TEST_ASSERT_TRUE(polygon.isInside(modm::Vector<int16_t, 2>(30, 30)));
	TEST_ASSERT_FALSE(polygon.isInside(modm::Vector<int16_t, 2>(15, 35)));
	TEST_ASSERT_FALSE(polygon.isInside(modm::Vector<int16_t, 2>(40, 28)));
	TEST_ASSERT_FALSE(polygon.isInside(modm::Vector<int16_t, 2>(40, 20)));
	TEST_ASSERT_TRUE(polygon.isInside(modm::Vector<int16_t, 2>(60, -50)));
	TEST_ASSERT_FALSE(polygon.isInside(modm::Vector<int16_t, 2>(70, -40)));
	TEST_ASSERT_FALSE(polygon.isInside(modm::Vector<int16_t, 2>(30, -40)));
	TEST_ASSERT_FALSE(polygon.isInside(modm::Vector<int16_t, 2>(-1, 0)));
}
