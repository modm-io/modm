/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2010, 2012, Fabian Greif
 * Copyright (c) 2012, 2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef TIME_TEST_HPP
#define TIME_TEST_HPP

#include <unittest/testsuite.hpp>

class TimeTest : public unittest::TestSuite
{
public:
	void
	testConversionToUnixTime();
	
	void
	testConversionToUnixTime2();
	
	void
	testConversionToUnixTime3();
	
	void
	testConversionToUnixTime4();
	
	void
	testConversionToUnixTime5();
	
	
	void
	testConversionToDate();
	
	void
	testConversionToDate2();
	
	void
	testConversionToDate3();
	
	void
	testConversionToDate4();
	
	void
	testConversionToDate5();
};

#endif	// TIME_TEST_HPP
