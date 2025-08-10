/*
 * Copyright (c) 2014, Sascha Schade
 * Copyright (c) 2014-2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/processing.hpp>
#include <modm/driver/temperature/tmp102.hpp>

typedef I2cMaster1 MyI2cMaster;

modm::tmp102::Data temperatureData;
modm::Tmp102<MyI2cMaster> temp(temperatureData, 0x48);

modm::Fiber fiber_sensor([]
{
	// ping the device until it responds
	while(not temp.ping()) modm::delay(100ms);

	temp.setUpdateRate(200);
	temp.enableExtendedMode();

	temp.configureAlertMode(
			modm::tmp102::ThermostatMode::Comparator,
			modm::tmp102::AlertPolarity::ActiveLow,
			modm::tmp102::FaultQueue::Faults6);
	temp.setLowerLimit(28.f);
	temp.setUpperLimit(30.f);

	while (true)
	{
		bool result{};
		temp.readComparatorMode(result);
		float temperature = temperatureData.getTemperature();
		uint8_t tI = (int) temperature;
		uint16_t tP = (temperature - tI) * 10000;
		MODM_LOG_INFO << "T= " << tI << ".";
		if (tP == 0)
		{
			MODM_LOG_INFO << "0000 C";
		}
		else if (tP == 625)
		{
			MODM_LOG_INFO << "0" << tP << " C";
		}
		else
		{
			MODM_LOG_INFO << tP << " C";
		}
		MODM_LOG_INFO << modm::endl;
		if (result) { MODM_LOG_INFO << "Heat me up!" << modm::endl; }
		modm::this_fiber::sleep_for(200ms);
		Board::LedDown::toggle();
	}
});

modm::Fiber fiber_blink([]
{
	Board::LedUp::setOutput();
	while(true)
	{
		Board::LedUp::toggle();
		modm::this_fiber::sleep_for(0.5s);
	}
});

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	MyI2cMaster::connect<GpioB7::Sda, GpioB8::Scl>();
	MyI2cMaster::initialize<Board::SystemClock, 400_kHz>();

	MODM_LOG_INFO << "\n\nRESTART\n\n";

	modm::fiber::Scheduler::run();

	return 0;
}
