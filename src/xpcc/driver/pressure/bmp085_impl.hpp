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

	CO_WAIT_UNTIL(
			adapter.configurePing() and
			(i2cTask = I2cTask::Ping, this->startTransaction(&adapter))
	);

	CO_WAIT_WHILE(i2cTask == I2cTask::Ping);

	CO_END_RETURN(i2cSuccess == I2cTask::Ping);
}

template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Bmp085<I2cMaster>::configure(void *ctx, Mode mode)
{
	CO_BEGIN(ctx);

	setMode(mode);
	buffer[0] = i(Register::CAL_AC1);

	CO_WAIT_UNTIL(
			adapter.configureWriteRead(buffer, 1, reinterpret_cast<uint8_t*>(&data.calibration), 22) and
			(i2cTask = I2cTask::ReadCalibration, this->startTransaction(&adapter))
	);

	CO_WAIT_WHILE(i2cTask == I2cTask::ReadCalibration);

	if (i2cSuccess == I2cTask::ReadCalibration) {
		uint16_t* element = reinterpret_cast<uint16_t*>(&data.calibration);
		element[ 0] = xpcc::fromBigEndian(element[0]);
		element[ 1] = xpcc::fromBigEndian(element[1]);
		element[ 2] = xpcc::fromBigEndian(element[2]);
		element[ 3] = xpcc::fromBigEndian(element[3]);
		element[ 4] = xpcc::fromBigEndian(element[4]);
		element[ 5] = xpcc::fromBigEndian(element[5]);

		element[ 6] = xpcc::fromBigEndian(element[6]);
		element[ 7] = xpcc::fromBigEndian(element[7]);

		element[ 8] = xpcc::fromBigEndian(element[8]);
		element[ 9] = xpcc::fromBigEndian(element[9]);
		element[10] = xpcc::fromBigEndian(element[10]);

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
	buffer[0] = i(Register::CONTROL);
	buffer[1] = i(Conversion::Temperature);

	CO_WAIT_UNTIL(
			adapter.configureWrite(buffer, 2) and
			(i2cTask = I2cTask::ConvertTemperature, this->startTransaction(&adapter))
	);

	CO_WAIT_WHILE(i2cTask == I2cTask::ConvertTemperature);

	if (i2cSuccess != I2cTask::ConvertTemperature) {
		CO_RETURN(false);
	}

	// Wait until temperature reading is succeeded
	timeout.restart(5);
	CO_WAIT_UNTIL(timeout.isExpired());

	// Get the temperature from sensor
	buffer[0] = i(Register::MSB);
	CO_WAIT_UNTIL(
			adapter.configureWriteRead(buffer, 1, data.raw, 2) and
			(i2cTask = I2cTask::ReadTemperature, this->startTransaction(&adapter))
	);

	data.meta &= ~data.TEMPERATURE_CALCULATED;

	CO_WAIT_WHILE(i2cTask == I2cTask::ReadTemperature);

	if (i2cSuccess != I2cTask::ReadTemperature) {
		CO_RETURN(false);
	}

	// buffer the mode for the timer later
	bufferedMode = data.meta & i(Mode::Mask);
	// Now start converting the pressure
	buffer[0] = i(Register::CONTROL);
	buffer[1] = i(Conversion::Pressure) | bufferedMode;

	CO_WAIT_UNTIL(
			adapter.configureWrite(buffer, 2) and
			(i2cTask = I2cTask::ConvertPressure, this->startTransaction(&adapter))
	);

	CO_WAIT_WHILE(i2cTask == I2cTask::ConvertPressure);

	if (i2cSuccess != I2cTask::ConvertPressure) {
		CO_RETURN(false);
	}

	// Wait until sensor has converted the pressure
	timeout.restart(conversionDelay[bufferedMode >> 6]);
	CO_WAIT_UNTIL(timeout.isExpired());

	// Get the pressure from sensor
	buffer[0] = i(Register::MSB);
	CO_WAIT_UNTIL(
			adapter.configureWriteRead(buffer, 1, data.raw + 2, 3) and
			(i2cTask = I2cTask::ReadPressure, this->startTransaction(&adapter))
	);

	data.meta &= ~data.PRESSURE_CALCULATED;

	CO_WAIT_WHILE(i2cTask == I2cTask::ReadPressure);

	CO_END_RETURN(i2cSuccess == I2cTask::ReadPressure);
}

void
xpcc::bmp085::Data::calculateCalibratedTemperature()
{
	int32_t x1, x2;
	uint16_t ut = (uint16_t(raw[0]) << 8) | raw[1];
	x1 = xpcc::math::mul(ut - calibration.ac6, calibration.ac5) >> 15;
	x2 = (int32_t(calibration.mc) << 11) / (x1 + calibration.md);
	b5 = x1 + x2;
	calibratedTemperature = int16_t((b5 + 8) >> 4);
	meta |= TEMPERATURE_CALCULATED;
}

void
xpcc::bmp085::Data::calculateCalibratedPressure()
{
	int32_t x1, x2, x3, b3, p;
	int16_t b6;
	uint32_t b4, b7;
	uint8_t oss = ((meta & i(Mode::Mask)) >> 6);

	calculateCalibratedTemperature();

	uint32_t up = ( (uint32_t(raw[2]) << 16) | (uint16_t(raw[3]) << 8) | raw[4] ) >> (8 - oss);
	b6 = int16_t(b5 - 4000);
	x1 = xpcc::math::mul(calibration.b2, xpcc::math::mul(b6, b6) >> 12) >> 11;
	x2 = xpcc::math::mul(calibration.ac2, b6) >> 11;
	x3 = x1 + x2;
	b3 = (xpcc::math::mul(calibration.ac1, 4) + x3) << oss;
	b3 = (b3 + 2) >> 2;
	x1 = xpcc::math::mul(calibration.ac3, b6) >> 13;
	x2 = xpcc::math::mul(calibration.b1, xpcc::math::mul(b6, b6) >> 12) >> 16;
	x3 = ((x1 + x2) + 2) >> 2;
	b4 = xpcc::math::mul(calibration.ac4, uint16_t(x3 + 32768)) >> 15;
	b7 = (up - b3) * (50000 >> oss);
	if (b7 < 0x80000000)
		p = (b7 << 1) / b4;
	else
		p = (b7 / b4) << 1;

	x1 = xpcc::math::mul(uint16_t(p >> 8), uint16_t(p >> 8));
	x1 = (x1 * 3038) >> 16;
	x2 = (-7357 * p) >> 16;
	calibratedPressure = p + ((x1 + x2 + 3791) >> 4);
	meta |= PRESSURE_CALCULATED;
}

template < typename I2cMaster >
constexpr uint8_t
xpcc::Bmp085< I2cMaster >::conversionDelay[];
