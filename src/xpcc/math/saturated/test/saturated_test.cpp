/*
 * Copyright (c) 2009, Martin Rosekeit
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

#include <xpcc/math/saturated/saturated.hpp>

#include "saturated_test.hpp"

void
SaturatedTest::testSigned8bit()
{
	//TEST_FAIL("TODO");
	
}
	
void
SaturatedTest::testUnsigned8bit()
{
	xpcc::Saturated<uint8_t> x;
	xpcc::Saturated<uint8_t> y(100);
	
	TEST_ASSERT_EQUALS(x.getValue(), 0);
	TEST_ASSERT_EQUALS(y.getValue(), 100);
	
	x = 200;
	
	TEST_ASSERT_EQUALS(x.getValue(), 200);
	
	x += y;
	
	TEST_ASSERT_EQUALS(x.getValue(), 255);
	
	x = 10;
	y = 20;
	x -= y;
	
	TEST_ASSERT_EQUALS(x.getValue(), 0);
	
	xpcc::Saturated<uint8_t> z;
	
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
	
	TEST_ASSERT_EQUALS(x.getValue(), 0);
	
	y = 200;
	//x = abs(y);
	x = y;
	x.absolute();
	
	TEST_ASSERT_EQUALS(x.getValue(), 200);
}
