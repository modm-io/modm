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

#include <stdio.h>
#include <cmath>
#include <inttypes.h>

#include <modm/debug/logger/logger.hpp>

#ifndef MODM_BMP085_DATA_HPP
#	error  "Don't include this file directly, use 'bmp180_data.hpp' instead!"
#endif

#undef  MODM_LOG_LEVEL
#define MODM_LOG_LEVEL modm::log::DISABLED

namespace modm {

namespace bmp085data {

// Double precision implementation

void
DataDouble::calculateCalibratedTemperature()
{
	if (not (meta & CALIBRATION_CALCULATED))
	{
		double c3 = double(160.0) * ::pow(2.0, -15.0) * calibration.ac3;
		double c4 = ::pow(10, -3) * ::pow(2, -15) * calibration.ac4;
		double b1 = ::pow(160, 2) * ::pow(2, -30) * calibration.b1;

		c5 = (::pow(2, -15) / 160.0) * double(calibration.ac5);
		c6 = calibration.ac6;
		mc = (::pow(2, 11) / ::pow(160, 2)) * double(calibration.mc);
		md = calibration.md / 160.0;
		x0 = calibration.ac1;
		x1 = double(160.0) * ::pow(2, -13) * double(calibration.ac2);
		x2 = ::pow(160, 2) * ::pow(2, -25) * double(calibration.b2);
		y00 = c4 * ::pow(2, 15);
		y11 = c4 * c3;
		y2 = c4 * b1;
		p0 = (3791.0 - 8.0) / 1600.0;
		p1 = double(1.0) - double(7357.0) * ::pow(2, -20);
		p2 = double(3038.0) * double(100.0) * ::pow(2, -36);

		MODM_LOG_DEBUG.printf("c3 = %9.5f\n", c3);
		MODM_LOG_DEBUG.printf("c4 = %9.5f\n", c4);
		MODM_LOG_DEBUG.printf("b1 = %9.5f\n", b1);
		MODM_LOG_DEBUG.printf("c5 = %9.5f\n", c5);
		MODM_LOG_DEBUG.printf("c6 = %9.5f\n", c6);
		MODM_LOG_DEBUG.printf("mc = %9.5f\n", mc);
		MODM_LOG_DEBUG.printf("md = %9.5f\n", md);
		MODM_LOG_DEBUG.printf("x0 = %9.5f\n", x0);
		MODM_LOG_DEBUG.printf("x1 = %9.5f\n", x1);
		MODM_LOG_DEBUG.printf("x2 = %9.5f\n", x2);
		MODM_LOG_DEBUG.printf("y00 = %9.5f\n", y00);
		MODM_LOG_DEBUG.printf("y11 = %9.5f\n", y11);
		MODM_LOG_DEBUG.printf("y2 = %9.5f\n", y2);
		MODM_LOG_DEBUG.printf("p0 = %9.5f\n", p0);
		MODM_LOG_DEBUG.printf("p1 = %9.5f\n", p1);
		MODM_LOG_DEBUG.printf("p2 = %9.5f\n", p2);

		meta |= CALIBRATION_CALCULATED;
	}

	int16_t adc = raw[0] << 8 | raw[1];

	double tu = adc;
	double a = c5 * (tu - c6);
	calibratedTemperatureDouble = (a + (mc / (a + md)));

	MODM_LOG_DEBUG.printf("T dp = %4.2f\n", calibratedTemperatureDouble);

	meta |= TEMPERATURE_CALCULATED;
}

void
DataDouble::calculateCalibratedPressure()
{
	MODM_LOG_DEBUG.printf("raw[2:5] = %02x %02x %02x\n", raw[2], raw[3], raw[4]);

	uint32_t up = ( (uint32_t(raw[2]) << 16) | (uint16_t(raw[3]) << 8) | raw[4] );
	MODM_LOG_DEBUG.printf("up = %9" PRId32 "\n", up);

	double pu = up / double(256.0);
	MODM_LOG_DEBUG.printf("pu = %9.5f\n", pu);

	calculateCalibratedTemperature();

	double s = calibratedTemperatureDouble - double(25.0);
	double x = (x2 * s * s) + ( x1 * s) + x0;
	double y = (y2 * s * s) + (y11 * s) + y00;
	double z = (pu - x) / y;

	MODM_LOG_DEBUG.printf("s = %9.5f\n", s);
	MODM_LOG_DEBUG.printf("x = %9.5f\n", x);
	MODM_LOG_DEBUG.printf("y = %9.5f\n", y);
	MODM_LOG_DEBUG.printf("z = %9.5f\n", z);

	calibratedPressureDouble = (p2 * pow(z,2)) + (p1 * z) + p0;

	MODM_LOG_DEBUG.printf("calibratedPressureDouble = %9.5f\n", calibratedPressureDouble);

	meta |= PRESSURE_CALCULATED;
}


int16_t
DataDouble::getTemperature()
{
	if (not (meta & TEMPERATURE_CALCULATED)) {
		calculateCalibratedTemperature();
	}
	return lrint(calibratedTemperatureDouble * double(10.0));
}

void
DataDouble::getTemperature(float &temp)
{
	if (not (meta & TEMPERATURE_CALCULATED)) {
		calculateCalibratedTemperature();
	}
	temp = float(calibratedTemperatureDouble);
}

void
DataDouble::getTemperature(double &temp)
{
	if (not (meta & TEMPERATURE_CALCULATED)) {
		calculateCalibratedTemperature();
	}
	temp = calibratedTemperatureDouble;
}


int32_t
DataDouble::getPressure()
{
	if (not (meta & PRESSURE_CALCULATED)) {
		calculateCalibratedPressure();
	}
	return lrint(calibratedPressureDouble * double(100.0));
}

} // bmp085data namespace

} // modm namespace
