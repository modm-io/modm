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

#include <xpcc/math/geometry/polygon_2d.hpp>

#include "polygon_2d_test.hpp"

void
Polygon2DTest::testConstructor()
{
	xpcc::Polygon2D<int16_t> polygon(5);
	
	TEST_ASSERT_EQUALS(polygon.getNumberOfPoints(), 0U);
}

void
Polygon2DTest::testAppendAndAccess()
{
	xpcc::Polygon2D<int16_t> polygon(5);
	
	polygon.append(xpcc::Vector2i(10, 20));
	
	TEST_ASSERT_EQUALS(polygon.getNumberOfPoints(), 1U);
	TEST_ASSERT_EQUALS(polygon[0], xpcc::Vector2i(10, 20));
	
	polygon.append(xpcc::Vector2i(30, 40));
	
	TEST_ASSERT_EQUALS(polygon.getNumberOfPoints(), 2U);
	TEST_ASSERT_EQUALS(polygon[0], xpcc::Vector2i(10, 20));
	TEST_ASSERT_EQUALS(polygon[1], xpcc::Vector2i(30, 40));
	
	polygon[0] = xpcc::Vector2i(50, 60);
	
	TEST_ASSERT_EQUALS(polygon[0], xpcc::Vector2i(50, 60));
}

void
Polygon2DTest::testShiftOperator()
{
	xpcc::Polygon2D<int16_t> polygon(2);
	
	polygon << xpcc::Vector2i(10, 20);
	
	TEST_ASSERT_EQUALS(polygon.getNumberOfPoints(), 1U);
	TEST_ASSERT_EQUALS(polygon[0], xpcc::Vector2i(10, 20));
	
	polygon << xpcc::Vector2i(30, 40);
	
	TEST_ASSERT_EQUALS(polygon.getNumberOfPoints(), 2U);
	TEST_ASSERT_EQUALS(polygon[0], xpcc::Vector2i(10, 20));
	TEST_ASSERT_EQUALS(polygon[1], xpcc::Vector2i(30, 40));
	
	polygon << xpcc::Vector2i(50, 60) << xpcc::Vector2i(70, 80);
	
	TEST_ASSERT_EQUALS(polygon.getNumberOfPoints(), 4U);
	TEST_ASSERT_EQUALS(polygon[0], xpcc::Vector2i(10, 20));
	TEST_ASSERT_EQUALS(polygon[1], xpcc::Vector2i(30, 40));
	TEST_ASSERT_EQUALS(polygon[2], xpcc::Vector2i(50, 60));
	TEST_ASSERT_EQUALS(polygon[3], xpcc::Vector2i(70, 80));
}

void
Polygon2DTest::testIntersectionPolygon()
{
	xpcc::Polygon2D<int16_t> polygon1(5);
	polygon1 << xpcc::Vector2i(0, 0)
			 << xpcc::Vector2i(10, 30)
			 << xpcc::Vector2i(50, 30)
			 << xpcc::Vector2i(30, 0)
			 << xpcc::Vector2i(60, -20);
	
	xpcc::Polygon2D<int16_t> polygon2(3);
	polygon2 << xpcc::Vector2i(40, 0)
			 << xpcc::Vector2i(70, 30)
			 << xpcc::Vector2i(80, -10);
	
	xpcc::Polygon2D<int16_t> polygon3(3);
	polygon3 << xpcc::Vector2i(50, 0)
			 << xpcc::Vector2i(20, -30)
			 << xpcc::Vector2i(60, -20);
	
	xpcc::Polygon2D<int16_t> polygon4(5);
	polygon4 << xpcc::Vector2i(20, -60)
			 << xpcc::Vector2i(50, -30)
			 << xpcc::Vector2i(30, -40)
			 << xpcc::Vector2i(30, -10)
			 << xpcc::Vector2i(-10, -10);
	
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
	xpcc::Polygon2D<int16_t> polygon(5);
	polygon << xpcc::Vector2i(0, 0)
			<< xpcc::Vector2i(10, 30)
			<< xpcc::Vector2i(50, 30)
			<< xpcc::Vector2i(30, 0)
			<< xpcc::Vector2i(60, -20);
	
	xpcc::Circle2D<int16_t> circle(xpcc::Vector2i(-20, 0), 20);
	
	TEST_ASSERT_TRUE(polygon.intersects(circle));
	
	// multiple intersections
	circle.setCenter(xpcc::Vector2i(20, 0));
	TEST_ASSERT_TRUE(polygon.intersects(circle));
	TEST_ASSERT_TRUE(circle.intersects(polygon));
	
	circle.setCenter(xpcc::Vector2i(70, -20));
	TEST_ASSERT_TRUE(polygon.intersects(circle));
	TEST_ASSERT_TRUE(circle.intersects(polygon));
	
	circle = xpcc::Circle2D<int16_t>(xpcc::Vector2i(40, 40), 10);
	
	TEST_ASSERT_TRUE(polygon.intersects(circle));
	TEST_ASSERT_TRUE(circle.intersects(polygon));
	
	circle.setCenter(xpcc::Vector2i(40, 10));
	TEST_ASSERT_TRUE(polygon.intersects(circle));
	TEST_ASSERT_TRUE(circle.intersects(polygon));
	
	// circle is inside the polygon
	circle.setCenter(xpcc::Vector2i(20, 10));
	TEST_ASSERT_FALSE(polygon.intersects(circle));
	TEST_ASSERT_FALSE(circle.intersects(polygon));
	
	// wide outside the polygon
	circle.setCenter(xpcc::Vector2i(100, 100));
	TEST_ASSERT_FALSE(polygon.intersects(circle));
	TEST_ASSERT_FALSE(circle.intersects(polygon));
}

void
Polygon2DTest::testIntersectionLineSegment()
{
	xpcc::Polygon2D<int16_t> polygon(5);
	polygon << xpcc::Vector2i(0, 0)
			<< xpcc::Vector2i(10, 30)
			<< xpcc::Vector2i(50, 30)
			<< xpcc::Vector2i(30, 0)
			<< xpcc::Vector2i(60, -20);
	
	// 1. outside the polygon
	xpcc::LineSegment2D<int16_t> line(xpcc::Vector2i(-20, 50),
									  xpcc::Vector2i(0, 30));
	TEST_ASSERT_FALSE(polygon.intersects(line));
	TEST_ASSERT_FALSE(line.intersects(polygon));
	
	// 2. line contained inside the polygon
	line = xpcc::LineSegment2D<int16_t>(xpcc::Vector2i(-20, 50),
										xpcc::Vector2i(0, 30));
	TEST_ASSERT_FALSE(polygon.intersects(line));
	TEST_ASSERT_FALSE(line.intersects(polygon));
	
	// 3.
	line = xpcc::LineSegment2D<int16_t>(xpcc::Vector2i(20, 0),
										xpcc::Vector2i(10, -30));
	TEST_ASSERT_TRUE(polygon.intersects(line));
	TEST_ASSERT_TRUE(line.intersects(polygon));
	
	// 4. intersection at (30, -10)
	line = xpcc::LineSegment2D<int16_t>(xpcc::Vector2i(30, -10),
										xpcc::Vector2i(30, -30));
	TEST_ASSERT_TRUE(polygon.intersects(line));
	TEST_ASSERT_TRUE(line.intersects(polygon));
	
	// 5.
	line = xpcc::LineSegment2D<int16_t>(xpcc::Vector2i(50, -40),
										xpcc::Vector2i(30, 40));
	TEST_ASSERT_TRUE(polygon.intersects(line));
	TEST_ASSERT_TRUE(line.intersects(polygon));
	
	// 6.
	line = xpcc::LineSegment2D<int16_t>(xpcc::Vector2i(40, -10),
										xpcc::Vector2i(30, 40));
	TEST_ASSERT_TRUE(polygon.intersects(line));
	TEST_ASSERT_TRUE(line.intersects(polygon));
	
	// 7.
	line = xpcc::LineSegment2D<int16_t>(xpcc::Vector2i(50, 30),
										xpcc::Vector2i(60, 10));
	TEST_ASSERT_TRUE(polygon.intersects(line));
	TEST_ASSERT_TRUE(line.intersects(polygon));
}

void
Polygon2DTest::testIntersectionPointsLineSegment()
{
	xpcc::Polygon2D<int16_t> polygon(5);
	polygon << xpcc::Vector2i(0, 0)
			<< xpcc::Vector2i(10, 30)
			<< xpcc::Vector2i(50, 30)
			<< xpcc::Vector2i(30, 0)
			<< xpcc::Vector2i(60, -20);
	
	xpcc::PointSet2D<int16_t> points(4);
	
	// 1. outside the polygon
	xpcc::LineSegment2D<int16_t> line(xpcc::Vector2i(-20, 50),
									  xpcc::Vector2i(0, 30));
	TEST_ASSERT_FALSE(polygon.getIntersections(line, points));
	
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 0U);
	
	// 2. line contained inside the polygon
	line = xpcc::LineSegment2D<int16_t>(xpcc::Vector2i(-20, 50),
										xpcc::Vector2i(0, 30));
	TEST_ASSERT_FALSE(polygon.getIntersections(line, points));
	
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 0U);
	
	// 3.
	line = xpcc::LineSegment2D<int16_t>(xpcc::Vector2i(20, 0),
										xpcc::Vector2i(10, -30));
	TEST_ASSERT_TRUE(polygon.getIntersections(line, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 1U);
	
	TEST_ASSERT_EQUALS(points[0].getX(), 18);
	TEST_ASSERT_EQUALS(points[0].getY(), -6);
	
	points.removeAll();
	
	// 4. intersection at (30, -10)
	line = xpcc::LineSegment2D<int16_t>(xpcc::Vector2i(30, -10),
										xpcc::Vector2i(30, -30));
	TEST_ASSERT_TRUE(polygon.getIntersections(line, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 1U);
	
	TEST_ASSERT_EQUALS(points[0].getX(),  30);
	TEST_ASSERT_EQUALS(points[0].getY(), -10);
	
	points.removeAll();
	
	// 5.
	line = xpcc::LineSegment2D<int16_t>(xpcc::Vector2i(50, -40),
										xpcc::Vector2i(30, 40));
	TEST_ASSERT_TRUE(polygon.getIntersections(line, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 4U);
	
	TEST_ASSERT_EQUALS(points[0].getX(), 32);
	TEST_ASSERT_EQUALS(points[0].getY(), 30);
	
	TEST_ASSERT_EQUALS(points[1].getX(), 37);
	TEST_ASSERT_EQUALS(points[1].getY(), 11);
	
	TEST_ASSERT_EQUALS(points[2].getX(), 42);
	TEST_ASSERT_EQUALS(points[2].getY(), -8);
	
	TEST_ASSERT_EQUALS(points[3].getX(),  44);
	TEST_ASSERT_EQUALS(points[3].getY(), -15);
	
	points.removeAll();
	
	// 6.
	line = xpcc::LineSegment2D<int16_t>(xpcc::Vector2i(40, -10),
										xpcc::Vector2i(30, 40));
	TEST_ASSERT_TRUE(polygon.getIntersections(line, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 3U);
	
	TEST_ASSERT_EQUALS(points[0].getX(), 32);
	TEST_ASSERT_EQUALS(points[0].getY(), 30);
	
	TEST_ASSERT_EQUALS(points[1].getX(), 36);
	TEST_ASSERT_EQUALS(points[1].getY(),  9);
	
	TEST_ASSERT_EQUALS(points[2].getX(), 39);
	TEST_ASSERT_EQUALS(points[2].getY(), -6);
	
	points.removeAll();
	
	// 7.
	line = xpcc::LineSegment2D<int16_t>(xpcc::Vector2i(50, 30),
										xpcc::Vector2i(60, 10));
	TEST_ASSERT_TRUE(polygon.getIntersections(line, points));
	TEST_ASSERT_EQUALS(points.getNumberOfPoints(), 2U);
	
	TEST_ASSERT_EQUALS(points[0].getX(), 50);
	TEST_ASSERT_EQUALS(points[0].getY(), 30);
	
	TEST_ASSERT_EQUALS(points[1].getX(), 50);
	TEST_ASSERT_EQUALS(points[1].getY(), 30);
	
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
	xpcc::Polygon2D<Type> polygon(5);
	polygon << xpcc::Vector<Type, 2>(0, 0)
			<< xpcc::Vector<Type, 2>(10, 30)
			<< xpcc::Vector<Type, 2>(50, 30)
			// Change this to 30,0 to generate a concave polygon, the test
			// can be kept the same. Requires a different algorithm.
			<< xpcc::Vector<Type, 2>(55, 0)
			<< xpcc::Vector<Type, 2>(60, -50);
	
	TEST_ASSERT_TRUE(polygon.isInside(xpcc::Vector<Type, 2>(0, 0)));
	TEST_ASSERT_TRUE(polygon.isInside(xpcc::Vector<Type, 2>(5, 0)));
	TEST_ASSERT_FALSE(polygon.isInside(xpcc::Vector<Type, 2>(5, 30)));
	
	TEST_ASSERT_TRUE(polygon.isInside(xpcc::Vector<Type, 2>(30, 29)));
	TEST_ASSERT_FALSE(polygon.isInside(xpcc::Vector<Type, 2>(15, 35)));
	TEST_ASSERT_TRUE(polygon.isInside(xpcc::Vector<Type, 2>(40, 28)));
	TEST_ASSERT_TRUE(polygon.isInside(xpcc::Vector<Type, 2>(40, 20)));
	TEST_ASSERT_TRUE(polygon.isInside(xpcc::Vector<Type, 2>(60, -50)));
	TEST_ASSERT_TRUE(polygon.isInside(xpcc::Vector<Type, 2>(45, -33)));
	TEST_ASSERT_FALSE(polygon.isInside(xpcc::Vector<Type, 2>(70, -40)));
	TEST_ASSERT_FALSE(polygon.isInside(xpcc::Vector<Type, 2>(30, -40)));
	TEST_ASSERT_FALSE(polygon.isInside(xpcc::Vector<Type, 2>(-1, 0)));
}

void
Polygon2DTest::testPointContainedCCW()
{
	xpcc::Polygon2D<int16_t> polygon(5);
		polygon << xpcc::Vector2i(0, 0)
				<< xpcc::Vector2i(60, -50)
				<< xpcc::Vector2i(30, 0)
				<< xpcc::Vector2i(50, 30)
				<< xpcc::Vector2i(10, 30);
		
	TEST_ASSERT_TRUE(polygon.isInside(xpcc::Vector<int16_t, 2>(0, 0)));
	TEST_ASSERT_TRUE(polygon.isInside(xpcc::Vector<int16_t, 2>(5, 0)));
	TEST_ASSERT_FALSE(polygon.isInside(xpcc::Vector<int16_t, 2>(5, 30)));
	TEST_ASSERT_TRUE(polygon.isInside(xpcc::Vector<int16_t, 2>(30, 30)));
	TEST_ASSERT_FALSE(polygon.isInside(xpcc::Vector<int16_t, 2>(15, 35)));
	TEST_ASSERT_FALSE(polygon.isInside(xpcc::Vector<int16_t, 2>(40, 28)));
	TEST_ASSERT_FALSE(polygon.isInside(xpcc::Vector<int16_t, 2>(40, 20)));
	TEST_ASSERT_TRUE(polygon.isInside(xpcc::Vector<int16_t, 2>(60, -50)));
	TEST_ASSERT_FALSE(polygon.isInside(xpcc::Vector<int16_t, 2>(70, -40)));
	TEST_ASSERT_FALSE(polygon.isInside(xpcc::Vector<int16_t, 2>(30, -40)));
	TEST_ASSERT_FALSE(polygon.isInside(xpcc::Vector<int16_t, 2>(-1, 0)));
}
