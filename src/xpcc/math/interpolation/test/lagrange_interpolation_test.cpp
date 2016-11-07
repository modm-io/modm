/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009, Thorsten Lajewski
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <xpcc/math/interpolation/lagrange.hpp>

#include "lagrange_interpolation_test.hpp"

void
LagrangeInterpolationTest::testCreation()
{
	typedef xpcc::Pair<uint8_t, float> Point;
	
	Point points[3] =
	{
		{  10, -50 },
		{  50,   0 },
		{ 100,  50 }
	};
	
	xpcc::interpolation::Lagrange<Point> value(points, 3);
}

void 
LagrangeInterpolationTest::testInterpolation()
{
	typedef xpcc::Pair<float, float> Point;
	
	// interpolate x^2 over the range of 1 <= x <= 3
	Point points[3] =
	{
		{ 1, 1 },
		{ 2, 4 },
		{ 3, 9 }
	};
	
	xpcc::interpolation::Lagrange<Point> value(points, 3);
	
	TEST_ASSERT_EQUALS_FLOAT(value.interpolate(1.f),   1.f);
	TEST_ASSERT_EQUALS_FLOAT(value.interpolate(1.5f),  2.25f);
	TEST_ASSERT_EQUALS_FLOAT(value.interpolate(2.f),   4.f);
	TEST_ASSERT_EQUALS_FLOAT(value.interpolate(2.5f),  6.25f);
	TEST_ASSERT_EQUALS_FLOAT(value.interpolate(3.f),   9.f);
	TEST_ASSERT_EQUALS_FLOAT(value.interpolate(3.5f), 12.25f);
}


