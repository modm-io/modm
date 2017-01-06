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

#include <unittest/testsuite.hpp>

class BitOperationTest : public unittest::TestSuite
{
public:
	void
	testSwap();
	
	void
	testReverse8bit();
	
	void
	testReverse16bit();
	
	void
	testReverse32bit();
	
	void
	testCount8bit();
	
	void
	testCount16bit();
	
	void
	testCount32bit();
};
