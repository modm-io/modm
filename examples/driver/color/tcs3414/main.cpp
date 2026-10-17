/*
 * Copyright (c) 2014, Sascha Schade
 * Copyright (c) 2014-2018, Niklas Hauser
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
#include <modm/driver/color/tcs3414.hpp>

/**
 * Example to demonstrate a MODM driver for colour sensor TCS3414
 *
 * This example uses I2cMaster2 of STM32F407
 *
 * SDA PB11
 * SCL PB10
 *
 * GND and +3V3 are connected to the colour sensor.
 *
 */

// typedef I2cMaster1 MyI2cMaster;
typedef I2cMaster2 MyI2cMaster;
modm::tcs3414::Data data;
modm::Tcs3414<MyI2cMaster> sensor{data};

modm::Fiber fiber_sensor([]
{
	MODM_LOG_INFO << "Ping the device from ThreadOne" << modm::endl;
	while (not sensor.ping()) modm::this_fiber::sleep_for(100ms);
	MODM_LOG_INFO << "Device responded" << modm::endl;

	sensor.initialize();
	MODM_LOG_INFO << "Device initialized" << modm::endl;

	sensor.configure(modm::tcs3414::Gain::X16, modm::tcs3414::Prescaler::D1);
	sensor.setIntegrationTime(modm::tcs3414::IntegrationMode::INTERNAL,
							  modm::tcs3414::NominalIntegrationTime::MSEC_100);
	MODM_LOG_INFO << "Device configured" << modm::endl;

	while (true)
	{
		if (sensor.readColor()) {
			const auto rgb = data.getColor();
			MODM_LOG_INFO << "RGB: " << rgb << "\tHSV: " << modm::color::Hsv(rgb) << modm::endl;
		}
		modm::this_fiber::sleep_for(500ms);
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

	MyI2cMaster::connect<GpioB11::Sda, GpioB10::Scl>();
	MyI2cMaster::initialize<Board::SystemClock, 100_kHz>();

	MODM_LOG_INFO << "\n\nWelcome to TCS3414 demo!\n\n";

	modm::fiber::Scheduler::run();
	return 0;
}
