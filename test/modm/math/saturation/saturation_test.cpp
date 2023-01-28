/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2021, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/math/saturation/saturated.hpp>

#include "saturation_test.hpp"
#include <cmath>

void
SaturationTest::testSigned8bit()
{
	modm::Saturated<int8_t> x;
	modm::Saturated<int8_t> y(100);

	TEST_ASSERT_EQUALS(x.getValue(), 0);
	TEST_ASSERT_EQUALS(y.getValue(), 100);

	x = 110;

	TEST_ASSERT_EQUALS(x.getValue(), 110);

	x += y;

	TEST_ASSERT_EQUALS(x.getValue(), 127);

	x = -100;
	y = 50;
	x -= y;

	TEST_ASSERT_EQUALS(x.getValue(), -128);

	modm::Saturated<uint8_t> z;

	x = 20;
	y = 10;

	z = x + y;

	TEST_ASSERT_EQUALS(x.getValue(), 20);
	TEST_ASSERT_EQUALS(y.getValue(), 10);
	TEST_ASSERT_EQUALS(z.getValue(), 30);

	z = x - y;

	TEST_ASSERT_EQUALS(x.getValue(), 20);
	TEST_ASSERT_EQUALS(y.getValue(), 10);
	TEST_ASSERT_EQUALS(z.getValue(), 10);

	y = z - x;

	TEST_ASSERT_EQUALS(x.getValue(), 20);
	TEST_ASSERT_EQUALS(y.getValue(), 0);
	TEST_ASSERT_EQUALS(z.getValue(), 10);

	x = -z;

	TEST_ASSERT_EQUALS(x.getValue(), -10);

	y = -100;
	x = abs(y);
	TEST_ASSERT_EQUALS(x.getValue(), 100);

	x = y;
	x.absolute();
	TEST_ASSERT_EQUALS(x.getValue(), 100);
}

void
SaturationTest::testUnsigned8bit()
{
	modm::Saturated<uint8_t> x;
	modm::Saturated<uint8_t> y(100);

	TEST_ASSERT_EQUALS(x.getValue(), 0U);
	TEST_ASSERT_EQUALS(y.getValue(), 100U);

	x = 200;

	TEST_ASSERT_EQUALS(x.getValue(), 200U);

	x += y;

	TEST_ASSERT_EQUALS(x.getValue(), 255U);

	x = 10;
	y = 20;
	x -= y;

	TEST_ASSERT_EQUALS(x.getValue(), 0U);

	x = 100;
	y = 3;
	x *= y;

	TEST_ASSERT_EQUALS(x.getValue(), 255U);

 	modm::Saturated<uint8_t> z;

	x = 20;
	y = 10;

	z = x + y;

	TEST_ASSERT_EQUALS(x.getValue(), 20U);
	TEST_ASSERT_EQUALS(y.getValue(), 10U);
	TEST_ASSERT_EQUALS(z.getValue(), 30U);

	z = x - y;

	TEST_ASSERT_EQUALS(x.getValue(), 20U);
	TEST_ASSERT_EQUALS(y.getValue(), 10U);
	TEST_ASSERT_EQUALS(z.getValue(), 10U);

	y = z - x;

	TEST_ASSERT_EQUALS(x.getValue(), 20U);
	TEST_ASSERT_EQUALS(y.getValue(), 0U);
	TEST_ASSERT_EQUALS(z.getValue(), 10U);

	x = -z;

	TEST_ASSERT_EQUALS(x.getValue(), 0U);

	y = 200;
	x = abs(y);
	TEST_ASSERT_EQUALS(x.getValue(), 200U);

	x = y;
	x.absolute();
	TEST_ASSERT_EQUALS(x.getValue(), 200U);
}

void
SaturationTest::testSigned16bit()
{
	modm::Saturated<int16_t> x;
	modm::Saturated<int16_t> y(30000);

	TEST_ASSERT_EQUALS(x.getValue(), 0);
	TEST_ASSERT_EQUALS(y.getValue(), 30000);

	x = 20000;

	TEST_ASSERT_EQUALS(x.getValue(), 20000);

	x += y;

	TEST_ASSERT_EQUALS(x.getValue(), 32767);

	x = 1000;
	y = 2000;
	x -= y;

	TEST_ASSERT_EQUALS(x.getValue(), -1000);

 	modm::Saturated<int16_t> z;

	x = 10000;
	y = 20000;

	z = x + y;

	TEST_ASSERT_EQUALS(x.getValue(), 10000);
	TEST_ASSERT_EQUALS(y.getValue(), 20000);
	TEST_ASSERT_EQUALS(z.getValue(), 30000);

	z = x - y;

	TEST_ASSERT_EQUALS(x.getValue(), 10000);
	TEST_ASSERT_EQUALS(y.getValue(), 20000);
	TEST_ASSERT_EQUALS(z.getValue(), -10000);

	y = z - x;

	// ???
	TEST_ASSERT_EQUALS(x.getValue(), 10000);
	TEST_ASSERT_EQUALS(y.getValue(), -20000);
	TEST_ASSERT_EQUALS(z.getValue(), -10000);

	x = -z;

	TEST_ASSERT_EQUALS(x.getValue(), 10000);

	y = -20000;
	x = abs(y);
	TEST_ASSERT_EQUALS(x.getValue(), 20000);

	x = y;
	x.absolute();
	TEST_ASSERT_EQUALS(x.getValue(), 20000);
}

void
SaturationTest::testUnsigned16bit()
{
	modm::Saturated<uint16_t> x;
	modm::Saturated<uint16_t> y(30000);

	TEST_ASSERT_EQUALS(x.getValue(), 0U);
	TEST_ASSERT_EQUALS(y.getValue(), 30000U);

	x = 40000;

	TEST_ASSERT_EQUALS(x.getValue(), 40000U);

	x += y;

	TEST_ASSERT_EQUALS(x.getValue(), 65535U);

	x = 1000;
	y = 2000;
	x -= y;

	TEST_ASSERT_EQUALS(x.getValue(), 0U);

	x = 20000;
	x *= 5;

	TEST_ASSERT_EQUALS(x.getValue(), 65535U);

 	modm::Saturated<uint16_t> z;

	x = 20000;
	y = 10000;

	z = x + y;

	TEST_ASSERT_EQUALS(x.getValue(), 20000U);
	TEST_ASSERT_EQUALS(y.getValue(), 10000U);
	TEST_ASSERT_EQUALS(z.getValue(), 30000U);

	z = x - y;

	TEST_ASSERT_EQUALS(x.getValue(), 20000U);
	TEST_ASSERT_EQUALS(y.getValue(), 10000U);
	TEST_ASSERT_EQUALS(z.getValue(), 10000U);

	y = z - x;

	TEST_ASSERT_EQUALS(x.getValue(), 20000U);
	TEST_ASSERT_EQUALS(y.getValue(), 0U);
	TEST_ASSERT_EQUALS(z.getValue(), 10000U);

	x = -z;

	TEST_ASSERT_EQUALS(x.getValue(), 0U);

	y = 20000;
	x = abs(y);
	TEST_ASSERT_EQUALS(x.getValue(), 20000U);

	x = y;
	x.absolute();
	TEST_ASSERT_EQUALS(x.getValue(), 20000U);
}