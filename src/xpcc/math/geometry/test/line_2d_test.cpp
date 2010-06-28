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

#include <xpcc/math/geometry/line_2d.hpp>

#include "line_2d_test.hpp"

void
Line2DTest::testDefaultConstructor()
{
	xpcc::Line2D<int16_t> line;
	
	TEST_ASSERT_EQUALS(line.getPoint(), xpcc::Point2D<int16_t>(0, 0));
	TEST_ASSERT_EQUALS(line.getDirectionVector(), xpcc::Vector2D<int16_t>(0, 0));
}

void
Line2DTest::testConstructor()
{
	xpcc::Line2D<int16_t> line(
			xpcc::Point2D<int16_t>(-20, -60),
			xpcc::Vector2D<int16_t>(90, 80));
	
	TEST_ASSERT_EQUALS(line.getPoint(), xpcc::Point2D<int16_t>(-20, -60));
	TEST_ASSERT_EQUALS(line.getDirectionVector(), xpcc::Vector2D<int16_t>(90, 80));
}

void
Line2DTest::testAccessors()
{
	xpcc::Line2D<int16_t> line;
	
	line.setPoint(xpcc::Point2D<int16_t>(40, 780));
	line.setDirectionVector(xpcc::Vector2D<int16_t>(-50, 16));
	
	TEST_ASSERT_EQUALS(line.getPoint(), xpcc::Point2D<int16_t>(40, 780));
	TEST_ASSERT_EQUALS(line.getDirectionVector(), xpcc::Vector2D<int16_t>(-50, 16));
}

void
Line2DTest::testDistanceToPoint()
{
	xpcc::Line2D<int16_t> line(
			xpcc::Point2D<int16_t>(-20, -60),
			xpcc::Vector2D<int16_t>(10, 10));
	
	int16_t distance = line.getDistanceTo(xpcc::Point2D<int16_t>(10, 30));
	
	// closest point is (40, 0) => sqrt(30^2 + 30^2) = 42.4264..
	TEST_ASSERT_EQUALS(distance, 42);
	
	TEST_ASSERT_EQUALS(line.getPoint(), xpcc::Point2D<int16_t>(-20, -60));
	TEST_ASSERT_EQUALS(line.getDirectionVector(), xpcc::Vector2D<int16_t>(10, 10));
	
	int16_t distance2 = line.getDistanceTo(xpcc::Point2D<int16_t>(-50, -50));
	
	// closest point is (-30, -70) => sqrt(20^2 + 20^2) = 28.28427..
	TEST_ASSERT_EQUALS(distance2, 28);
	
	int16_t distance3 = line.getDistanceTo(xpcc::Point2D<int16_t>(70, -70));
	
	// closest point is (20, -20) => sqrt(50^2 + 50^2) = 70.7106..
	TEST_ASSERT_EQUALS(distance3, 71);
}
