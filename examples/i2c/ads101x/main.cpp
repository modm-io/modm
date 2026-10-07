// coding: utf-8
/*
 * Copyright (c) 2022, Jonas Kazem  Andersen
 * Copyright (c) 2022, Rasmus Kleist Hørlyck Sørensen
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <modm/processing.hpp>
#include <modm/driver/adc/ads101x.hpp>

using Scl = modm::platform::GpioC6;
using Sda = modm::platform::GpioC7;
using I2cMaster = modm::platform::I2cMaster4;

modm::ads101x::Data data;
modm::Ads101x<I2cMaster> adc{data};

modm::Fiber fiber_sensor([]
{
	// test of communication and initialization
	while (not adc.ping())
	{
		MODM_LOG_ERROR << "Pinging Ads101x failed" << modm::endl;
		modm::this_fiber::sleep_for(1s);
	}

	while (not adc.initialize())
	{
		MODM_LOG_ERROR << "Initialising Ads101x failed" << modm::endl;
		modm::this_fiber::sleep_for(1s);
	}

	MODM_LOG_INFO << "Ads101x successfully initialised" << modm::endl;

	while (true)
	{
		// Single shot and full scale range test
		MODM_LOG_INFO << "Started single conversion test" << modm::endl;

		modm::ShortTimeout timeout(10s);
		while(not timeout.isExpired())
		{
			// Test single shoot mode with full scale range 0.256
			adc.setFullScaleRange(modm::ads101x::FullScaleRange::V0_256);
			adc.startSingleShotConversion(modm::ads101x::InputMultiplexer::Input4);
			while (adc.isBusy()) modm::this_fiber::yield();
			adc.readConversionResult();
			MODM_LOG_INFO << "Single Conversion result in range 0.256V: " << data.getVoltage() << modm::endl;

			// Test single shoot mode with full scale range 0.512
			adc.setFullScaleRange(modm::ads101x::FullScaleRange::V0_512);
			adc.startSingleShotConversion(modm::ads101x::InputMultiplexer::Input4);
			while (adc.isBusy()) modm::this_fiber::yield();
			adc.readConversionResult();
			MODM_LOG_INFO << "Single Conversion result in range 0.512V: " << data.getVoltage() << modm::endl;

			// Test single shoot mode with full scale range 1.024
			adc.setFullScaleRange(modm::ads101x::FullScaleRange::V1_024);
			adc.startSingleShotConversion(modm::ads101x::InputMultiplexer::Input4);
			while (adc.isBusy()) modm::this_fiber::yield();
			adc.readConversionResult();
			MODM_LOG_INFO << "Single Conversion result in range 1.024V: " << data.getVoltage() << modm::endl;

			// Test single shoot mode with full scale range 2.048
			adc.setFullScaleRange(modm::ads101x::FullScaleRange::V2_048);
			adc.startSingleShotConversion(modm::ads101x::InputMultiplexer::Input4);
			while (adc.isBusy()) modm::this_fiber::yield();
			adc.readConversionResult();
			MODM_LOG_INFO << "Single Conversion result in range 2.048V: " << data.getVoltage() << modm::endl;

			// Test single shoot mode with full scale range 4.096
			adc.setFullScaleRange(modm::ads101x::FullScaleRange::V4_096);
			adc.startSingleShotConversion(modm::ads101x::InputMultiplexer::Input4);
			while (adc.isBusy()) modm::this_fiber::yield();
			adc.readConversionResult();
			MODM_LOG_INFO << "Single Conversion result in range 4.096V: " << data.getVoltage() << modm::endl;

			// Test single shoot mode with full scale range 6.144
			adc.setFullScaleRange(modm::ads101x::FullScaleRange::V6_144);
			adc.startSingleShotConversion(modm::ads101x::InputMultiplexer::Input4);
			while (adc.isBusy()) modm::this_fiber::yield();
			adc.readConversionResult();
			MODM_LOG_INFO << "Single Conversion result in range 6.144V: " << data.getVoltage() << modm::endl;

			modm::this_fiber::sleep_for(1s);
		}

		// Continuous mode test
		MODM_LOG_INFO << "Started continuous conversion test" << modm::endl;
		adc.setFullScaleRange(modm::ads101x::FullScaleRange::V6_144);
		adc.startContinuousConversion(modm::ads101x::InputMultiplexer::Input4);

		timeout.restart(10s);
		while (not timeout.isExpired())
		{
			modm::this_fiber::sleep_for(1s);
			adc.readConversionResult();
			MODM_LOG_INFO << "Continuous Conversion result in range 6.144V: " << data.getVoltage() << modm::endl;
		}
	}
});

modm::Fiber fiber_blink([]
{
	Board::LedD13::setOutput();
	while(true)
	{
		Board::LedD13::toggle();
		modm::this_fiber::sleep_for(0.5s);
	}
});

int
main()
{
	Board::initialize();

	I2cMaster::connect<Scl::Scl, Sda::Sda>(I2cMaster::PullUps::Internal);
	I2cMaster::initialize<Board::SystemClock, 100_kHz>();

	MODM_LOG_INFO << "==========ADS101x Test==========" << modm::endl;

	modm::fiber::Scheduler::run();

	return 0;
}
