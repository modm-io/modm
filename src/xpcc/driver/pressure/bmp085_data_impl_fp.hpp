// coding: utf-8
/* Copyright (c) 2016, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include <stdio.h>
#include <inttypes.h>
#include <cmath>

#include <xpcc/debug/logger/logger.hpp>

#ifndef XPCC_BMP085_DATA_HPP
#	error  "Don't include this file directly, use 'bmp180_data.hpp' instead!"
#endif

#undef  XPCC_LOG_LEVEL
#define XPCC_LOG_LEVEL xpcc::log::DISABLED

namespace xpcc {

namespace bmp085data {

// Fixed point implementation

void
Data::calculateCalibratedTemperature()
{
	int32_t x1, x2;
	uint16_t ut = (uint16_t(raw[0]) << 8) | raw[1];
	XPCC_LOG_DEBUG.printf("ut = %" PRId16 "\n", ut);

	x1 = xpcc::math::mul( int16_t(ut - calibration.ac6), int16_t(calibration.ac5)) >> 15;
	XPCC_LOG_DEBUG.printf("x1 = %" PRId32 "\n", x1);

	x2 = (int32_t(calibration.mc) << 11) / (x1 + calibration.md);
	XPCC_LOG_DEBUG.printf("x2 = %" PRId32 "\n", x2);

	b5 = x1 + x2;
	XPCC_LOG_DEBUG.printf("b5 = %" PRId32 "\n", b5);

	calibratedTemperature = int16_t((b5 + 8) >> 4);
	XPCC_LOG_DEBUG.printf("T = %" PRId16 "\n", calibratedTemperature);

	meta |= TEMPERATURE_CALCULATED;
}

void
Data::calculateCalibratedPressure()
{
	int32_t x1, x2, x3, b3, p;
	int16_t b6;
	uint32_t b4, b7;
	uint8_t oss = ((meta & 0b11000000) >> 6); // ToDo Use Mask again

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

int16_t
Data::getTemperature()
{
	if (not (meta & TEMPERATURE_CALCULATED)) {
		calculateCalibratedTemperature();
	}
	return calibratedTemperature;
}

int32_t
Data::getPressure()
{
	if (not (meta & PRESSURE_CALCULATED)) {
		calculateCalibratedPressure();
	}
	return calibratedPressure;
}

} // bmp085data namespace

} // xpcc namespace
