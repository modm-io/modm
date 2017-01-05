/*
 * Copyright (c) 2011, Fabian Greif
 * Copyright (c) 2013, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/architecture/architecture.hpp>
#include <modm/debug/logger.hpp>

// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::DEBUG

int
main()
{
	// Use the logging streams to print some messages.
	// Change MODM_LOG_LEVEL above to enable or disable these messages 
	MODM_LOG_DEBUG   << MODM_FILE_INFO << "debug"   << modm::endl;
	MODM_LOG_INFO    << MODM_FILE_INFO << "info"    << modm::endl;
	MODM_LOG_WARNING << MODM_FILE_INFO << "warning" << modm::endl;
	MODM_LOG_ERROR   << MODM_FILE_INFO << "error"   << modm::endl;
	
	return 0;
}
