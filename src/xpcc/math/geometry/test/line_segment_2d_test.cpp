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
 * $Id$
 */
// ----------------------------------------------------------------------------

#include <xpcc/math/geometry/line_segment_2d.hpp>

#include "line_segment_2d_test.hpp"

void
LineSegment2DTest::testDefaultConstructor()
{
	xpcc::LineSegment2D<int16_t> line;
	
	TEST_ASSERT_EQUALS(line.getStartPoint(), xpcc::Point2D<int16_t>(0, 0));
	TEST_ASSERT_EQUALS(line.getEndPoint(), xpcc::Point2D<int16_t>(0, 0));
}

void
LineSegment2DTest::testConstructor()
{
	xpcc::LineSegment2D<int16_t> line(
			xpcc::Point2D<int16_t>(-20, -60),
			xpcc::Point2D<int16_t>(70, 30));
	
	TEST_ASSERT_EQUALS(line.getStartPoint(), xpcc::Point2D<int16_t>(-20, -60));
	TEST_ASSERT_EQUALS(line.getEndPoint(), xpcc::Point2D<int16_t>(70, 30));
}

void
LineSegment2DTest::testAccessor()
{
	xpcc::LineSegment2D<int16_t> line;
	
	line.setStartPoint(xpcc::Point2D<int16_t>(30, 40));
	
	TEST_ASSERT_EQUALS(line.getStartPoint(), xpcc::Point2D<int16_t>(30, 40));
	TEST_ASSERT_EQUALS(line.getEndPoint(), xpcc::Point2D<int16_t>(0, 0));
	
	line.setEndPoint(xpcc::Point2D<int16_t>(50, 60));
	
	TEST_ASSERT_EQUALS(line.getStartPoint(), xpcc::Point2D<int16_t>(30, 40));
	TEST_ASSERT_EQUALS(line.getEndPoint(), xpcc::Point2D<int16_t>(50, 60));
	
	line.setPoints(
			xpcc::Point2D<int16_t>(-60, -40),
			xpcc::Point2D<int16_t>(70, 80));
	
	TEST_ASSERT_EQUALS(line.getStartPoint(), xpcc::Point2D<int16_t>(-60, -40));
	TEST_ASSERT_EQUALS(line.getEndPoint(), xpcc::Point2D<int16_t>(70, 80));
}

void
LineSegment2DTest::testLength()
{
	xpcc::LineSegment2D<int16_t> line(
			xpcc::Point2D<int16_t>(0, 0),
			xpcc::Point2D<int16_t>(70, 0));
	
	TEST_ASSERT_EQUALS(line.getLength(), 70);
	
	line.setPoints(
			xpcc::Point2D<int16_t>(-20, -60),
			xpcc::Point2D<int16_t>(70, 30));
	
	TEST_ASSERT_EQUALS(line.getLength(), 127);
}

void
LineSegment2DTest::testLengthFloat()
{
	xpcc::LineSegment2D<float> line(
			xpcc::Point2D<float>(0, 0),
			xpcc::Point2D<float>(70, 10));
	
	TEST_ASSERT_EQUALS_FLOAT(line.getLength(), 70.71067812f);
}

void
LineSegment2DTest::testDistanceToPoint()
{
	xpcc::LineSegment2D<int16_t> line(
			xpcc::Point2D<int16_t>(-20, -60),
			xpcc::Point2D<int16_t>(50, 10));
	
	int16_t distance = line.getDistanceTo(xpcc::Point2D<int16_t>(10, 30));
	
	// closest point is (40, 0) => sqrt(30^2 + 30^2) = 42.4264..
	TEST_ASSERT_EQUALS(distance, 42);
	
	TEST_ASSERT_EQUALS(line.getStartPoint(), xpcc::Point2D<int16_t>(-20, -60));
	TEST_ASSERT_EQUALS(line.getEndPoint(), xpcc::Point2D<int16_t>(50, 10));
	
	int16_t distance2 = line.getDistanceTo(xpcc::Point2D<int16_t>(-50, -70));
	
	// closest point of the line is before the start point
	// => closest point is (-20, -60) => sqrt(30^2 + 10^2) = 31.6227766..
	TEST_ASSERT_EQUALS(distance2, 32);
	
	int16_t distance3 = line.getDistanceTo(xpcc::Point2D<int16_t>(100, 20));
	
	// closest point of the line is after the end point
	// closest point is (50, 10) => sqrt(50^2 + 10^2) = 50.9901951..
	TEST_ASSERT_EQUALS(distance3, 51);
}
