/*
 * Copyright (c) 2012, Fabian Greif
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "spi_device_test.hpp"

#define ENABLE_MACRO_EXPORT
#include "spi_device.hpp"
#undef ENABLE_MACRO_EXPORT

// ----------------------------------------------------------------------------
void
SpiDeviceTest::testInitialState()
{
	test::SpiDevice device;
	
	TEST_ASSERT_TRUE(device.isSuccessful());
}

void
SpiDeviceTest::testEmptyTransmission()
{
	test::SpiDevice device;
	
	device.start(0, 0, __LINE__, false);
	
	TEST_ASSERT_TRUE(device.isSuccessful());
}

void
SpiDeviceTest::testSingleTransmission()
{
	uint8_t arg1Rx[] = {1, 2, 3, 4};
	uint8_t arg1Tx[] = {4, 3, 2, 1};

	test::Transmission transmissions[] = {
		test::Transmission(XPCC_ARRAY_SIZE(arg1Rx), arg1Rx, arg1Tx),
	};
	
	test::SpiDevice device;
	device.start(transmissions, ARRAY_SIZE(transmissions), __LINE__, false);
	
	TEST_ASSERT_FALSE(device.isSuccessful());
	
	device.select();
	TEST_ASSERT_EQUALS(device.write(1), 4);
	TEST_ASSERT_EQUALS(device.write(2), 3);
	TEST_ASSERT_EQUALS(device.write(3), 2);
	TEST_ASSERT_EQUALS(device.write(4), 1);
	device.deselect();
	
	TEST_ASSERT_TRUE(device.isSuccessful());
	
	device.select();
	device.deselect();
	
	TEST_ASSERT_FALSE(device.isSuccessful());
	
	// Restart transmission
	device.start(transmissions, ARRAY_SIZE(transmissions), __LINE__, false);
	device.select();
	TEST_ASSERT_EQUALS(device.write(1), 4);
	TEST_ASSERT_EQUALS(device.write(2), 3);
	TEST_ASSERT_EQUALS(device.write(3), 2);
	TEST_ASSERT_EQUALS(device.write(4), 1);
	TEST_ASSERT_EQUALS(device.write(5), 0xff);		// one byte to much
	device.deselect();
	
	TEST_ASSERT_FALSE(device.isSuccessful());
}

void
SpiDeviceTest::testMultipleTransmissions()
{
	uint8_t arg1Rx[] = {1, 2, 3, 4};
	uint8_t arg1Tx[] = {4, 3, 2, 1};

	uint8_t arg2Rx[] = {5, 6};
	uint8_t arg2Tx[] = {100, 101};

	test::Transmission transmissions[] = {
		test::Transmission(XPCC_ARRAY_SIZE(arg1Rx), arg1Rx, arg1Tx),
		test::Transmission(XPCC_ARRAY_SIZE(arg2Rx), arg2Rx, arg2Tx),
	};
	
	test::SpiDevice device;
	device.start(transmissions, ARRAY_SIZE(transmissions), __LINE__, false);
	
	TEST_ASSERT_FALSE(device.isSuccessful());
	
	device.select();
	TEST_ASSERT_EQUALS(device.write(1), 4);
	TEST_ASSERT_EQUALS(device.write(2), 3);
	TEST_ASSERT_EQUALS(device.write(3), 2);
	TEST_ASSERT_EQUALS(device.write(4), 1);
	device.deselect();
	
	device.select();
	TEST_ASSERT_EQUALS(device.write(5), 100);
	TEST_ASSERT_EQUALS(device.write(6), 101);
	device.deselect();
	
	TEST_ASSERT_TRUE(device.isSuccessful());
}
