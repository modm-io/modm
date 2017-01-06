/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2011-2012, Niklas Hauser
 * Copyright (c) 2012, Georgi Grinshpun
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <unittest/testsuite.hpp>

class QuaternionTest : public unittest::TestSuite
{
public:
	void
	testConstructor();
	
	void
	testAssign();
	
	void
	testRawDataAccess();
	
	void
	testOperators();
	
	void
	testLength();
};
