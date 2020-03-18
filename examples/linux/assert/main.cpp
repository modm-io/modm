/*
 * Copyright (c) 2016-2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/platform.hpp>
#include <modm/architecture/interface/assert.hpp>
#include <modm/debug.hpp>

static modm::Abandonment
test_assertion_handler(const modm::AssertionInfo &info)
{
	if (strncmp(info.name, "io.", 3) == 0) {
		MODM_LOG_WARNING << "Ignoring all 'io.*' assertions!" << modm::endl;
		return modm::Abandonment::Ignore;
	}
	return modm::Abandonment::DontCare;
}
MODM_ASSERTION_HANDLER(test_assertion_handler);

static modm::Abandonment
log_assertion_handler(const modm::AssertionInfo &info)
{
	MODM_LOG_DEBUG.printf("Assertion '%s' failed!\n", info.name);
	return modm::Abandonment::DontCare;
}
MODM_ASSERTION_HANDLER_DEBUG(log_assertion_handler);

// ----------------------------------------------------------------------------
int
main()
{
	modm_assert_continue_fail_debug(false, "io.tx", "IO transmit buffer is full!");

	modm_assert_continue_fail_debug(false, "uart.init", "UART init failed!");

	modm_assert(false, "can.init", "CAN init timed out!");

	return 0;
}
