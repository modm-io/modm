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

#include <modm/math/interpolation/linear.hpp>

#include "linear_interpolation_test.hpp"

void
LinearInterpolationTest::testInterpolationRam()
{
	typedef modm::Pair<int16_t, uint16_t> Point;
	
	Point points[3] =
	{
		{ -10, 50 },
		{  50, 10 },
		{ 100,  0 }
	};
	
	modm::interpolation::Linear<Point> value(points, 3);
	
	TEST_ASSERT_EQUALS(value.interpolate(-20), 50U);
	TEST_ASSERT_EQUALS(value.interpolate( 10), 37U);
	TEST_ASSERT_EQUALS(value.interpolate( 30), 24U);
	TEST_ASSERT_EQUALS(value.interpolate( 50), 10U);
	TEST_ASSERT_EQUALS(value.interpolate(150),  0U);
}

typedef modm::Pair<uint8_t, int16_t> MyPair;

FLASH_STORAGE(MyPair flashValues[6]) =
{
	{ 30, -200 },
	{ 50, 0 },
	{ 90, 50 },
	{ 150, 2050 },
	{ 200, 3000 },
	{ 220, 20000 }
};

void 
LinearInterpolationTest::testInterpolationFlash()
{
	modm::interpolation::Linear<MyPair, modm::accessor::Flash> \
		value(modm::accessor::asFlash(flashValues), 6);
	
	TEST_ASSERT_EQUALS(value.interpolate(  0),  -200);
	TEST_ASSERT_EQUALS(value.interpolate( 10),  -200);
	TEST_ASSERT_EQUALS(value.interpolate( 30),  -200);
	TEST_ASSERT_EQUALS(value.interpolate( 32),  -180);
	TEST_ASSERT_EQUALS(value.interpolate( 40),  -100);
	TEST_ASSERT_EQUALS(value.interpolate( 90),    50);
	TEST_ASSERT_EQUALS(value.interpolate(100),   383);
	TEST_ASSERT_EQUALS(value.interpolate(110),   716);
	TEST_ASSERT_EQUALS(value.interpolate(120),  1050);
	TEST_ASSERT_EQUALS(value.interpolate(130),  1383);
	TEST_ASSERT_EQUALS(value.interpolate(140),  1716);
	TEST_ASSERT_EQUALS(value.interpolate(150),  2050);
	TEST_ASSERT_EQUALS(value.interpolate(200),  3000);
	TEST_ASSERT_EQUALS(value.interpolate(201),  3850);
	TEST_ASSERT_EQUALS(value.interpolate(203),  5550);
	TEST_ASSERT_EQUALS(value.interpolate(219), 19150);
	TEST_ASSERT_EQUALS(value.interpolate(220), 20000);
	TEST_ASSERT_EQUALS(value.interpolate(230), 20000);
	TEST_ASSERT_EQUALS(value.interpolate(250), 20000);
}
