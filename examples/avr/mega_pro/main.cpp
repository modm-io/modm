/*
 * Copyright (c) 2011, Fabian Greif
 * Copyright (c) 2013-2014, 2017-2018, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
using namespace std::chrono_literals;
#include <math.h>

// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::DEBUG

int
main()
{
	Board::initialize();
	Board::LedD13::setOutput();

	// Use the logging streams to print some messages.
	// Change MODM_LOG_LEVEL above to enable or disable these messages
	MODM_LOG_DEBUG   << "debug"   << modm::endl;
	MODM_LOG_INFO    << "info"    << modm::endl;
	MODM_LOG_WARNING << "warning" << modm::endl;
	MODM_LOG_ERROR   << "error"   << modm::endl;

	uint8_t counter{0};
	while (true)
	{
		MODM_LOG_INFO << "Loop " << counter++ << modm::endl;
		Board::LedD13::toggle();

		MODM_LOG_INFO << "sin " << (int)sinf(counter) << modm::endl;
		modm::delay(1s);
	}
}
