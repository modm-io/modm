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
#include <avr/pgmspace.h>

using modm::accessor::asFlash;

// Flash support on avr-gcc is so horribly broken.
#define IFS(s) asFlash(PSTR(s))

#define MODM_CAN_MODULE_NAME 		"can"
#define MODM_IOBUFFER_MODULE_NAME 	"iobuffer"
#define MODM_UART_MODULE_NAME 		"uart"

extern "C" void
modm_abandon(const char * module,
			 const char * location,
			 const char * failure)
{
	serialStream << IFS("Assertion '")
			<< asFlash(module) << '.'
			<< asFlash(location) << '.'
			<< asFlash(failure)
			<< IFS("' failed! Abandoning.") << modm::endl;

	Leds::setOutput();
	while(1) {
		Leds::write(1);
		modm::delayMilliseconds(20);
		Leds::write(0);
		modm::delayMilliseconds(180);
	}
}

static modm::Abandonment
test_assertion_handler(const char * module,
					   const char * /* location */,
					   const char * /* failure */)
{
	serialStream << IFS("#1: '") << asFlash(module) << IFS("'!") << modm::endl;
	// The strings are located in FLASH!!!
	if (strcmp_P(module, PSTR(MODM_IOBUFFER_MODULE_NAME)) == 0)
		return modm::Abandonment::Ignore;
	return modm::Abandonment::DontCare;
}
MODM_ASSERTION_HANDLER(test_assertion_handler);

static modm::Abandonment
test_assertion_handler2(const char * /* module */,
						const char * location,
						const char * /* failure */)
{
	serialStream << IFS("#2: '") << asFlash(location) << IFS("'!") << modm::endl;
	return modm::Abandonment::DontCare;
}
MODM_ASSERTION_HANDLER(test_assertion_handler2);

static modm::Abandonment
test_assertion_handler3(const char * /* module */,
						const char * /* location */,
						const char * failure)
{
	serialStream << IFS("#3: '") << asFlash(failure) << IFS("'!") << modm::endl;
	return modm::Abandonment::DontCare;
}
MODM_ASSERTION_HANDLER(test_assertion_handler3);

// ----------------------------------------------------------------------------
int main()
{
	Board::initialize();
	Leds::setOutput();

	modm_assert(true, MODM_CAN_MODULE_NAME, "init", "timeout");

	modm_assert_debug(false, MODM_IOBUFFER_MODULE_NAME, "tx", "full");

	modm_assert(false, MODM_UART_MODULE_NAME, "init", "mode");

	while (1)
	{
		Led3::toggle();
		modm::delayMilliseconds(500);
	}
}
