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

#define XPCC_CAN_MODULE_NAME "can"
#define XPCC_IOBUFFER_MODULE_NAME "iobuffer"
#define XPCC_UART_MODULE_NAME "uart"

using namespace Board;

extern "C" void xpcc_abandon(const char * module,
							 const char * location,
							 const char * failure)
{
	XPCC_LOG_ERROR << "Assertion '"
			<< module << "." << location << "." << failure
			<< "' failed! Abandoning." << xpcc::endl;

	LedGreen::setOutput();
	while(1) {
		LedBlue::set();
		xpcc::delayMilliseconds(20);
		LedBlue::reset();
		xpcc::delayMilliseconds(180);
	}
}

static xpcc::Abandonment
test_assertion_handler(const char * module,
					   const char * /* location */,
					   const char * /* failure */)
{
	if (strcmp(module, XPCC_IOBUFFER_MODULE_NAME) == 0)
		return xpcc::Abandonment::Ignore;
	return xpcc::Abandonment::DontCare;
}
XPCC_ASSERTION_HANDLER(test_assertion_handler);

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	xpcc_assert(true, XPCC_CAN_MODULE_NAME, "init", "timeout");

	xpcc_assert_debug(false, XPCC_IOBUFFER_MODULE_NAME, "tx", "full");

	xpcc_assert(false, XPCC_UART_MODULE_NAME, "init", "mode");

	while(1)
	{
		LedRed::toggle();
		xpcc::delayMilliseconds(500);
	}

	return 0;
}
