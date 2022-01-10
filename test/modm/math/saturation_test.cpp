/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2022, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/math/saturated.hpp>

#include "saturation_test.hpp"
#include <cmath>

void
SaturationTest::testSaturated_int8_t()
{
	modm::Saturated<int8_t> x;
	TEST_ASSERT_EQUALS(x, 0);

	modm::Saturated<int8_t> y(100);
	TEST_ASSERT_EQUALS(y, 100);

	x = 110;

	TEST_ASSERT_EQUALS(x, 110);

	x += y;

	TEST_ASSERT_EQUALS(x, 127);

	x = -100;
	y = 50;
	x -= y;

	TEST_ASSERT_EQUALS(x, -128);

	modm::Saturated<uint8_t> z;

	x = 20;
	y = 10;

	z = x + y;

	TEST_ASSERT_EQUALS(x, 20);
	TEST_ASSERT_EQUALS(y, 10);
	TEST_ASSERT_EQUALS(z, 30);

	z = x - y;

	TEST_ASSERT_EQUALS(x, 20);
	TEST_ASSERT_EQUALS(y, 10);
	TEST_ASSERT_EQUALS(z, 10);

	y = z - x;

	TEST_ASSERT_EQUALS(x, 20);
	TEST_ASSERT_EQUALS(y, 0);
	TEST_ASSERT_EQUALS(z, 10);

	x = -z;

	TEST_ASSERT_EQUALS(x, -10);

	y = -100;
	x = abs(y);
	TEST_ASSERT_EQUALS(x, 100);

	x = y;
	x.absolute();
	TEST_ASSERT_EQUALS(x, 100);
}

void
SaturationTest::testSaturated_uint8_t()
{
	modm::Saturated<uint8_t> x;
	TEST_ASSERT_EQUALS(x, 0U);

	modm::Saturated<uint8_t> y(100);
	TEST_ASSERT_EQUALS(y, 100U);

	x = 200;

	TEST_ASSERT_EQUALS(x, 200U);

	x += y;

	TEST_ASSERT_EQUALS(x, 255U);

	x = 10;
	y = 20;
	x -= y;

	TEST_ASSERT_EQUALS(x, 0U);

	x = 100;
	y = 3;
	x *= y;

	TEST_ASSERT_EQUALS(x, 255U);

 	modm::Saturated<uint8_t> z;

	x = 20;
	y = 10;

	z = x + y;

	TEST_ASSERT_EQUALS(x, 20U);
	TEST_ASSERT_EQUALS(y, 10U);
	TEST_ASSERT_EQUALS(z, 30U);

	z = x - y;

	TEST_ASSERT_EQUALS(x, 20U);
	TEST_ASSERT_EQUALS(y, 10U);
	TEST_ASSERT_EQUALS(z, 10U);

	y = z - x;

	TEST_ASSERT_EQUALS(x, 20U);
	TEST_ASSERT_EQUALS(y, 0U);
	TEST_ASSERT_EQUALS(z, 10U);

	x = -z;

	TEST_ASSERT_EQUALS(x, 0U);

	y = 200;
	x = abs(y);
	TEST_ASSERT_EQUALS(x, 200U);

	x = y;
	x.absolute();
	TEST_ASSERT_EQUALS(x, 200U);
}

void
SaturationTest::testSaturated_int16_t()
{
	modm::Saturated<int16_t> x;
	TEST_ASSERT_EQUALS(x, 0);

	modm::Saturated<int16_t> y(30000);
	TEST_ASSERT_EQUALS(y, 30000);

	x = 20000;
	TEST_ASSERT_EQUALS(x, 20000);

	x += y;
	TEST_ASSERT_EQUALS(x, 32767);

	x = 1000;
	y = 2000;
	x -= y;
	TEST_ASSERT_EQUALS(x, -1000);

 	modm::Saturated<int16_t> z;

	x = 10000;
	y = 20000;
	z = x + y;
	TEST_ASSERT_EQUALS(x, 10000);
	TEST_ASSERT_EQUALS(y, 20000);
	TEST_ASSERT_EQUALS(z, 30000);

	z = x - y;
	TEST_ASSERT_EQUALS(x, 10000);
	TEST_ASSERT_EQUALS(y, 20000);
	TEST_ASSERT_EQUALS(z, -10000);

	y = z - x;
	TEST_ASSERT_EQUALS(x, 10000);
	TEST_ASSERT_EQUALS(y, -20000);
	TEST_ASSERT_EQUALS(z, -10000);

	x = -z;
	TEST_ASSERT_EQUALS(x, 10000);

	y = -20000;
	x = abs(y);
	TEST_ASSERT_EQUALS(x, 20000);

	x = y;
	x.absolute();
	TEST_ASSERT_EQUALS(x, 20000);
}

void
SaturationTest::testSaturated_uint16_t()
{
	modm::Saturated<uint16_t> x;
	TEST_ASSERT_EQUALS(x, 0U);

	modm::Saturated<uint16_t> y(30000);
	TEST_ASSERT_EQUALS(y, 30000U);

	x = 40000;
	TEST_ASSERT_EQUALS(x, 40000U);

	x += y;
	TEST_ASSERT_EQUALS(x, 65535U);

	x = 1000;
	y = 2000;
	x -= y;
	TEST_ASSERT_EQUALS(x, 0U);

	x = 20000;
	x *= 5;
	TEST_ASSERT_EQUALS(x, 65535U);

 	modm::Saturated<uint16_t> z;

	x = 20000;
	y = 10000;
	z = x + y;
	TEST_ASSERT_EQUALS(x, 20000U);
	TEST_ASSERT_EQUALS(y, 10000U);
	TEST_ASSERT_EQUALS(z, 30000U);

	z = x - y;
	TEST_ASSERT_EQUALS(x, 20000U);
	TEST_ASSERT_EQUALS(y, 10000U);
	TEST_ASSERT_EQUALS(z, 10000U);

	y = z - x;
	TEST_ASSERT_EQUALS(x, 20000U);
	TEST_ASSERT_EQUALS(y, 0U);
	TEST_ASSERT_EQUALS(z, 10000U);

	x = -z;
	TEST_ASSERT_EQUALS(x, 0U);

	y = 20000;
	x = abs(y);
	TEST_ASSERT_EQUALS(x, 20000U);

	x = y;
	x.absolute();
	TEST_ASSERT_EQUALS(x, 20000U);
}

void
SaturationTest::testSaturated_uint8_t_ref() {
	uint8_t x = 100;

	modm::Saturated<uint8_t&> xRef = x;
	TEST_ASSERT_EQUALS(xRef, 100U);

	x += 10;
	TEST_ASSERT_EQUALS(xRef, 110U);

	xRef += 200;
	TEST_ASSERT_EQUALS(x, 255U);

	xRef = 30000U;
	TEST_ASSERT_EQUALS(x, 255U);

	x = 20;
	xRef -= 200;
	TEST_ASSERT_EQUALS(x, 0U);

	x = 42;
	uint8_t y = xRef;
	TEST_ASSERT_EQUALS(y, 42U);

	x = 20;
	y = xRef - 100;
	TEST_ASSERT_EQUALS(y, 0U);
	TEST_ASSERT_EQUALS(x, 20U);

	y = xRef + 3000;
	TEST_ASSERT_EQUALS(y, 255U);
	TEST_ASSERT_EQUALS(x, 20U);
}