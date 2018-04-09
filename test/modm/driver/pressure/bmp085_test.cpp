/*
 * Copyright (c) 2016-2017, Sascha Schade
 * Copyright (c) 2016-2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/driver/pressure/bmp085.hpp>
#include <modm/debug/logger/logger.hpp>

#include <algorithm> // for std::max

#include "bmp085_test.hpp"

#undef  MODM_LOG_LEVEL
#define MODM_LOG_LEVEL modm::log::DISABLED

void
Bmp085Test::testConversion()
{
	modm::bmp085::DataDouble dataTable[3];
	uint16_t adc_temp_min[3];
	uint16_t adc_temp_max[3];

	uint16_t adc_press_min[3];
	uint16_t adc_press_max[3];

	// Set of calibration constants taken from Bosch's datasheet BMP180
	dataTable[0].calibration.ac1 =    408;
	dataTable[0].calibration.ac2 =    -72;
	dataTable[0].calibration.ac3 = -14383;
	dataTable[0].calibration.ac4 =  32741;
	dataTable[0].calibration.ac5 =  32757;
	dataTable[0].calibration.ac6 =  23153;
	dataTable[0].calibration.b1  =   6190;
	dataTable[0].calibration.b2  =      4;
	dataTable[0].calibration.mb  = -32768;
	dataTable[0].calibration.mc  =  -8711;
	dataTable[0].calibration.md  =   2868;
	adc_temp_min[0] = 0x366d;
	adc_temp_max[0] = 0x4b40;
	adc_press_min[0] = 0x4000;
	adc_press_max[0] = 0xE000;

	// Set of calibration constants taken from
	// wmrx00.sourceforge.net/Arduino/BMP085-Calcs.pdf
	dataTable[1].calibration.ac1 =   7911;
	dataTable[1].calibration.ac2 =   -934;
	dataTable[1].calibration.ac3 = -14306;
	dataTable[1].calibration.ac4 =  31567;
	dataTable[1].calibration.ac5 =  25671;
	dataTable[1].calibration.ac6 =  18974;
	dataTable[1].calibration.b1  =   5498;
	dataTable[1].calibration.b2  =     46;
	dataTable[1].calibration.mb  = -32768;
	dataTable[1].calibration.mc  = -11075;
	dataTable[1].calibration.md  =   2432;
	adc_temp_min[1] = 0x1a5a;
	adc_temp_max[1] = 0x3778;
	adc_press_min[1] = 0x4000;
	adc_press_max[1] = 0xE000;

	// Set of calibration constants read from an arbitrary chosen sensor
	dataTable[2].calibration.ac1 =   6805;
	dataTable[2].calibration.ac2 =  -1014;
	dataTable[2].calibration.ac3 = -14733;
	dataTable[2].calibration.ac4 =  33868;
	dataTable[2].calibration.ac5 =  24983;
	dataTable[2].calibration.ac6 =  18496;
	dataTable[2].calibration.b1  =   6515;
	dataTable[2].calibration.b2  =     33;
	dataTable[2].calibration.mb  = -32768;
	dataTable[2].calibration.mc  = -11786;
	dataTable[2].calibration.md  =   2830;
	adc_temp_min[2] = 0x15cc;
	adc_temp_max[2] = 0x32cd;
	adc_press_min[2] = 0x4000;
	adc_press_max[2] = 0xE000;

	for (size_t jj = 0; jj < MODM_ARRAY_SIZE(dataTable); ++jj)
	{
		modm::bmp085::DataDouble* dataDouble;

		dataDouble = &(dataTable[jj]);

		modm::bmp085::Data data;

		data.calibration.ac1 = dataDouble->calibration.ac1;
		data.calibration.ac2 = dataDouble->calibration.ac2;
		data.calibration.ac3 = dataDouble->calibration.ac3;
		data.calibration.ac4 = dataDouble->calibration.ac4;
		data.calibration.ac5 = dataDouble->calibration.ac5;
		data.calibration.ac6 = dataDouble->calibration.ac6;
		data.calibration.b1  = dataDouble->calibration.b1;
		data.calibration.b2  = dataDouble->calibration.b2;
		data.calibration.mb  = dataDouble->calibration.mb;
		data.calibration.mc  = dataDouble->calibration.mc;
		data.calibration.md  = dataDouble->calibration.md;

		// Temperature

		uint16_t total_error = 0;
		uint16_t max_error = 0;

		MODM_LOG_DEBUG.printf("ADC --double--\t--int--\t--error--\n");

		for (uint16_t adc = adc_temp_min[jj]; adc <= adc_temp_max[jj]; ++adc)
		{
			dataDouble->raw[0] = adc >> 8;
			dataDouble->raw[1] = adc & 0xff;
			dataDouble->rawTemperatureTouched();

			data.raw[0] = dataDouble->raw[0];
			data.raw[1] = dataDouble->raw[1];
			data.rawTemperatureTouched();

			int16_t tempDouble = dataDouble->getTemperature();
			int16_t temp = data.getTemperature();

			int16_t error = tempDouble - temp;

			MODM_LOG_DEBUG.printf("%04x\t%5d\t%5d\t%2d\n", adc, tempDouble, temp, error);

			uint16_t error_abs = error;
			if (error < 0) error_abs = -error;
			total_error += error_abs;
			if (error_abs > max_error) {
				max_error = error_abs;
			}
		}

		MODM_LOG_DEBUG.printf("== Errors ==\n");
		MODM_LOG_DEBUG.printf(" max = %d\n", max_error);
		MODM_LOG_DEBUG.printf(" sum = %d\n", total_error);

		TEST_ASSERT_TRUE(total_error < 1000);
		TEST_ASSERT_TRUE(max_error <= 1);

		// Pressure

		total_error = 0;
		max_error = 0;

		// Check pressure conversion at 10 different temperatures
		uint16_t adc_temp_span = adc_temp_max[jj] - adc_temp_min[jj];
		uint16_t adc_temp_inc  = std::max(adc_temp_span / 10U, 1U);

		for (uint16_t adc_temp = adc_temp_min[jj];
			 adc_temp < adc_temp_max[jj];
			 adc_temp += adc_temp_inc)
		{
			data.raw[0] = adc_temp >> 8;
			data.raw[1] = adc_temp & 0xff;
			data.rawTemperatureTouched();

			dataDouble->raw[0] = adc_temp >> 8;
			dataDouble->raw[1] = adc_temp & 0xff;
			dataDouble->rawTemperatureTouched();

			double temp;
			dataDouble->getTemperature(temp);

			MODM_LOG_DEBUG.printf("adc_temp = %04x, T = %f\n", adc_temp, temp);

			uint16_t adc_press_span = adc_press_max[jj] - adc_press_min[jj];
			uint16_t adc_press_inc  = std::max(adc_press_span / 10U, 1U);
			for (uint16_t adc_press = adc_press_min[jj];
				 adc_press < adc_press_max[jj];
				 adc_press += adc_press_inc)
			{
				data.raw[2] = adc_press >> 8;
				data.raw[3] = adc_press & 0xff;
				data.raw[4] = 0;
				data.rawPressureTouched();

				dataDouble->raw[2] = adc_press >> 8;
				dataDouble->raw[3] = adc_press & 0xff;
				dataDouble->raw[4] = 0;
				dataDouble->rawPressureTouched();


				auto pressFp = data.getPressure();
				auto pressDp = dataDouble->getPressure();

				if ((pressFp < 30000) or (pressDp < 30000) or (pressFp > 110000) or (pressDp > 110000)) {
					continue;
				}

				int16_t error = pressFp - pressDp;
				MODM_LOG_DEBUG.printf("  adc_press = %04" PRIx16 "  PressFp = %6" PRId32 " Pa\t PressDp = %6" PRId32 " Pa \t Diff = %3" PRId16 " Pa\n", adc_press, pressFp, pressDp, error);

				uint16_t error_abs = error;
				if (error < 0) error_abs = -error;
				total_error += error_abs;
				if (error_abs > max_error) {
					max_error = error_abs;
				}
			}
		}

		MODM_LOG_DEBUG.printf("== Errors ==\n");
		MODM_LOG_DEBUG.printf(" max = %d\n", max_error);
		MODM_LOG_DEBUG.printf(" sum = %d\n", total_error);

		TEST_ASSERT_EQUALS_RANGE(total_error, 0u, 1541u);
		TEST_ASSERT_EQUALS_RANGE(max_error,   0u,   71u);
	}
}
