/*
 * Copyright (c) 2015-2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>

using namespace Board;

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	LedRed::set();
	usb::VBus::setOutput(modm::Gpio::Low);
	usb::Overcurrent::setOutput(modm::Gpio::Low);

	while (true)
	{
		LedRed::toggle();
		LedGreen::toggle();

		modm::delay(Button::read() ? 125ms : 500ms);

		usb::VBus::toggle();
		usb::Overcurrent::toggle();

		modm::delay(Button::read() ? 125ms : 500ms);
	}

	return 0;
}
