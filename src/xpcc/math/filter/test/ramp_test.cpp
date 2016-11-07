/*
 * Copyright (c) 2009-2010, Martin Rosekeit
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

#include <xpcc/math/filter/ramp.hpp>

#include "ramp_test.hpp"

void
RampTest::testConstructor()
{
	xpcc::filter::Ramp<int16_t> ramp(3, 4);
	
	TEST_ASSERT_TRUE(ramp.isTargetReached());
	TEST_ASSERT_EQUALS(ramp.getValue(), 0);
}

void
RampTest::testRamp()
{
	xpcc::filter::Ramp<int16_t> ramp(3, 4);
	
	ramp.update();
	TEST_ASSERT_EQUALS(ramp.getValue(), 0);
	
	ramp.setTarget(20);
	TEST_ASSERT_EQUALS(ramp.getValue(), 0);
	TEST_ASSERT_FALSE(ramp.isTargetReached());
	
	for (int i = 0; i < 6; ++i) {
		ramp.update();
		TEST_ASSERT_EQUALS(ramp.getValue(), i * 3 + 3);
	}
	TEST_ASSERT_FALSE(ramp.isTargetReached());
	
	ramp.update();
	TEST_ASSERT_EQUALS(ramp.getValue(), 20);
	TEST_ASSERT_TRUE(ramp.isTargetReached());
	
	ramp.update();
	TEST_ASSERT_EQUALS(ramp.getValue(), 20);
	TEST_ASSERT_TRUE(ramp.isTargetReached());
	
	ramp.setTarget(-50);
	TEST_ASSERT_EQUALS(ramp.getValue(), 20);
	TEST_ASSERT_FALSE(ramp.isTargetReached());
	
	for (int i = 0; i < 17; ++i) {
		ramp.update();
		TEST_ASSERT_EQUALS(ramp.getValue(), 20 - (i + 1) * 4);
	}
	TEST_ASSERT_FALSE(ramp.isTargetReached());
	
	ramp.update();
	TEST_ASSERT_EQUALS(ramp.getValue(), -50);
	TEST_ASSERT_TRUE(ramp.isTargetReached());
	
	ramp.update();
	TEST_ASSERT_EQUALS(ramp.getValue(), -50);
	TEST_ASSERT_TRUE(ramp.isTargetReached());
	
}
