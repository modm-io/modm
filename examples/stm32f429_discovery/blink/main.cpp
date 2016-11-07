/*
 * Copyright (c) 2015-2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <xpcc/architecture/platform.hpp>

using namespace Board;

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	LedRed::set();
	usb::VBus::setOutput(xpcc::Gpio::Low);
	usb::Overcurrent::setOutput(xpcc::Gpio::Low);

	while (1)
	{
		LedRed::toggle();
		LedGreen::toggle();

		xpcc::delayMilliseconds(Button::read() ? 125 : 500);

		usb::VBus::toggle();
		usb::Overcurrent::toggle();

		xpcc::delayMilliseconds(Button::read() ? 125 : 500);
	}

	return 0;
}
