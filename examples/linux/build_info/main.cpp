/*
 * Copyright (c) 2014, Kevin Laeufer
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

#include <xpcc_build_info.hpp>

// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::INFO

int
main()
{
	// Let's print some information that is provided in the xpcc_build_info.hpp
	XPCC_LOG_INFO << "Machine:  " << XPCC_BUILD_MACHINE  << xpcc::endl;
	XPCC_LOG_INFO << "User:     " << XPCC_BUILD_USER     << xpcc::endl;
	XPCC_LOG_INFO << "Os:       " << XPCC_BUILD_OS       << xpcc::endl;
	XPCC_LOG_INFO << "Compiler: " << XPCC_BUILD_COMPILER << xpcc::endl;

	return 0;
}
