/*
 * Copyright (c) 2015-2016, Niklas Hauser
 * Copyright (c) 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/architecture/platform.hpp>
#include "calc.hpp"

using namespace Board;

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	LedJ13::setOutput(xpcc::Gpio::Low);
	LedJ5::setOutput(xpcc::Gpio::Low);
	LedA12::setOutput(xpcc::Gpio::Low);
	LedD4::setOutput(xpcc::Gpio::Low);

	// Use the logging streams to print some messages.
	// Change XPCC_LOG_LEVEL above to enable or disable these messages
	XPCC_LOG_DEBUG   << "debug"   << xpcc::endl;
	XPCC_LOG_INFO    << "info"    << xpcc::endl;
	XPCC_LOG_WARNING << "warning" << xpcc::endl;
	XPCC_LOG_ERROR   << "error"   << xpcc::endl;

	uint32_t counter(0);

	while (true)
	{
		xpcc::delayMilliseconds(Button::read() ? 250 : 1000);
		LedJ13::toggle();
		LedJ5::toggle();
		LedA12::toggle();
		LedD4::toggle();

		XPCC_LOG_INFO << "loop: " << counter++ << xpcc::endl;
		XPCC_LOG_INFO << "double: " << Calc::get() << xpcc::endl;

		Calc::calc();
	}

	return 0;
}
