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
#include <modm/driver/storage/i2c_eeprom.hpp>
#include <array>

using namespace Board;
using namespace modm::literals;

// Generic I2C EEPROM driver example using on-board 2 kBit AT24MAC402 EEPROM
// For extended AT24MAC402 functionality see specific example and documentation.
int main()
{
	Board::initialize();

	MODM_LOG_INFO << "I2C EEPROM Test" << modm::endl;

	/* Defined in board support package:
	 * using Sda = GpioA3;
	 * using Scl = GpioA4;
	 * using I2c = I2cMaster0;
	 */
	I2c::connect<Sda::Twd, Scl::Twck>();
	I2c::initialize<SystemClock, 400_kHz>();

	// address 0x57 = 1 0 1 0 A0 A1 A2
	// with A0 = A1 = A2 = 1 connected to 3.3V
	// 8 bit address width
	constexpr auto AddressWidth = 1; // use 2 for 16 bit address EEPROMs
	modm::I2cEeprom<I2c, AddressWidth> eeprom{0x57};

	MODM_LOG_INFO << "EEPROM detected: " << RF_CALL_BLOCKING(eeprom.ping()) << "\n\n";

	// Write 4 data bytes to address 0x80
	/*constexpr std::array<uint8_t, 4> data = {0xAA, 0xBB, 0xCC, 0xDD};
	const bool writeSuccess = RF_CALL_BLOCKING(eeprom.write(0x80, data.data(), 4));
	MODM_LOG_INFO << "write successful: " << writeSuccess << "\n\n";*/

	// Read 4 data bytes from address 0x80
	std::array<uint8_t, 4> buffer{};
	bool readSuccess = false;
	// retry read until device responds after finishing previous write
	while (!readSuccess)
	{
		readSuccess = RF_CALL_BLOCKING(eeprom.read(0x80, buffer.data(), buffer.size()));
	}
	MODM_LOG_INFO << "read successful: " << readSuccess << "\n";
	MODM_LOG_INFO.printf("data: 0x%02x 0x%02x 0x%02x 0x%02x\n\n", buffer[0], buffer[1], buffer[2], buffer[3]);

	while (true)
	{
		Led0::toggle();
		Led1::toggle();
		modm::delay(500ms);
	}

	return 0;
}
