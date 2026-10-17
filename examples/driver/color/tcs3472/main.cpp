/*
 * Copyright (c) 2014, Sascha Schade
 * Copyright (c) 2014-2018, 2021 Niklas Hauser
 * Copyright (c) 2021, Thomas Sommer
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

using namespace modm::platform;

#ifdef MODM_CPU_AVR
using SensorI2c = I2cMaster;
#else
using SensorI2c = Board::i2c::Controller;
#endif

modm::Fiber fiber_blink([]
{
	Board::LedD13::setOutput();
	while (true)
	{
		Board::LedD13::toggle();
		modm::this_fiber::sleep_for(0.5s);
	}
});

modm::tcs3472::Data data;
modm::Tcs3472<SensorI2c> sensor{data};
using TCS3472_INT = Board::D2;
modm::Fiber fiber_sensor([]
{
	TCS3472_INT::setInput(Gpio::InputType::PullUp);

	MODM_LOG_INFO << "Ping TCS34725" << modm::endl;
	// ping the device until it responds
	while (not sensor.ping()) modm::this_fiber::sleep_for(100ms);
	MODM_LOG_INFO << "TCS34725 responded" << modm::endl;

	sensor.initialize(sensor.Enable_InterruptMode_Waittime);
	sensor.configure(modm::tcs3472::Gain::X16, modm::tcs3472::IntegrationTime::MSEC_2_4);
	sensor.setInterruptPersistenceFilter(modm::tcs3472::InterruptPersistence::CNT_20);
	// Setup WaitTime to further slow down samplerate
	sensor.setWaitTime(modm::tcs3472::WaitTime::MSEC_2_4);

	// Dummy read required
	sensor.readColor();
	// Fetch one sample ...
	sensor.readColor();
	// ...and set the high threshold 20% above current clear
	sensor.setInterruptHighThreshold(data.getClear() * 1.2);

	while (true)
	{
		// wait until the interrupt pin signals a brighter color
		sensor.reloadInterrupt();
		modm::this_fiber::poll([]{ return not TCS3472_INT::read(); });
		if (sensor.readColor())
		{
			const auto rgb = data.getColor();
			MODM_LOG_INFO << "RGB: " << rgb << "\tHSV: " << modm::color::Hsv(rgb) << modm::endl;
		}
	}
});

int
main()
{
	Board::initialize();
#ifdef MODM_CPU_AVR
	SensorI2c::initialize<Board::SystemClock, 100_kHz>();
#else
	Board::initializeI2c();
#endif

	MODM_LOG_INFO << "Welcome to the TCS3472 demo!" << modm::endl;

	modm::fiber::Scheduler::run();
	return 0;
}
