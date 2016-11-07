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

#include <xpcc/math/geometry/location_2d.hpp>

#include "location_2d_test.hpp"

void
Location2DTest::testDefaultConstructor()
{
	xpcc::Location2D<int16_t> location;
	
	TEST_ASSERT_EQUALS(location.getPosition(), xpcc::Vector2i(0, 0));
	TEST_ASSERT_EQUALS_FLOAT(location.getOrientation(), 0.f);
}

void
Location2DTest::testConstructor()
{
	xpcc::Location2D<int16_t> location(
			xpcc::Vector2i(10, 20),
			M_PI);
	
	TEST_ASSERT_EQUALS(location.getPosition(), xpcc::Vector2i(10, 20));
	TEST_ASSERT_EQUALS_FLOAT(location.getOrientation(), M_PI);
}

void
Location2DTest::testAccessors()
{
	xpcc::Location2D<int16_t> location;
	
	location.setPosition(xpcc::Vector2i(30, 40));
	
	TEST_ASSERT_EQUALS(location.getPosition(), xpcc::Vector2i(30, 40));
	
	location.setPosition(50, 60);
	
	TEST_ASSERT_EQUALS(location.getPosition(), xpcc::Vector2i(50, 60));
	
	location.setOrientation(M_PI / 2);
	
	TEST_ASSERT_EQUALS_FLOAT(location.getOrientation(), M_PI / 2);
}

void
Location2DTest::testMove()
{
	xpcc::Location2D<int16_t> location(
			xpcc::Vector2i(-10, 20),
			M_PI);
	
	xpcc::Location2D<int16_t> movement(
			xpcc::Vector2i(10, 10),
			M_PI / 2);
	
	location.move(movement);
	
	TEST_ASSERT_EQUALS(location.getPosition(), xpcc::Vector2i(-20, 10));
	TEST_ASSERT_EQUALS_FLOAT(location.getOrientation(), -M_PI / 2);
	
	location.move(30, M_PI / 2);
	
	TEST_ASSERT_EQUALS(location.getPosition(), xpcc::Vector2i(-20, -20));
	TEST_ASSERT_EQUALS_FLOAT(location.getOrientation(), 0.f);
}

void
Location2DTest::testConvert()
{
	xpcc::Location2D<float> a(
			xpcc::Vector<float, 2>(-10.65, 20.31),
			M_PI);
	
	xpcc::Location2D<int16_t> b = a.convert<int16_t>();
	
	TEST_ASSERT_EQUALS(b.getX(), -11);
	TEST_ASSERT_EQUALS(b.getY(), 20);
}
