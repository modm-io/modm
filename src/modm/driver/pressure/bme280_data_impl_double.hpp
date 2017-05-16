/*
 * Copyright (c) 2016, Sascha Schade
 * Copyright (c) 2017, Fabian Greif
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

#include <modm/debug/logger/logger.hpp>

#ifndef MODM_BME280_DATA_HPP
#	error  "Don't include this file directly, use 'bme280_data.hpp' instead!"
#endif

#undef  MODM_LOG_LEVEL
#define MODM_LOG_LEVEL modm::log::DISABLED

namespace modm
{
namespace bme280data
{
// Double precision implementation

DataDouble::DataDouble() :
	t_fine(0)
{
}

void
DataDouble::calculateCalibratedTemperature()
{
	// Temperature value is expected to be received in 20 bit format,
	// positive, stored in a 32 bit signed integer.
	int32_t adc = ((raw[3] << 16) | (raw[4] << 8) | (raw[5] << 0));
	adc >>= 4;

	double adc_16384 = double(adc) / double(16384.0);
	double adc_131072 = double(adc) / double(131072.0);

	double T1_1024 = double(calibration.T1) / double(1024.0);
	double T1_8192 = double(calibration.T1) / double(8192.0);
	double T2 = double(calibration.T2);
	double T3 = double(calibration.T3);

	// double var1, var2, T;
	double var1 = (adc_16384 - T1_1024) * T2;
	double var2 = ((adc_131072 - T1_8192) * (adc_131072 - T1_8192)) * T3;

	t_fine = var1 + var2;

	calibratedTemperatureDouble = (var1 + var2) / double(5120.0);

	MODM_LOG_DEBUG.printf("T dp = %4.2f\n", calibratedTemperatureDouble);

	meta |= TEMPERATURE_CALCULATED;
}

void
DataDouble::calculateCalibratedPressure()
{
	// Before pressure can be calculated, temperature must be.
	if (not (meta & TEMPERATURE_CALCULATED)) {
		calculateCalibratedTemperature();
	}

	int32_t adc = (((int32_t(raw[0])) << 16) | (raw[1] << 8) | (raw[2] << 0));
	adc >>= 4;

	double P1 = double(calibration.P1);
	double P2 = double(calibration.P2);
	double P3 = double(calibration.P3);
	double P4 = double(calibration.P4);
	double P5 = double(calibration.P5);
	double P6 = double(calibration.P6);
	double P7 = double(calibration.P7);
	double P8 = double(calibration.P8);
	double P9 = double(calibration.P9);

	double var1 = (t_fine / double(2.0)) - double(64000.0);

	double var2 = var1 * var1 * P6 / double(32768.0);
	var2 = var2 + var1 * P5 * double(2.0);
	var2 = var2 / double(4.0) + (P4 * double(65636.0));

	var1 = (P3 * var1 * var1 / double(524288.0) + P2 * var1) / double(524288.0);
	var1 = (double(1.0) + var1 / double(32768.0)) * P1;
	if (var1 == 0.0) {
		calibratedPressureDouble = double(0.0);
		return;
	}

	double p = double(1048576.0) - double(adc);
	p = (p - (var2 / double(4096.0))) * double(6250.0) / var1;

	var1 = P9 * p * p / double(2147483648.0);
	var2 = p * P8 / double(32768.0);

	p = p + (var1 + var2 + P7) / double(16.0);

	calibratedPressureDouble = p;

	meta |= PRESSURE_CALCULATED;
}



int32_t
DataDouble::getTemperature()
{
	if (not (meta & TEMPERATURE_CALCULATED)) {
		calculateCalibratedTemperature();
	}
	return lrint(calibratedTemperatureDouble * double(100.0));
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
	return lrint(calibratedPressureDouble);
}

} // bme280 namespace

} // modm namespace
