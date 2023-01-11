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

#ifndef MODM_MS5837_DATA_HPP
#	error  "Don't include this file directly, use 'ms5837_data.hpp' instead!"
#endif

namespace modm
{

namespace ms5837data
{

int32_t
Data::getPressure()
{
	if (not (meta & PRESSURE_CALCULATED))
	{
		calculateCalibratedValues();
	}
	return calibratedPressure;
}

int32_t
Data::getTemperature()
{
	if (not (meta & TEMPERATURE_CALCULATED))
	{
		calculateCalibratedValues();
	}
	return calibratedTemperature;
}

void
Data::calculateCalibratedValues()
{
	// Read uncompensated pressure and temperature from raw buffer
	const uint32_t D1_rawPress = uint32_t(raw[0] << 16 | raw[1] << 8 | raw[2]);
	const uint32_t D2_rawTemp = uint32_t(raw[3] << 16 | raw[4] << 8 | raw[5]);

	// Compute intermediate values used to calculate the pressure and temperature
	const int32_t dT = D2_rawTemp - (uint32_t(prom.data[5]) << 8);

	const int32_t temp = 2000 + ((int64_t(dT) * prom.data[6]) >> 23);
	const int64_t off = (int64_t(prom.data[2]) << 16) + ((int64_t(prom.data[4]) * int64_t(dT)) >> 7);
	const int64_t sens = (int64_t(prom.data[1]) << 15) + ((int64_t(prom.data[3]) * int64_t(dT)) >> 8);

	int64_t temp_i = 0;
	int64_t off_i = 0;
	int64_t sens_i = 0;

	// Second order temperature compensation

	// Low temp
	if (temp < 2000)
	{
		// Temperature is less then 20.00 degrees Centigrade
		temp_i = 3*(int64_t(dT) * int64_t(dT)) >> 33;

		off_i = (3 * (temp - 2000) * (temp - 2000)) >> 1;
		sens_i = (5 * (temp - 2000) * (temp - 2000)) >> 3;

		if (temp < -1500)
		{
			// Temperature is less then -15.00 degrees Centigrade
			off_i += 7 * (temp + 1500) * (temp + 1500);
			sens_i += (4 * (temp + 1500) * (temp + 1500));
		}
	}

	// High Temp
	else{
		temp_i = 2*(int64_t(dT) * int64_t(dT)) >> 37;
		off_i = (1 * (temp - 2000) * (temp - 2000)) >> 4;
		sens_i = 0;
	}

	const int64_t off_2 = off - off_i;
	const int64_t sens_2 = sens - sens_i;

	calibratedPressure = (((sens_2 * int64_t(D1_rawPress)) >> 21) - off_2) >> 13;
	meta |= PRESSURE_CALCULATED;

	calibratedTemperature = temp - temp_i;
	meta |= TEMPERATURE_CALCULATED;
}

} // ms5837data

} // modm namespace
