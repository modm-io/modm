/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009, Thorsten Lajewski
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2015, Kevin Laeufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/math/geometry/point_set_2d.hpp>

#include "point_set_2d_test.hpp"


void
PointSet2DTest::testConstructor()
{
	xpcc::PointSet2D<int16_t> set(5);
	
	TEST_ASSERT_EQUALS(set.getNumberOfPoints(), 0U);
}

void
PointSet2DTest::testInitializerListConstructor()
{
	xpcc::PointSet2D<int16_t> set
		{ xpcc::Vector2i(0, 0),   xpcc::Vector2i(10, 0),
		  xpcc::Vector2i(10, 10), xpcc::Vector2i(0, 10) };

	TEST_ASSERT_EQUALS(set.getNumberOfPoints(), 4U);
	TEST_ASSERT_EQUALS(set[0], xpcc::Vector2i( 0,  0));
	TEST_ASSERT_EQUALS(set[1], xpcc::Vector2i(10,  0));
	TEST_ASSERT_EQUALS(set[2], xpcc::Vector2i(10, 10));
	TEST_ASSERT_EQUALS(set[3], xpcc::Vector2i( 0, 10));
}

void
PointSet2DTest::testAppendAndAccess()
{
	xpcc::PointSet2D<int16_t> set(5);
	
	set.append(xpcc::Vector2i(10, 20));
	
	TEST_ASSERT_EQUALS(set.getNumberOfPoints(), 1U);
	TEST_ASSERT_EQUALS(set[0], xpcc::Vector2i(10, 20));
	
	set.append(xpcc::Vector2i(30, 40));
	
	TEST_ASSERT_EQUALS(set.getNumberOfPoints(), 2U);
	TEST_ASSERT_EQUALS(set[0], xpcc::Vector2i(10, 20));
	TEST_ASSERT_EQUALS(set[1], xpcc::Vector2i(30, 40));
	
	set[0] = xpcc::Vector2i(50, 60);
	
	TEST_ASSERT_EQUALS(set[0], xpcc::Vector2i(50, 60));
}

void
PointSet2DTest::testIterator()
{
	xpcc::PointSet2D<int16_t> set(3);
	set.append(xpcc::Vector2i(10, 20));
	set.append(xpcc::Vector2i(20, 30));
	set.append(xpcc::Vector2i(30, 40));
	
	xpcc::PointSet2D<int16_t>::const_iterator it;
	
	int count;
	for (it = set.begin(), count = 0; it != set.end(); ++it, ++count) {
		TEST_ASSERT_EQUALS(set[count], (*it));
	}
	
	TEST_ASSERT_EQUALS(count, 3);
}
