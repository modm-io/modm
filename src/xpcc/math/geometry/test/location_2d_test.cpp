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

#include <xpcc/math/geometry/location_2d.hpp>

#include "location_2d_test.hpp"

void
Location2DTest::testDefaultConstructor()
{
	xpcc::Location2D<int16_t> location;
	
	TEST_ASSERT_EQUALS(location.getPosition(), xpcc::Point2D<int16_t>(0, 0));
	TEST_ASSERT_EQUALS_FLOAT(location.getOrientation(), 0.f);
}

void
Location2DTest::testConstructor()
{
	xpcc::Location2D<int16_t> location(
			xpcc::Point2D<int16_t>(10, 20),
			M_PI);
	
	TEST_ASSERT_EQUALS(location.getPosition(), xpcc::Point2D<int16_t>(10, 20));
	TEST_ASSERT_EQUALS_FLOAT(location.getOrientation(), M_PI);
}

void
Location2DTest::testAccessors()
{
	xpcc::Location2D<int16_t> location;
	
	location.setPosition(xpcc::Point2D<int16_t>(30, 40));
	
	TEST_ASSERT_EQUALS(location.getPosition(), xpcc::Point2D<int16_t>(30, 40));
	
	location.setPosition(50, 60);
	
	TEST_ASSERT_EQUALS(location.getPosition(), xpcc::Point2D<int16_t>(50, 60));
	
	location.setOrientation(M_PI / 2);
	
	TEST_ASSERT_EQUALS_FLOAT(location.getOrientation(), M_PI / 2);
}

void
Location2DTest::testMove()
{
	xpcc::Location2D<int16_t> location(
			xpcc::Point2D<int16_t>(-10, 20),
			M_PI);
	
	xpcc::Location2D<int16_t> movement(
			xpcc::Point2D<int16_t>(10, 10),
			M_PI / 2);
	
	location.move(movement);
	
	TEST_ASSERT_EQUALS(location.getPosition(), xpcc::Point2D<int16_t>(-20, 10));
	TEST_ASSERT_EQUALS_FLOAT(location.getOrientation(), -M_PI / 2);
	
	location.move(30, M_PI / 2);
	
	TEST_ASSERT_EQUALS(location.getPosition(), xpcc::Point2D<int16_t>(-20, -20));
	TEST_ASSERT_EQUALS_FLOAT(location.getOrientation(), 0.f);
}

void
Location2DTest::testConvert()
{
	
}
