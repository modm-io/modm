/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009, Thorsten Lajewski
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, 2015, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef CLOCK_TEST_HPP
#define CLOCK_TEST_HPP

#include <unittest/testsuite.hpp>

// The only purpose of the test is to ensure that XPCC__CLOCK_TESTMODE is
// set to 1 when running the other tests.
class ClockTest : public unittest::TestSuite
{
public:
	void
	testClock();
};

#endif

