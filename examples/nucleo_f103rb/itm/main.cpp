/*
 * Copyright (c) 2019, Niklas Hauser
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

using namespace Board;

modm::IODeviceWrapper<Itm, modm::IOBuffer::DiscardIfFull> itm_device;
modm::IOStream stream(itm_device);

int
main()
{
	Board::initialize();
	Itm::initialize();
	LedD13::setOutput();

	stream << "Hello from the SWO." << modm::endl;
	stream << "debug"   << modm::endl;
	stream << "info"    << modm::endl;
	stream << "warning" << modm::endl;
	stream << "error"   << modm::endl;


	while (true)
	{
		static modm::PeriodicTimer tmr{500ms};
		if (tmr.execute())
		{
			tmr.restart(Button::read() ? 100ms : 500ms);
			LedD13::toggle();

			static uint32_t counter{0};
			stream << "loop: " << counter++ << modm::endl;
		}
		Itm::update();
	}

	return 0;
}
