/*
 * Copyright (c) 2011, Fabian Greif
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2013-2017, Niklas Hauser
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
#include <modm/processing.hpp>
// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	// Use the logging streams to print some messages.
	// Change MODM_LOG_LEVEL above to enable or disable these messages
	MODM_LOG_DEBUG   << "debug"   << modm::endl;
	MODM_LOG_INFO    << "info"    << modm::endl;
	MODM_LOG_WARNING << "warning" << modm::endl;
	MODM_LOG_ERROR   << "error"   << modm::endl;

	uint32_t ii(1);
	modm::Timeout timeout;

	while (true)
	{
		Board::LedBlue::set();
		timeout.restart(100ms);
		while(not timeout.isExpired()) ;

		Board::LedBlue::reset();
		timeout.restart(900ms);
		while(not timeout.isExpired()) ;
		MODM_LOG_INFO << "Seconds since reboot: " << ii++ << modm::endl;
	}

	return 0;
}
