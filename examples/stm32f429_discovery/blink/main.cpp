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
	usb::Vbus::setOutput(modm::Gpio::Low);
	usb::Overcurrent::setOutput(modm::Gpio::Low);

	// Use the logging streams to print some messages.
	// Change MODM_LOG_LEVEL above to enable or disable these messages
	MODM_LOG_DEBUG   << "debug"   << modm::endl;
	MODM_LOG_INFO    << "info"    << modm::endl;
	MODM_LOG_WARNING << "warning" << modm::endl;
	MODM_LOG_ERROR   << "error"   << modm::endl;

	uint32_t counter(0);

	while (true)
	{
		LedRed::toggle();
		LedGreen::toggle();

		modm::delay(Button::read() ? 125ms : 500ms);

		usb::Vbus::toggle();
		usb::Overcurrent::toggle();

		modm::delay(Button::read() ? 125ms : 500ms);

		MODM_LOG_INFO << "loop: " << counter++ << modm::endl;
	}

	return 0;
}
