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

#ifndef SPI_DEVICE_TEST_HPP
#define SPI_DEVICE_TEST_HPP

#include <unittest/testsuite.hpp>

class SpiDeviceTest : public unittest::TestSuite
{
public:
	void
	testInitialState();
	
	void
	testEmptyTransmission();
	
	void
	testSingleTransmission();
	
	void
	testMultipleTransmissions();
};


#endif // SPI_DEVICE_TEST_HPP
