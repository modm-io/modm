/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <unittest/testsuite.hpp>

class BoundedDequeTest : public unittest::TestSuite
{
public:
	void
	testForward();
	
	void
	testBackward();
	
	void
	testBoth();
	
	void
	testClear();
	
	void
	testConstIterator();

	// Test if append() fails when the queue is full
	void
	testFull();

	// Test if queue of length one is handled properly
	void
	testOneElementQueue();
	
	void
	testOverwrite();
	
	void
	testElementAccess();
};
