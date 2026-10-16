/*
 * Copyright (c) 2011, Fabian Greif
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2013-2017, Niklas Hauser
 * Copyright (c) 2014, 2016, Sascha Schade
 * Copyright (c) 2022, Andrey Kunitsyn
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/debug/logger.hpp>

// ----------------------------------------------------------------------------
// Set the log level
#undef MODM_LOG_LEVEL
#define MODM_LOG_LEVEL modm::log::DEBUG

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	// Use the logging streams to print some messages.
	// Change MODM_LOG_LEVEL above to enable or disable these messages
	MODM_LOG_DEBUG   << MODM_FILE_INFO << "debug"   << modm::endl;
	MODM_LOG_INFO    << MODM_FILE_INFO << "info"    << modm::endl;
	MODM_LOG_WARNING << MODM_FILE_INFO << "warning" << modm::endl;
	MODM_LOG_ERROR   << MODM_FILE_INFO << "error"   << modm::endl;

	uint32_t uptime{};
	while (true)
	{
		Board::Leds::toggle();
		modm::delay(1s);
		MODM_LOG_INFO << "Seconds since reboot: " << ++uptime << modm::endl;
	}
	return 0;
}
