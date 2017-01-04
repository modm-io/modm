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

#include <modm/driver/pressure/bme280.hpp>
#include <modm/debug/logger/logger.hpp>

#include "bme280_test.hpp"

#undef  XPCC_LOG_LEVEL
#define XPCC_LOG_LEVEL xpcc::log::DISABLED

void
Bme280Test::testConversion()
{
	static constexpr size_t table_size = 4;
	xpcc::bme280::DataDouble dataTable[table_size];
	uint32_t adc_temp_min[table_size];
	uint32_t adc_temp_max[table_size];

	uint32_t adc_press_min[table_size];
	uint32_t adc_press_max[table_size];

	// Set of calibration constants read from an arbitrary chosen sensor (A)
	dataTable[0].calibration.T1 = 28192;
	dataTable[0].calibration.T2 = 26148;
	dataTable[0].calibration.T3 = 50;
	dataTable[0].calibration.P1 = 38104;
	dataTable[0].calibration.P2 = -10853;
	dataTable[0].calibration.P3 = 3024;
	dataTable[0].calibration.P4 = 10657;
	dataTable[0].calibration.P5 = 13;
	dataTable[0].calibration.P6 = -7;
	dataTable[0].calibration.P7 = 9900;
	dataTable[0].calibration.P8 = -10230;
	dataTable[0].calibration.P9 = 4285;
	dataTable[0].calibration.H1 = 75;
	dataTable[0].calibration.H2 = 359;
	dataTable[0].calibration.H3 = 0;
	dataTable[0].calibration.H4 = 326;
	dataTable[0].calibration.H5 = 0;
	dataTable[0].calibration.H6 = 30;

	adc_temp_min[0] = 0x4ECA0;
	adc_temp_max[0] = 0xB0AA0;

	adc_press_min[0] = 0x01000;
	adc_press_max[0] = 0xE0000;


	// Set of calibration constants read from an arbitrary chosen sensor (B)
	dataTable[1].calibration.T1 = 28533;
	dataTable[1].calibration.T2 = 27194;
	dataTable[1].calibration.T3 = 50;
	dataTable[1].calibration.P1 = 38286;
	dataTable[1].calibration.P2 = -10616;
	dataTable[1].calibration.P3 = 3024;
	dataTable[1].calibration.P4 = 4099;
	dataTable[1].calibration.P5 = 40;
	dataTable[1].calibration.P6 = -7;
	dataTable[1].calibration.P7 = 9900;
	dataTable[1].calibration.P8 = -10230;
	dataTable[1].calibration.P9 = 4285;
	dataTable[1].calibration.H1 = 75;
	dataTable[1].calibration.H2 = 356;
	dataTable[1].calibration.H3 = 0;
	dataTable[1].calibration.H4 = 334;
	dataTable[1].calibration.H5 = 0;
	dataTable[1].calibration.H6 = 30;

	adc_temp_min[1] = 0x51530;
	adc_temp_max[1] = 0xAF720;

	adc_press_min[1] = 0x01000;
	adc_press_max[1] = 0xE0000;


	// Sample data from BMP280 datasheet
	dataTable[2].calibration.T1 = 27504;
	dataTable[2].calibration.T2 = 26435;
	dataTable[2].calibration.T3 = -1000;
	dataTable[2].calibration.P1 = 36477;
	dataTable[2].calibration.P2 = -10685;
	dataTable[2].calibration.P3 = 3024;
	dataTable[2].calibration.P4 = 2855;
	dataTable[2].calibration.P5 = 140;
	dataTable[2].calibration.P6 = -7;
	dataTable[2].calibration.P7 = 15500;
	dataTable[2].calibration.P8 = -14600;
	dataTable[2].calibration.P9 = 6000;

	adc_temp_min[2] = 519888;
	adc_temp_max[2] = 519889;

	adc_press_min[2] = 0x01000;
	adc_press_max[2] = 0xE0000;


	// Set of calibration constants from a Sparkfun screenshot
	// https://learn.sparkfun.com/tutorials/sparkfun-bme280-breakout-hookup-guide
	// https://cdn.sparkfun.com/assets/learn_tutorials/4/1/9/readall_output.png
	// ADC Values from screenshot. Temperature is 25.13 degree Celsius.

	dataTable[3].calibration.T1 = 27974;
	dataTable[3].calibration.T2 = 26594;
	dataTable[3].calibration.T3 = 50;
	dataTable[3].calibration.P1 = 38207;
	dataTable[3].calibration.P2 = -10702;
	dataTable[3].calibration.P3 = 3024;
	dataTable[3].calibration.P4 = 7917;
	dataTable[3].calibration.P5 = -188;
	dataTable[3].calibration.P6 = -7;
	dataTable[3].calibration.P7 = 9900;
	dataTable[3].calibration.P8 = -10230;
	dataTable[3].calibration.P9 = 4285;
	dataTable[3].calibration.H1 = 75;
	dataTable[3].calibration.H2 = 358;
	dataTable[3].calibration.H3 = 0;
	dataTable[3].calibration.H4 = 328;
	dataTable[3].calibration.H5 = 0;
	dataTable[3].calibration.H6 = 30;

	adc_temp_min[3] = 0x809F0;
	adc_temp_max[3] = 0x809F0;

	adc_press_min[3] = 0x62090;
	adc_press_max[3] = 0x62090;


	for (size_t jj = 0; jj < XPCC_ARRAY_SIZE(dataTable); ++jj)
	{
		xpcc::bme280::DataDouble* dataDouble;

		dataDouble = &(dataTable[jj]);

		// Fixed Point
		xpcc::bme280::Data data;

		// Copy calibration from double precision to fixed point
		data.calibration.T1 = dataDouble->calibration.T1;
		data.calibration.T2 = dataDouble->calibration.T2;
		data.calibration.T3 = dataDouble->calibration.T3;

		data.calibration.P1 = dataDouble->calibration.P1;
		data.calibration.P2 = dataDouble->calibration.P2;
		data.calibration.P3 = dataDouble->calibration.P3;
		data.calibration.P4 = dataDouble->calibration.P4;
		data.calibration.P5 = dataDouble->calibration.P5;
		data.calibration.P6 = dataDouble->calibration.P6;
		data.calibration.P7 = dataDouble->calibration.P7;
		data.calibration.P8 = dataDouble->calibration.P8;
		data.calibration.P9 = dataDouble->calibration.P9;

		data.calibration.H1 = dataDouble->calibration.H1;
		data.calibration.H2 = dataDouble->calibration.H2;
		data.calibration.H3 = dataDouble->calibration.H3;
		data.calibration.H4 = dataDouble->calibration.H4;
		data.calibration.H5 = dataDouble->calibration.H5;
		data.calibration.H6 = dataDouble->calibration.H6;

		// Evaluate Temperature

		uint32_t total_error = 0;
		uint32_t max_error = 0;

		XPCC_LOG_DEBUG.printf("ADC --double--\t--int--\t--error--\n");

		for (uint32_t adc = adc_temp_min[jj]; adc <= adc_temp_max[jj]; ++adc)
		{
			dataDouble->raw[3] = adc >> 12;
			dataDouble->raw[4] = adc >> 4;
			dataDouble->raw[5] = adc << 4;
			dataDouble->rawTemperatureTouched();

			data.raw[3] = dataDouble->raw[3];
			data.raw[4] = dataDouble->raw[4];
			data.raw[5] = dataDouble->raw[5];
			data.rawTemperatureTouched();

			int32_t tempDouble = dataDouble->getTemperature();
			int32_t temp = data.getTemperature();

			int32_t error = tempDouble - temp;

			XPCC_LOG_DEBUG.printf("%05x\t%5d\t%5d\t%2d\n", adc, tempDouble, temp, error);

			error = std::abs(error);
			total_error += error;
			if (uint32_t(error) > max_error) {
				max_error = error;
			}
		}

		XPCC_LOG_DEBUG.printf("== Errors ==\n");
		XPCC_LOG_DEBUG.printf(" max = %d\n", max_error);
		XPCC_LOG_DEBUG.printf(" sum = %d\n", total_error);

		TEST_ASSERT_TRUE(total_error < 51000);
		TEST_ASSERT_TRUE(max_error <= 1);


		// Pressure

		total_error = 0;
		max_error = 0;

		// Check pressure conversion at 10 different temperatures
		uint32_t adc_temp_span = adc_temp_max[jj] - adc_temp_min[jj];

		uint32_t adc_temp_step = adc_temp_span/10;
		if (adc_temp_step == 0) {
			adc_temp_step = 1;
		}

		for (uint32_t adc_temp = adc_temp_min[jj]; 
			 adc_temp <= adc_temp_max[jj]; 
			 adc_temp += adc_temp_step)
		{
			dataDouble->raw[3] = adc_temp >> 12;
			dataDouble->raw[4] = adc_temp >> 4;
			dataDouble->raw[5] = adc_temp << 4;
			dataDouble->rawTemperatureTouched();

			data.raw[3] = dataDouble->raw[3];
			data.raw[4] = dataDouble->raw[4];
			data.raw[5] = dataDouble->raw[5];
			data.rawTemperatureTouched();

			double temp;
			dataDouble->getTemperature(temp);

			XPCC_LOG_DEBUG.printf("adc_temp = %05x, T = %f\n", adc_temp, temp);

			uint32_t adc_press_span = adc_press_max[jj] - adc_press_min[jj];

			uint32_t adc_press_step = adc_press_span / 10;
			if (adc_press_step == 0) {
				adc_press_step = 1;
			}

			for (uint32_t adc_press = adc_press_min[jj];
				 adc_press <= adc_press_max[jj];
				 adc_press += adc_press_step)
			{
				data.raw[0] = adc_press >> 12;
				data.raw[1] = adc_press >> 4;
				data.raw[2] = adc_press << 4;
				data.rawPressureTouched();

				dataDouble->raw[0] = data.raw[0];
				dataDouble->raw[1] = data.raw[1];
				dataDouble->raw[2] = data.raw[2];
				dataDouble->rawPressureTouched();


				auto pressFp = data.getPressure() / 1000;
				auto pressDp = dataDouble->getPressure();

				if ((pressFp < 30000) or (pressDp < 30000) or (pressFp > 110000) or (pressDp > 110000)) {
					continue;
				}

				int32_t error = pressFp - pressDp;
				XPCC_LOG_DEBUG.printf("  adc_press = %05x  PressFp = %9d Pa\t PressDp = %9d Pa \t Diff = %5d Pa\n", 
					adc_press, pressFp, pressDp, error);

				error = std::abs(error);
				total_error += error;
				if (uint32_t(error) > max_error) {
					max_error = error;
				}
			}
		}

		XPCC_LOG_DEBUG.printf("== Errors ==\n");
		XPCC_LOG_DEBUG.printf(" max = %d\n", max_error);
		XPCC_LOG_DEBUG.printf(" sum = %d\n", total_error);

		TEST_ASSERT_TRUE(total_error <= 2455);
		TEST_ASSERT_TRUE(max_error <= 50);
	}
}
