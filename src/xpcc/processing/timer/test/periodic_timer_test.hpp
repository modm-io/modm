// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include <unittest/testsuite.hpp>

class PeriodicTimerTest : public unittest::TestSuite
{
public:
	virtual void
	setUp();


	void
	testConstructor();

	void
	testTimer();

	void
	testRestart();
};
