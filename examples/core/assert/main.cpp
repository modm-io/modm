/*
 * Copyright (c) 2016-2017, Niklas Hauser
 * Copyright (c) 2017, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#if __has_include(<modm/board.hpp>)
#include <modm/board.hpp>
#else
#include <modm/platform.hpp>
#include <modm/debug.hpp>
#endif
#include <modm/architecture/interface/assert.hpp>
#include <string.h>

#ifdef MODM_CPU_AVR
// The assertion names are located in Flash on AVRs!
#define NAME(info) modm::accessor::asFlash(info.name)
#else
#define NAME(info) info.name
#endif

static bool
startsWith(const char *name, const char *prefix)
{
#ifdef MODM_CPU_AVR
	return strncmp_P(prefix, name, strlen(prefix)) == 0;
#else
	return strncmp(name, prefix, strlen(prefix)) == 0;
#endif
}

static modm::Abandonment
io_assertion_handler(const modm::AssertionInfo &info)
{
	if (startsWith(info.name, "io.")) {
		MODM_LOG_WARNING << "Ignoring all 'io.*' assertions!" << modm::endl;
		return modm::Abandonment::Ignore;
	}
	return modm::Abandonment::DontCare;
}
MODM_ASSERTION_HANDLER(io_assertion_handler);

static modm::Abandonment
log_assertion_handler(const modm::AssertionInfo &info)
{
	MODM_LOG_DEBUG << "Assertion '" << NAME(info) << "' failed!" << modm::endl;
	return modm::Abandonment::DontCare;
}
MODM_ASSERTION_HANDLER_DEBUG(log_assertion_handler);

// ----------------------------------------------------------------------------
int
main()
{
#if __has_include(<modm/board.hpp>)
	Board::initialize();
#endif
	MODM_LOG_INFO << "Starting test..." << modm::endl;

	// only fails for debug builds, but is ignored anyways
	modm_assert_continue_fail_debug(false, "io.tx", "IO transmit buffer is full!");

	modm_assert_continue_fail_debug(false, "uart.init", "UART init failed!");

	// does not fail, should not be optimized away
	volatile bool true_condition = true;
	modm_assert(true_condition, "can.init", "CAN init timed out!");

	// always fails and abandons execution
	modm_assert(false, "can.init", "CAN init timed out!");

	return 0;
}
