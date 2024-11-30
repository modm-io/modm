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
#include <modm/io/iostream.hpp>

using Usart2 = BufferedUart<UsartHal2>;
modm::IODeviceWrapper< Usart2, modm::IOBuffer::BlockIfFull > device;
modm::IOStream stream(device);

typedef I2cMaster1 MyI2cMaster;
modm::tmp102::Data temperatureData;
modm::Tmp102<MyI2cMaster> temp(temperatureData, 0x48);

modm::Fiber fiber_sensor([]
{
	while(not temp.ping()) modm::this_fiber::sleep_for(100ms);

	temp.setUpdateRate(200);
	temp.enableExtendedMode();

	temp.configureAlertMode(
			modm::tmp102::ThermostatMode::Comparator,
			modm::tmp102::AlertPolarity::ActiveLow,
			modm::tmp102::FaultQueue::Faults6);
	temp.setLowerLimit(28.f);
	temp.setUpperLimit(30.f);

	bool result{};
	while (true)
	{
		temp.readComparatorMode(result);
		float temperature = temperatureData.getTemperature();
		uint8_t tI = (int) temperature;
		uint16_t tP = (temperature - tI) * 10000;

		stream << "T= " << tI << ".";
		if (tP == 0) stream << "0000 C";
		else if (tP == 625) stream << "0" << tP << " C";
		else stream << tP << " C";
		stream << modm::endl;
		if (result) stream << "Heat me up!" << modm::endl;

		modm::this_fiber::sleep_for(200ms);
		Board::LedRed::toggle();
	}
});

modm::Fiber fiber_blink([]
{
	Board::LedOrange::setOutput();
	while(true)
	{
		Board::LedOrange::toggle();
		modm::this_fiber::sleep_for(0.5s);
	}
});

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	Usart2::connect<GpioA2::Tx>();
	Usart2::initialize<Board::SystemClock, 38400_Bd>();

	MyI2cMaster::connect<GpioB7::Sda, GpioB8::Scl>(MyI2cMaster::PullUps::Internal);
	MyI2cMaster::initialize<Board::SystemClock, 100_kHz>();

	stream << "\n\nRESTART\n\n";

	modm::fiber::Scheduler::run();
	return 0;
}
