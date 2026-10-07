/*
 * Copyright (c) 2014, Sascha Schade
 * Copyright (c) 2014-2018, 2021 Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/driver/color/tcs3472.hpp>
#include <modm/processing.hpp>

modm::tcs3472::Data data;
modm::Tcs3472<I2cMaster1> sensor{data};

modm::Fiber fiber_sensor([]
{
	MODM_LOG_INFO << "Ping the device from ThreadOne" << modm::endl;

	// ping the device until it responds
	while (not sensor.ping()) modm::this_fiber::sleep_for(100ms);
	MODM_LOG_INFO << "Device responded" << modm::endl;

	while (not sensor.initialize()) modm::this_fiber::sleep_for(100ms);
	MODM_LOG_INFO << "Device initialized" << modm::endl;

	while (not sensor.configure(sensor.Gain::X4, sensor.IntegrationTime::MSEC_101))
		modm::this_fiber::sleep_for(100ms);
	MODM_LOG_INFO << "Device configured" << modm::endl;

	while (true)
	{
		if (sensor.readColor())
		{
			const auto rgb = data.getColor();
			MODM_LOG_INFO << "RGB: " << rgb << "\tHSV: " << modm::color::Hsv(rgb) << modm::endl;
		}
		modm::this_fiber::sleep_for(500ms);
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

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	I2cMaster1::connect<Board::D14::Sda, Board::D15::Scl>();
	I2cMaster1::initialize<Board::SystemClock, 100_kHz>();

	MODM_LOG_INFO << "\n\nWelcome to TCS3472 demo!\n\n";

	modm::fiber::Scheduler::run();
	return 0;
}
