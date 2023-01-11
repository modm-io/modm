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
Ms5837<I2cMaster>::Ms5837(DataBase &data, uint8_t address) : I2cDevice<I2cMaster, 2>(address), data(data)
{
}

// -----------------------------------------------------------------------------

template < typename I2cMaster >
modm::ResumableResult<bool>
Ms5837<I2cMaster>::initialize()
{
	RF_BEGIN();

	// Reset sensor once after power-on to load calibration PROM into the internal registers
	buffer[0] = i(Command::Reset);
	this->transaction.configureWrite(buffer, 1);
	if (!RF_CALL( this->runTransaction() ))
	{
		RF_RETURN(false);
	}

	// 4 ms reload (?: from MS5611)
	timeout.restart(std::chrono::milliseconds(4));
	RF_WAIT_UNTIL(timeout.isExpired());

	// Read the factory calibration from PROM
	data.prom.data[0] = RF_CALL(readProm(0));
	data.prom.data[1] = RF_CALL(readProm(1));
	data.prom.data[2] = RF_CALL(readProm(2));
	data.prom.data[3] = RF_CALL(readProm(3));
	data.prom.data[4] = RF_CALL(readProm(4));
	data.prom.data[5] = RF_CALL(readProm(5));
	data.prom.data[6] = RF_CALL(readProm(6));

	factory_crc = (data.prom.data[0] >> 12) & 0xF;

	RF_END_RETURN(factory_crc == data.prom.calculateCrc());
}

// -----------------------------------------------------------------------------

template < typename I2cMaster >
modm::ResumableResult<bool>
Ms5837<I2cMaster>::readout(OversamplingRatio osrPressure, OversamplingRatio osrTemperature)
{
	RF_BEGIN();

	// start a pressure conversion
	buffer[0] = i(Command::Convert) | i(Conversion::Pressure) | i(osrPressure);
	this->transaction.configureWrite(buffer, 1);
	if (!RF_CALL( this->runTransaction() )){
		RF_RETURN(false);
	}

	// Wait until pressure conversion has finished
	timeout.restart(std::chrono::milliseconds(conversionDelay[i(osrPressure) >> 1]));
	RF_WAIT_UNTIL(timeout.isExpired());

	// Get the pressure conversion result from sensor
	buffer[0] = i(Command::AdcRead);

	this->transaction.configureWriteRead(buffer, 1, data.raw, 3);
	if (!RF_CALL( this->runTransaction() )){
		RF_RETURN(false);
	}

	// Notify data class about changed buffer
	data.rawPressureTouched();


	// start a temperature conversion
	buffer[0] = i(Command::Convert) | i(Conversion::Temperature) | i(osrTemperature);
	this->transaction.configureWrite(buffer, 1);
	if (!RF_CALL( this->runTransaction() )){
		RF_RETURN(false);
	}


	// Wait until temperature conversion has finished
	timeout.restart(std::chrono::milliseconds(conversionDelay[i(osrTemperature) >> 1]));
	RF_WAIT_UNTIL(timeout.isExpired());

	// Get the temperature conversion result from sensor
	buffer[0] = i(Command::AdcRead);

	this->transaction.configureWriteRead(buffer, 1, data.raw + 3, 3);
	if (!RF_CALL( this->runTransaction() )){
		RF_RETURN(false);
	}

	// Notify data class about changed buffer
	data.rawTemperatureTouched();

	RF_END_RETURN(true);
}

// -----------------------------------------------------------------------------

template < typename I2cMaster >
modm::ResumableResult<uint16_t>
Ms5837<I2cMaster>::readProm(uint8_t address)
{
	RF_BEGIN();

	// MODM_LOG_DEBUG.printf("MS5837 readProm(%02x)\n", address);

	buffer[0] = i(Command::PromRead) | ((address & 0b111) << 1);
	this->transaction.configureWriteRead(buffer, 1, buffer, 2);
	if (! RF_CALL( this->runTransaction() ))
	{
		// MODM_LOG_DEBUG.printf("MS5837 readProm(%02x) Failed\n", address);
		RF_RETURN(0);
	}

	// MODM_LOG_DEBUG.printf("MS5837 readProm(%02x) Success: %d (%04x)\n", address, (buffer[0] << 8) | buffer[1], (buffer[0] << 8) | buffer[1]);
	RF_END_RETURN(static_cast<uint16_t>((buffer[0] << 8) | buffer[1]));
}

} // modm namespace
