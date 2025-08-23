// coding: utf-8
// ----------------------------------------------------------------------------
/*
 * Copyright (c) 2023, Vivien Henry
 * Based on the ms5837, courtesy of Rasmus Kleist Hørlyck Sørensen
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_MS5837_HPP
#   error "Don't include this file directly, use 'ms5837.hpp' instead!"
#endif

#include <modm/math/utils/endianness.hpp>

namespace modm
{

template < typename I2cMaster >
Ms5837<I2cMaster>::Ms5837(DataBase &data, uint8_t address) : I2cDevice<I2cMaster>(address), data(data)
{
}

// -----------------------------------------------------------------------------

template < typename I2cMaster >
bool
Ms5837<I2cMaster>::initialize()
{
	// Reset sensor once after power-on to load calibration PROM into the internal registers
	buffer[0] = i(Command::Reset);
	if (!I2cDevice<I2cMaster>::write(buffer, 1))
	{
		return false;
	}

	// 4 ms reload (?: from MS5611)
	timeout.restart(std::chrono::milliseconds(4));
	modm::this_fiber::poll([&]{ return timeout.isExpired(); });

	// Read the factory calibration from PROM
	data.prom.data[0] = readProm(0);
	data.prom.data[1] = readProm(1);
	data.prom.data[2] = readProm(2);
	data.prom.data[3] = readProm(3);
	data.prom.data[4] = readProm(4);
	data.prom.data[5] = readProm(5);
	data.prom.data[6] = readProm(6);

	factory_crc = (data.prom.data[0] >> 12) & 0xF;

	return factory_crc == data.prom.calculateCrc();
}

// -----------------------------------------------------------------------------

template < typename I2cMaster >
bool
Ms5837<I2cMaster>::readout(OversamplingRatio osrPressure, OversamplingRatio osrTemperature)
{
	// start a pressure conversion
	buffer[0] = i(Command::Convert) | i(Conversion::Pressure) | i(osrPressure);
	if (!I2cDevice<I2cMaster>::write(buffer, 1)){
		return false;
	}

	// Wait until pressure conversion has finished
	timeout.restart(std::chrono::milliseconds(conversionDelay[i(osrPressure) >> 1]));
	modm::this_fiber::poll([&]{ return timeout.isExpired(); });

	// Get the pressure conversion result from sensor
	buffer[0] = i(Command::AdcRead);
	if (!I2cDevice<I2cMaster>::writeRead(buffer, 1, data.raw, 3)){
		return false;
	}

	// Notify data class about changed buffer
	data.rawPressureTouched();


	// start a temperature conversion
	buffer[0] = i(Command::Convert) | i(Conversion::Temperature) | i(osrTemperature);
	if (!I2cDevice<I2cMaster>::write(buffer, 1)){
		return false;
	}


	// Wait until temperature conversion has finished
	timeout.restart(std::chrono::milliseconds(conversionDelay[i(osrTemperature) >> 1]));
	modm::this_fiber::poll([&]{ return timeout.isExpired(); });

	// Get the temperature conversion result from sensor
	buffer[0] = i(Command::AdcRead);
	if (!I2cDevice<I2cMaster>::writeRead(buffer, 1, data.raw + 3, 3)){
		return false;
	}

	// Notify data class about changed buffer
	data.rawTemperatureTouched();

	return true;
}

// -----------------------------------------------------------------------------

template < typename I2cMaster >
uint16_t
Ms5837<I2cMaster>::readProm(uint8_t address)
{
	// MODM_LOG_DEBUG.printf("MS5837 readProm(%02x)\n", address);

	buffer[0] = i(Command::PromRead) | ((address & 0b111) << 1);
	if (! I2cDevice<I2cMaster>::writeRead(buffer, 1, buffer, 2))
	{
		// MODM_LOG_DEBUG.printf("MS5837 readProm(%02x) Failed\n", address);
		return 0;
	}

	// MODM_LOG_DEBUG.printf("MS5837 readProm(%02x) Success: %d (%04x)\n", address, (buffer[0] << 8) | buffer[1], (buffer[0] << 8) | buffer[1]);
	return static_cast<uint16_t>((buffer[0] << 8) | buffer[1]);
}

} // modm namespace
