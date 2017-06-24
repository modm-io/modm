/*
 * Copyright (c) 2015-2017, Niklas Hauser
 * Copyright (c) 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board/board.hpp>
#include "calc.hpp"

using namespace Board;

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	LedJ13::setOutput(modm::Gpio::Low);
	LedJ5::setOutput(modm::Gpio::Low);
	LedA12::setOutput(modm::Gpio::Low);
	LedD4::setOutput(modm::Gpio::Low);

	// Use the logging streams to print some messages.
	// Change MODM_LOG_LEVEL above to enable or disable these messages
	MODM_LOG_DEBUG   << "debug"   << modm::endl;
	MODM_LOG_INFO    << "info"    << modm::endl;
	MODM_LOG_WARNING << "warning" << modm::endl;
	MODM_LOG_ERROR   << "error"   << modm::endl;

	uint32_t counter(0);

	while (true)
	{
		modm::delayMilliseconds(Button::read() ? 250 : 1000);
		LedJ13::toggle();
		LedJ5::toggle();
		LedA12::toggle();
		LedD4::toggle();

		MODM_LOG_INFO << "loop: " << counter++ << modm::endl;
		MODM_LOG_INFO << "double: " << Calc::get() << modm::endl;

		Calc::calc();
	}

	return 0;
}
