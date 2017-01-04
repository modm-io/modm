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

using namespace Board;

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	LedD13::setOutput(xpcc::Gpio::Low);

	// Use the logging streams to print some messages.
	// Change XPCC_LOG_LEVEL above to enable or disable these messages
	XPCC_LOG_DEBUG   << "debug"   << xpcc::endl;
	XPCC_LOG_INFO    << "info"    << xpcc::endl;
	XPCC_LOG_WARNING << "warning" << xpcc::endl;
	XPCC_LOG_ERROR   << "error"   << xpcc::endl;

	uint32_t counter(0);

	while (1)
	{
		xpcc::delayMilliseconds(Button::read() ? 250 : 1000);
		LedD13::toggle();

		XPCC_LOG_INFO << "loop: " << counter++ << xpcc::endl;
	}

	return 0;
}
