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

#include <modm/architecture.hpp>
#include <modm/debug/logger.hpp>

// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::DEBUG

int
main()
{
	// Use the logging streams to print some messages.
	// Change XPCC_LOG_LEVEL above to enable or disable these messages 
	XPCC_LOG_DEBUG   << XPCC_FILE_INFO << "debug"   << xpcc::endl;
	XPCC_LOG_INFO    << XPCC_FILE_INFO << "info"    << xpcc::endl;
	XPCC_LOG_WARNING << XPCC_FILE_INFO << "warning" << xpcc::endl;
	XPCC_LOG_ERROR   << XPCC_FILE_INFO << "error"   << xpcc::endl;
	
	return 0;
}
