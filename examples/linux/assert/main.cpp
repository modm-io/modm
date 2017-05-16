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

#include <modm/architecture/platform.hpp>

#define MODM_CAN_MODULE_NAME "can"
#define MODM_IOBUFFER_MODULE_NAME "iobuffer"
#define MODM_UART_MODULE_NAME "uart"

static modm::Abandonment
test_assertion_handler(const char * module,
					   const char * /* location */,
					   const char * /* failure */,
					   uintptr_t /* context */)
{
	if (strcmp(module, MODM_IOBUFFER_MODULE_NAME) == 0)
		return modm::Abandonment::Ignore;
	return modm::Abandonment::DontCare;
}
MODM_ASSERTION_HANDLER(test_assertion_handler);

// ----------------------------------------------------------------------------
int
main()
{
	modm_assert(true, MODM_CAN_MODULE_NAME, "init", "timeout");

	modm_assert_debug(false, MODM_IOBUFFER_MODULE_NAME, "tx", "full");

	modm_assert(false, MODM_UART_MODULE_NAME, "init", "mode");

	return 0;
}
