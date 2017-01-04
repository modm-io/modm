/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/container/pair.hpp>
#include <modm/architecture/driver/accessor/flash.hpp>

#include "pair_test.hpp"

// check if Pair could be stored inside the Flash memory
typedef xpcc::Pair<uint8_t, int16_t> myPair;

FLASH_STORAGE(myPair values[]) = {
	{ 1, 3 },
	{ 5, 30 },
	{ 27, 100 }
};

void
PairTest::testPair()
{
	xpcc::accessor::Flash<myPair> ptr(values);
	
	TEST_ASSERT_EQUALS(ptr[1].getFirst(), 5);
	TEST_ASSERT_EQUALS(ptr[1].getSecond(), 30);
	
	TEST_ASSERT_EQUALS(ptr[2].getFirst(), 27);
	TEST_ASSERT_EQUALS(ptr[2].getSecond(), 100);
	
	xpcc::Pair<int16_t, int16_t> pair = { 124, -1523 };
	
	TEST_ASSERT_EQUALS(pair.getFirst(), 124);
	TEST_ASSERT_EQUALS(pair.getSecond(), -1523);
}
