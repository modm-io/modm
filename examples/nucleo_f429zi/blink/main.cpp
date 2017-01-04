/*
 * Copyright (c) 2016, Niklas Hauser
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

int
main()
{
	Board::initialize();
	Leds::setOutput();

	// Use the logging streams to print some messages.
	// Change XPCC_LOG_LEVEL above to enable or disable these messages
	XPCC_LOG_DEBUG   << "debug"   << xpcc::endl;
	XPCC_LOG_INFO    << "info"    << xpcc::endl;
	XPCC_LOG_WARNING << "warning" << xpcc::endl;
	XPCC_LOG_ERROR   << "error"   << xpcc::endl;

	uint32_t counter(0);

	while (1)
	{
		Leds::write(1 << (counter % Leds::width));
		xpcc::delayMilliseconds(Button::read() ? 100 : 500);

		XPCC_LOG_INFO << "loop: " << counter++ << xpcc::endl;
	}

	return 0;
}
