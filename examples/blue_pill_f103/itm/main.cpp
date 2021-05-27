/*
 * Copyright (c) 2016, 2021, Sascha Schade
 * Copyright (c) 2017, Niklas Hauser
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
#include <modm/debug/logger.hpp>

using namespace Board;

modm::IODeviceWrapper<Itm, modm::IOBuffer::DiscardIfFull> itm_device;
modm::IOStream stream(itm_device);

/*
 * Blinks the green user LED with 1 Hz and outputs the loop counter over ITM.
 * SWO pin is PB3. openocd > 0.11.0 is required. Receive data with:
 * scons log-itm fcpu=72000000
 */

int
main()
{
	Board::initialize();
	Itm::initialize();

	stream << "Hello from the SWO." << modm::endl;
	stream << "debug"   << modm::endl;
	stream << "info"    << modm::endl;
	stream << "warning" << modm::endl;
	stream << "error"   << modm::endl;

	LedGreen::set();

	while (true)
	{
		static modm::PeriodicTimer tmr{500ms};

		if (tmr.execute())
		{
			LedGreen::toggle();

			static uint32_t counter{0};
			stream << "loop: " << counter++ << modm::endl;
		}
		Itm::update();
	}

	return 0;
}
