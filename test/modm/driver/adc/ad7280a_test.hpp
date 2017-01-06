/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2010, 2012, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef AD7280A_TEST_HPP
#define AD7280A_TEST_HPP

#include <unittest/testsuite.hpp>

class Ad7280aTest : public unittest::TestSuite
{
public:
	void
	testCrcByte();
	
	void
	testCrcMessage();
	
	void
	testChainSetup();
	
	void
	testSelftest();
	
	void
	testSoftwareReset();
	
	void
	testChannelRead();
	
	void
	testAllChannelRead();
	
	void
	testBalancer();
};


#endif // AD7280A_TEST_HPP
