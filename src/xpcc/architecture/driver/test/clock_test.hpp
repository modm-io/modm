// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
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

