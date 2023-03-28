/*
 * Copyright (c) 2023, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "i2c_platform_test.hpp"

#include <modm/platform.hpp>
#include <modm/board.hpp>
#include <modm/driver/storage/at24mac402.hpp>

using namespace modm::platform;
using namespace Board;

namespace
{
	modm::At24Mac402<I2c> eeprom{0x57};
}

void
I2cPlatformTest::setUp()
{
	I2c::connect<Sda::Twd, Scl::Twck>();
	I2c::initialize<SystemClock, 400_kHz>();
}

void
I2cPlatformTest::testPing()
{
	// ping at wrong address
	for (uint8_t address = 0x50; address <= 0x56; ++address) {
		eeprom.setAddress(address);
		const bool response = RF_CALL_BLOCKING(eeprom.ping());
		TEST_ASSERT_FALSE(response);
	}
	// set correct address 0x57
	eeprom.setAddress(0x57);
	// ping at correct address
	for (int i = 0; i < 20; ++i) {
		const bool response = RF_CALL_BLOCKING(eeprom.ping());
		TEST_ASSERT_TRUE(response);
	}
}

void
I2cPlatformTest::testDataRead()
{
	std::array<uint8_t, 6> buffer{};

	// read pre-programmed MAC address

	// read at wrong address
	eeprom.setAddress(0x55);
	bool readSuccess = RF_CALL_BLOCKING(eeprom.readMac(buffer));
	TEST_ASSERT_FALSE(readSuccess);

	// read at correct address
	eeprom.setAddress(0x57);
	readSuccess = RF_CALL_BLOCKING(eeprom.readMac(buffer));
	TEST_ASSERT_TRUE(readSuccess);

	TEST_ASSERT_EQUALS(buffer[0], 0xfc);
	TEST_ASSERT_EQUALS(buffer[1], 0xc2);
	TEST_ASSERT_EQUALS(buffer[2], 0x3d);
}
