// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_BMP085_HPP
#	error  "Don't include this file directly, use 'bmp085.hpp' instead!"
#endif
#include <xpcc/math/utils/operator.hpp>
#include <xpcc/math/utils/bit_operation.hpp>

// ----------------------------------------------------------------------------
template < typename I2cMaster >
xpcc::Bmp085<I2cMaster>::Bmp085(Data &data, uint8_t address)
:	data(data),
 	i2cTask(I2cTask::Idle),
 	i2cSuccess(0),
 	adapter(address, i2cTask, i2cSuccess)
{
}

// ----------------------------------------------------------------------------
// MARK: - Tasks
template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Bmp085<I2cMaster>::ping(void *ctx)
{
	CO_BEGIN(ctx);

	CO_WAIT_UNTIL(adapter.configurePing() && this->startTransaction(&adapter));

	i2cTask = I2cTask::Ping;

	CO_WAIT_WHILE(i2cTask == I2cTask::Ping);

	if (i2cSuccess == I2cTask::Ping)
		CO_RETURN(true);

	CO_END();
}

template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Bmp085<I2cMaster>::configure(void *ctx, bmp085::Mode mode)
{
	data.config = mode;
	buffer[0] = bmp085::REGISTER_CAL_AC1;
	
	CO_BEGIN(ctx);

	CO_WAIT_UNTIL(
			!adapter.isBusy() && (
					adapter.configureWriteRead(
							buffer, 1,
							reinterpret_cast<uint8_t*>(&data.calibration), 22) &&
					this->startTransaction(&adapter)
			)
	);

	i2cTask = I2cTask::Configure;

	CO_WAIT_WHILE(i2cTask == I2cTask::Configure);

	if (i2cSuccess == I2cTask::Configure) {
		uint16_t* element = reinterpret_cast<uint16_t*>(&data.calibration);
		element[ 0] = xpcc::swap(element[0]);
		element[ 1] = xpcc::swap(element[1]);
		element[ 2] = xpcc::swap(element[2]);
		element[ 3] = xpcc::swap(element[3]);
		element[ 4] = xpcc::swap(element[4]);
		element[ 5] = xpcc::swap(element[5]);

		element[ 6] = xpcc::swap(element[6]);
		element[ 7] = xpcc::swap(element[7]);

		element[ 8] = xpcc::swap(element[8]);
		element[ 9] = xpcc::swap(element[9]);
		element[10] = xpcc::swap(element[10]);

		CO_RETURN(true);
	}

	CO_END_RETURN(false);
}

template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Bmp085<I2cMaster>::readout(void *ctx)
{
	CO_BEGIN(ctx);

	// Start temperature reading
	buffer[0] = bmp085::REGISTER_CONTROL;
	buffer[1] = bmp085::CONVERSION_TEMPERATURE;

	CO_WAIT_UNTIL(
			!adapter.isBusy() && (
					adapter.configureWrite(buffer, 2) &&
					this->startTransaction(&adapter)
			)
	);

	i2cTask = I2cTask::Readout;

	CO_WAIT_WHILE(i2cTask == I2cTask::Readout);

	if (i2cSuccess != I2cTask::Readout) {
		CO_RETURN(false);
	}

	// Wait until temperature reading is succeeded
	timeout.restart(5);
	CO_WAIT_UNTIL(timeout.isExpired());

	// Get the temperature from sensor
	buffer[0] = bmp085::REGISTER_CONVERSION_MSB;
	CO_WAIT_UNTIL(
			!adapter.isBusy() && (
					adapter.configureWriteRead(
							buffer, 1,
							data.raw, 2) &&
					this->startTransaction(&adapter)
			)
	);

	data.temperatureCalculated = false;

	i2cTask = I2cTask::Readout;

	CO_WAIT_WHILE(i2cTask == I2cTask::Readout);

	if (i2cSuccess != I2cTask::Readout) {
		CO_RETURN(false);
	}

	// Now start converting the pressure
	buffer[0] = bmp085::REGISTER_CONTROL;
	buffer[1] = bmp085::CONVERSION_PRESSURE | data.config;

	CO_WAIT_UNTIL(
			!adapter.isBusy() && (
					adapter.configureWrite(buffer, 2) &&
					this->startTransaction(&adapter)
			)
	);

	i2cTask = I2cTask::Readout;

	CO_WAIT_WHILE(i2cTask == I2cTask::Readout);

	if (i2cSuccess != I2cTask::Readout) {
		CO_RETURN(false);
	}

	// Wait until sensor has converted the pressure
	{
		uint8_t oss = ((data.config & bmp085::MODE) >> 6);
		timeout.restart(conversionDelay[oss]);
	}
	CO_WAIT_UNTIL(timeout.isExpired());

	// Get the pressure from sensor
	buffer[0] = bmp085::REGISTER_CONVERSION_MSB;
	CO_WAIT_UNTIL(
			!adapter.isBusy() && (
					adapter.configureWriteRead(
							buffer, 1,
							data.raw + 2, 3) &&
					this->startTransaction(&adapter)
			)
	);

	data.pressureCalculated = false;

	i2cTask = I2cTask::Readout;

	CO_WAIT_WHILE(i2cTask == I2cTask::Readout);

	if (i2cSuccess != I2cTask::Readout) {
		CO_RETURN(false);
	}

	CO_END_RETURN(true);
}

void
xpcc::bmp085::Data::calculateCalibratedTemperature()
{
	int32_t x1, x2;
	uint16_t ut = static_cast<uint16_t>(raw[0] << 8 | raw[1]);
	x1 = xpcc::math::mul(ut - calibration.ac6, calibration.ac5) >> 15;
	x2 = (static_cast<int32_t>(calibration.mc) << 11) / (x1 + calibration.md);
	b5 = x1 + x2;
	calibratedTemperature = static_cast<int16_t>((b5 + 8) >> 4);
	temperatureCalculated = true;
}

void
xpcc::bmp085::Data::calculateCalibratedPressure()
{
	int32_t x1, x2, x3, b3, p;
	int16_t b6;
	uint32_t b4, b7;
	uint8_t oss = ((config & bmp085::MODE) >> 6);

	calculateCalibratedTemperature();

	uint32_t up = (static_cast<uint32_t>(raw[2])<<16|static_cast<uint16_t>(raw[3])<<8|raw[4]) >> (8 - oss);
	b6 = static_cast<int16_t>(b5 - 4000);
	x1 = xpcc::math::mul(calibration.b2, xpcc::math::mul(b6, b6) >> 12) >> 11;
	x2 = xpcc::math::mul(calibration.ac2, b6) >> 11;
	x3 = x1 + x2;
	b3 = (xpcc::math::mul(calibration.ac1, 4) + x3) << oss;
	b3 = (b3 + 2) >> 2;
	x1 = xpcc::math::mul(calibration.ac3, b6) >> 13;
	x2 = xpcc::math::mul(calibration.b1, xpcc::math::mul(b6, b6) >> 12) >> 16;
	x3 = ((x1 + x2) + 2) >> 2;
	b4 = xpcc::math::mul(calibration.ac4, static_cast<uint16_t>(x3 + 32768)) >> 15;
	b7 = (up - b3) * (50000 >> oss);
	if (b7 < 0x80000000)
		p = (b7 << 1) / b4;
	else
		p = (b7 / b4) << 1;

	x1 = xpcc::math::mul(static_cast<uint16_t>(p >> 8), static_cast<uint16_t>(p >> 8));
	x1 = (x1 * 3038) >> 16;
	x2 = (-7357 * p) >> 16;
	calibratedPressure = p + ((x1 + x2 + 3791) >> 4);
	pressureCalculated = false;
}

template < typename I2cMaster >
constexpr uint8_t
xpcc::Bmp085< I2cMaster >::conversionDelay[];
