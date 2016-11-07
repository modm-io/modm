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

#include <xpcc/architecture/platform.hpp>
#include <avr/pgmspace.h>

using xpcc::accessor::asFlash;

// Flash support on avr-gcc is so horribly broken.
#define IFS(s) asFlash(PSTR(s))

#define XPCC_CAN_MODULE_NAME 		"can"
#define XPCC_IOBUFFER_MODULE_NAME 	"iobuffer"
#define XPCC_UART_MODULE_NAME 		"uart"

extern "C" void
xpcc_abandon(const char * module,
			 const char * location,
			 const char * failure)
{
	serialStream << IFS("Assertion '")
			<< asFlash(module) << '.'
			<< asFlash(location) << '.'
			<< asFlash(failure)
			<< IFS("' failed! Abandoning.") << xpcc::endl;

	Leds::setOutput();
	while(1) {
		Leds::write(1);
		xpcc::delayMilliseconds(20);
		Leds::write(0);
		xpcc::delayMilliseconds(180);
	}
}

static xpcc::Abandonment
test_assertion_handler(const char * module,
					   const char * /* location */,
					   const char * /* failure */)
{
	serialStream << IFS("#1: '") << asFlash(module) << IFS("'!") << xpcc::endl;
	// The strings are located in FLASH!!!
	if (strcmp_P(module, PSTR(XPCC_IOBUFFER_MODULE_NAME)) == 0)
		return xpcc::Abandonment::Ignore;
	return xpcc::Abandonment::DontCare;
}
XPCC_ASSERTION_HANDLER(test_assertion_handler);

static xpcc::Abandonment
test_assertion_handler2(const char * /* module */,
						const char * location,
						const char * /* failure */)
{
	serialStream << IFS("#2: '") << asFlash(location) << IFS("'!") << xpcc::endl;
	return xpcc::Abandonment::DontCare;
}
XPCC_ASSERTION_HANDLER(test_assertion_handler2);

static xpcc::Abandonment
test_assertion_handler3(const char * /* module */,
						const char * /* location */,
						const char * failure)
{
	serialStream << IFS("#3: '") << asFlash(failure) << IFS("'!") << xpcc::endl;
	return xpcc::Abandonment::DontCare;
}
XPCC_ASSERTION_HANDLER(test_assertion_handler3);

// ----------------------------------------------------------------------------
int main()
{
	Board::initialize();
	Leds::setOutput();

	xpcc_assert(true, XPCC_CAN_MODULE_NAME, "init", "timeout");

	xpcc_assert_debug(false, XPCC_IOBUFFER_MODULE_NAME, "tx", "full");

	xpcc_assert(false, XPCC_UART_MODULE_NAME, "init", "mode");

	while (1)
	{
		Led3::toggle();
		xpcc::delayMilliseconds(500);
	}
}
