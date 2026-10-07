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
#include <modm/driver/adc/ads7828.hpp>

using Scl = modm::platform::GpioC6;
using Sda = modm::platform::GpioC7;
using I2cMaster = modm::platform::I2cMaster4;

modm::ads7828::Data data;
modm::Ads7828<I2cMaster> adc{data, 0x48};

modm::Fiber fiber_sensor([]
{
	while (not adc.ping())
	{
		MODM_LOG_ERROR << "Could not ping Ads7828" << modm::endl;
		modm::this_fiber::sleep_for(1s);
	}

	while (true)
	{
		MODM_LOG_INFO << "-------------------------------" << modm::endl << modm::endl;
		adc.startMeasurement(modm::ads7828::InputChannel::Ch0);
		adc.readConversionResult();
		MODM_LOG_INFO.printf("Ch0 measuremnt is \t %.4f", (double)data.getVoltage(3.3f));
		MODM_LOG_INFO << modm::endl;

		adc.startMeasurement(modm::ads7828::InputChannel::Ch1);
		adc.readConversionResult();
		MODM_LOG_INFO.printf("Ch1 measuremnt is \t %.4f", (double)data.getVoltage(3.3f));
		MODM_LOG_INFO << modm::endl;

		adc.startMeasurement(modm::ads7828::InputChannel::Ch2);
		adc.readConversionResult();
		MODM_LOG_INFO.printf("Ch2 measuremnt is \t %.4f", (double)data.getVoltage(3.3f));
		MODM_LOG_INFO << modm::endl;

		adc.startMeasurement(modm::ads7828::InputChannel::Ch3);
		adc.readConversionResult();
		MODM_LOG_INFO.printf("Ch3 measuremnt is \t %.4f", (double)data.getVoltage(3.3f));
		MODM_LOG_INFO << modm::endl;

		adc.startMeasurement(modm::ads7828::InputChannel::Ch4);
		adc.readConversionResult();
		MODM_LOG_INFO.printf("Ch4 measuremnt is \t %.4f", (double)data.getVoltage(3.3f));
		MODM_LOG_INFO << modm::endl;

		adc.startMeasurement(modm::ads7828::InputChannel::Ch5);
		adc.readConversionResult();
		MODM_LOG_INFO.printf("Ch5 measuremnt is  \t %.4f", (double)data.getVoltage(3.3f));
		MODM_LOG_INFO << modm::endl;

		adc.startMeasurement(modm::ads7828::InputChannel::Ch6);
		adc.readConversionResult();
		MODM_LOG_INFO.printf("Ch6 measuremnt is \t %.4f", (double)data.getVoltage(3.3f));
		MODM_LOG_INFO << modm::endl;

		adc.startMeasurement(modm::ads7828::InputChannel::Ch7);
		adc.readConversionResult();
		MODM_LOG_INFO.printf("Ch7 measuremnt is \t %.4f", (double)data.getVoltage(3.3f));
		MODM_LOG_INFO << modm::endl;

		MODM_LOG_INFO << "----Diff Inputs-------------" << modm::endl;

		adc.startMeasurement(modm::ads7828::InputChannel::Ch0Ch1);
		adc.readConversionResult();
		MODM_LOG_INFO.printf("Ch0 - Ch1 is \t %.4f", (double)data.getVoltage(3.3f));
		MODM_LOG_INFO << modm::endl;

		adc.startMeasurement(modm::ads7828::InputChannel::Ch2Ch3);
		adc.readConversionResult();
		MODM_LOG_INFO.printf("Ch2 - Ch3 is\t %.4f", (double)data.getVoltage(3.3f));
		MODM_LOG_INFO << modm::endl;

		adc.startMeasurement(modm::ads7828::InputChannel::Ch4Ch5);
		adc.readConversionResult();
		MODM_LOG_INFO.printf("Ch4 - Ch5 is \t %.4f", (double)data.getVoltage(3.3f));
		MODM_LOG_INFO << modm::endl;

		adc.startMeasurement(modm::ads7828::InputChannel::Ch6Ch7);
		adc.readConversionResult();
		MODM_LOG_INFO.printf("Ch6 - Ch7 is \t %.4f", (double)data.getVoltage(3.3f));
		MODM_LOG_INFO << modm::endl;

		adc.startMeasurement(modm::ads7828::InputChannel::Ch1Ch0);
		adc.readConversionResult();
		MODM_LOG_INFO.printf("Ch1 - Ch0 is \t %.4f", (double)data.getVoltage(3.3f));
		MODM_LOG_INFO << modm::endl;

		adc.startMeasurement(modm::ads7828::InputChannel::Ch3Ch2);
		adc.readConversionResult();
		MODM_LOG_INFO.printf("Ch3 - Ch2 is \t %.4f", (double)data.getVoltage(3.3f));
		MODM_LOG_INFO << modm::endl;

		adc.startMeasurement(modm::ads7828::InputChannel::Ch5Ch4);
		adc.readConversionResult();
		MODM_LOG_INFO.printf("Ch5 - Ch4 is \t %.4f", (double)data.getVoltage(3.3f));
		MODM_LOG_INFO << modm::endl;

		adc.startMeasurement(modm::ads7828::InputChannel::Ch7Ch6);
		adc.readConversionResult();
		MODM_LOG_INFO.printf("Ch7 - Ch6 is \t %.4f", (double)data.getVoltage(3.3f));
		MODM_LOG_INFO << modm::endl;

		MODM_LOG_INFO << "---Toggling Power Down and Internal Ref----" << modm::endl;

		adc.setPowerDownSelection(modm::ads7828::PowerDown::InternalReferenceOffAdcConverterOff);
		adc.startMeasurement(modm::ads7828::InputChannel::Ch0);
		adc.readConversionResult();
		MODM_LOG_INFO.printf("Default: \t\t\t\t %.4f", (double)data.getVoltage(3.3f));
		MODM_LOG_INFO << modm::endl;

		adc.setPowerDownSelection(modm::ads7828::PowerDown::InternalReferenceOnAdcConverterOff);
		adc.startMeasurement(modm::ads7828::InputChannel::Ch0);
		adc.readConversionResult();
		MODM_LOG_INFO.printf("Internal ref on:  \t\t\t %.4f", (double)data.getVoltage(2.5f));
		MODM_LOG_INFO << modm::endl;

		adc.setPowerDownSelection(modm::ads7828::PowerDown::InternalReferenceOffAdcConverterOn);
		adc.startMeasurement(modm::ads7828::InputChannel::Ch0);
		adc.readConversionResult();
		MODM_LOG_INFO.printf("No power down \t\t\t\t %.4f", (double)data.getVoltage(3.3f));
		MODM_LOG_INFO << modm::endl;

		adc.setPowerDownSelection(modm::ads7828::PowerDown::InternalReferenceOnAdcConverterOn);
		adc.startMeasurement(modm::ads7828::InputChannel::Ch0);
		adc.readConversionResult();
		MODM_LOG_INFO.printf("Internal ref on, no power down: \t %.4f", (double)data.getVoltage(2.5f));
		MODM_LOG_INFO << modm::endl;

		MODM_LOG_INFO << "-------------------------------" << modm::endl << modm::endl;

		modm::this_fiber::sleep_for(1s);
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

	MODM_LOG_INFO << "==========Ads7828 Test==========" << modm::endl;

	modm::fiber::Scheduler::run();
	return 0;
}
