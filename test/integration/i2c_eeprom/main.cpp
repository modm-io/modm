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

#include <modm/board.hpp>
#include <modm/driver/storage/at24mac402.hpp>
#include <modm/driver/storage/i2c_eeprom.hpp>
#include <array>

#include "../integration_test.hpp"

using namespace Board;

/**
 * Tests the generic I2C EEPROM driver and the AT24MAC402 driver with the
 * 2 kBit AT24MAC402 EEPROM on the board: writes four bytes with one driver
 * and reads them back with both.
 */

// retry until the device responds after finishing the previous write
template< class Eeprom >
static bool
read(Eeprom &eeprom, std::array<uint8_t, 4> &buffer)
{
	for (uint8_t tries = 0; tries < 100; tries++)
	{
		if (eeprom.read(0x80, buffer.data(), buffer.size())) return true;
		modm::delay(1ms);
	}
	return false;
}

int main()
{
	Board::initialize();

	/* Defined in board support package:
	 * using Sda = GpioA3;
	 * using Scl = GpioA4;
	 * using I2c = I2cMaster0;
	 */
	I2c::connect<Sda::Twd, Scl::Twck>();
	I2c::initialize<SystemClock, 400_kHz>();

	// address 0x57 = 1 0 1 0 A0 A1 A2
	// with A0 = A1 = A2 = 1 connected to 3.3V
	modm::At24Mac402<I2c> eeprom{0x57};
	// 8 bit address width
	modm::I2cEeprom<I2c, 1> genericEeprom{0x57};

	bool passed = eeprom.ping();
	MODM_LOG_INFO << "EEPROM detected: " << passed << modm::endl;

	// Read pre-programmed MAC address
	std::array<uint8_t, 6> mac{};
	const bool macSuccess = eeprom.readMac(mac);
	MODM_LOG_INFO.printf("MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
						 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	// an unprogrammed address reads as all ones
	passed &= macSuccess and (mac != std::array<uint8_t, 6>{0xff, 0xff, 0xff, 0xff, 0xff, 0xff});

	// Alternate the data, so that we never read back what an earlier run wrote
	std::array<uint8_t, 4> buffer{};
	passed &= read(eeprom, buffer);
	const std::array<uint8_t, 4> data = (buffer[0] == 0xAA)
			? std::array<uint8_t, 4>{0x11, 0x22, 0x33, 0x44}
			: std::array<uint8_t, 4>{0xAA, 0xBB, 0xCC, 0xDD};

	// Write 4 data bytes to address 0x80
	passed &= eeprom.write(0x80, data.data(), data.size());

	// Read them back with both drivers
	passed &= read(eeprom, buffer) and (buffer == data);
	buffer.fill(0);
	passed &= read(genericEeprom, buffer) and (buffer == data);
	MODM_LOG_INFO.printf("data: 0x%02x 0x%02x 0x%02x 0x%02x\n",
						 buffer[0], buffer[1], buffer[2], buffer[3]);

	return finishTest(passed);
}
