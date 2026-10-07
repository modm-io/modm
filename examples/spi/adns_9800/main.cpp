/*
 * Copyright (c) 2011, Fabian Greif
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2013-2017, Niklas Hauser
 * Copyright (c) 2014, 2016, 2018, Sascha Schade
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

#include <modm/driver/motion/adns9800.hpp>

#include <inttypes.h>

// ----------------------------------------------------------------------------
modm::Fiber fiber_blink([]
{
	modm::ShortTimeout timeout(100ms);
	uint32_t uptime{};

	while (true)
	{
		Board::LedGreen::reset();

		timeout.wait();
		timeout.restart(100ms);

		Board::LedGreen::set();

		timeout.wait();
		timeout.restart(4.9s);

		MODM_LOG_INFO << "Seconds since reboot: " << uptime << modm::endl;

		uptime += 5;
	}
});


using Cs = GpioOutputA4;
using Adns9800 = modm::Adns9800<
	/* Spi = */ SpiMaster1,
	/* Ncs = */ Cs >;

modm::Fiber fiber_adns9800([]
{
	modm::ShortPeriodicTimer timer(10ms);
	int32_t x{}, y{};

	Cs::setOutput(modm::Gpio::High);

	SpiMaster1::connect<GpioOutputA7::Mosi, GpioOutputA5::Sck, GpioInputA6::Miso>();
	SpiMaster1::initialize<Board::SystemClock, 2.25_MHz>();
	SpiMaster1::setDataMode(SpiMaster1::DataMode::Mode3);

	Adns9800::initialise();

	while (true)
	{
		timer.wait();

		{
			int16_t delta_x, delta_y;
			Adns9800::getDeltaXY(delta_x, delta_y);
			MODM_LOG_INFO.printf(
					"dx = %5"PRId16", dy = %5"PRId16"; x = %9"PRId32", y=%9"PRId32"\n",
					delta_x, delta_y, x, y);

			x += delta_x;
			y += delta_y;
		}
	}
});


// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	MODM_LOG_INFO << "Welcome to ADNS 9800 demo." << modm::endl;

	modm::fiber::Scheduler::run();
	return 0;
}
