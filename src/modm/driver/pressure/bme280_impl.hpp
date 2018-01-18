/*
 * Copyright (c) 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_BME280_HPP
#	error  "Don't include this file directly, use 'bme280.hpp' instead!"
#endif
#include <modm/math/utils/operator.hpp>
#include <modm/math/utils/bit_operation.hpp>
#include <modm/math/utils/endianness.hpp>

#undef  MODM_LOG_LEVEL
#define MODM_LOG_LEVEL modm::log::DISABLED

// ----------------------------------------------------------------------------
template < typename I2cMaster >
modm::Bme280<I2cMaster>::Bme280(Data &data, uint8_t address) :
	I2cDevice<I2cMaster, 1>(address), data(data)
{
}

// ----------------------------------------------------------------------------
// MARK: - Tasks
template < typename I2cMaster >
modm::ResumableResult<bool>
modm::Bme280<I2cMaster>::initialize(Mode mode, Oversampling pressure, Oversampling temperature, Oversampling humidity)
{
	RF_BEGIN();

	// Very first verify Chip Id
	// {
	// 	buffer[0] = i(Register::CHIP_ID);
	// 	uint8_t chid;
		
	// 	this->transaction.configureWriteRead(buffer, 1, &chid, 1);

	// 	if (RF_CALL( this->runTransaction() ))
	// 	{
	// 		MODM_LOG_DEBUG.printf("BME280 Chip Id check. Read %02x, expected %02x\n", chid, ChipId);
	// 		if (chid != ChipId) {
	// 			MODM_LOG_ERROR.printf("BME280 Chip Id mismatch. Read %02x, expected %02x\n", chid, ChipId);
	// 			RF_RETURN(false);
	// 		}
	// 	} else {
	// 		RF_RETURN(false);
	// 	}
	// }

	// First configure humidity sampling because 
	// Changes to this register only become effective after a write operation to “ctrl_meas”.
	{
		CtrlHum_t ctrl_hum = Humidity(humidity);

		buffer[0] = i(Register::CTRL_HUM);
		buffer[1] = ctrl_hum.value;
	}
	this->transaction.configureWrite(buffer, 2);
	if (not RF_CALL( this->runTransaction() )) {
		RF_RETURN(false);
	}

	
	// Now hardcoded to Normal mode (which may be inaccurate due to bus traffic)
	{
		CtrlMeas_t ctrl_meas = Mode_t(mode);
		ctrl_meas |= Pressure(pressure);
		ctrl_meas |= Temperature(temperature);

		buffer[0] = i(Register::CTRL_MEAS);
		buffer[1] = ctrl_meas.value;
	}

	this->transaction.configureWrite(buffer, 2);
	if (not RF_CALL( this->runTransaction() )) {
		RF_RETURN(false);
	}

	
	// Configure the standby time
	{
		Config_t config = TimeStandby_t(TimeStandby::S1);

		buffer[0] = i(Register::CONFIG);
		buffer[1] = config.value;
	}
	this->transaction.configureWrite(buffer, 2);
	if (not RF_CALL( this->runTransaction() )) {
		RF_RETURN(false);
	}


	// Read 26 bytes of Calib00 to Calib25
	buffer[0] = i(Register::CALIB00);

	this->transaction.configureWriteRead(buffer, 1, reinterpret_cast<uint8_t*>(&data.calibration), 26);

	if (RF_CALL( this->runTransaction() ))
	{
		{
			MODM_LOG_DEBUG << "Raw calibration data: ";
			uint8_t *rr = reinterpret_cast<uint8_t*>(&data.calibration);
			for (uint8_t ii = 0; ii < 26; ++ii) {
				MODM_LOG_DEBUG.printf("%x ", rr[ii]);
			}
			MODM_LOG_DEBUG << modm::endl;
		}

		uint16_t* element = reinterpret_cast<uint16_t*>(&data.calibration);
		element[ 0] = modm::fromLittleEndian(element[0]);
		element[ 1] = modm::fromLittleEndian(element[1]);
		element[ 2] = modm::fromLittleEndian(element[2]);
		element[ 3] = modm::fromLittleEndian(element[3]);
		element[ 4] = modm::fromLittleEndian(element[4]);
		element[ 5] = modm::fromLittleEndian(element[5]);

		element[ 6] = modm::fromLittleEndian(element[6]);
		element[ 7] = modm::fromLittleEndian(element[7]);

		element[ 8] = modm::fromLittleEndian(element[8]);
		element[ 9] = modm::fromLittleEndian(element[9]);
		element[10] = modm::fromLittleEndian(element[10]);
		element[11] = modm::fromLittleEndian(element[11]);
		element[12] = modm::fromLittleEndian(element[12]);
	} else {
		RF_RETURN(false);
	}

	// Read 7 bytes of Calib26 to Calib32
	// Calib33 to Calib41 are not used in the compensation formulas.
	// So they are not read here.

	// Reading E1 .. E7
	// 67 01 00 14 06 00 1E (hexadecimal) shall be converted to
	// H2=359, H3=0, H4=326, H5=0, H6=30 (decimal)

	buffer[0] = i(Register::CALIB26);

	this->transaction.configureWriteRead(buffer, 1, calBuffer, 7);

	if (RF_CALL( this->runTransaction() ))
	{
		data.calibration.H2 = (calBuffer[1] << 8) | calBuffer[0];
		data.calibration.H3 =  calBuffer[2];
		data.calibration.H4 = (uint16_t(calBuffer[3]) << 4) | (calBuffer[4] & 0x0f);
		data.calibration.H5 = (uint16_t(calBuffer[5]) << 4) | (calBuffer[4] >>   4);
		data.calibration.H6 =  calBuffer[6];

		RF_RETURN(true);
	}

	RF_END_RETURN(false);
}

template < typename I2cMaster >
modm::ResumableResult<bool>
modm::Bme280<I2cMaster>::readout()
{
	RF_BEGIN();

	// Get the raw data from sensor
	// It is advised by the datasheet to readout the complete sensor
	// data at once to avoid mixing old and new data.
	// And a single 8-byte transaction is even faster than multiple 3-byte
	// transactions.
	buffer[0] = i(Register::PRESS_MSB);
	this->transaction.configureWriteRead(buffer, 1, data.raw, 8);

	MODM_LOG_DEBUG.printf("RAW: %02x %02x %02x %02x %02x %02x %02x %02x\n",
		data.raw[0], data.raw[1], data.raw[2], data.raw[3],
		data.raw[4], data.raw[5], data.raw[6], data.raw[7]);

	// There will come new data, so new calculation necessary
	data.rawTemperatureTouched();
	data.rawPressureTouched();
	data.rawHumidityTouched();

	RF_END_RETURN_CALL( this->runTransaction() );
}
