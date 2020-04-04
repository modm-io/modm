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

#include <modm/board.hpp>
#include <modm/architecture/interface/assert.hpp>

using modm::accessor::asFlash;

// Flash support on avr-gcc is so horribly broken.
#define IFS(s) asFlash(PSTR(s))

extern "C" void
modm_abandon(const modm::AssertionInfo &info)
{
	serialStream << IFS("Assertion '") << asFlash(info.name) << '\'';
	if (info.context != uintptr_t(-1))
		serialStream << IFS(" @ ") << (void *)info.context << IFS(" (") << (uint32_t)info.context << IFS(")");
#if MODM_ASSERTION_INFO_HAS_DESCRIPTION
	serialStream << IFS(" failed!\n  ") << asFlash(info.description) << IFS("\nAbandoning...\n");
#else
	serialStream << IFS(" failed!\nAbandoning...\n");
#endif

	Leds::setOutput();
	while (true) {
		Leds::write(1);
		modm::delay(20ms);
		Leds::write(0);
		modm::delay(180ms);
	}
}


static modm::Abandonment
test_assertion_handler(const modm::AssertionInfo &info)
{
	serialStream << IFS("#1: '") << asFlash(info.name) << IFS("'!") << modm::endl;
	// The strings are located in FLASH!!!
	if (strncmp_P("io.", info.name, 3) == 0) {
		serialStream << IFS("Ignoring assertion!") << modm::endl;
		return modm::Abandonment::Ignore;
	}
	return modm::Abandonment::DontCare;
}
MODM_ASSERTION_HANDLER(test_assertion_handler);

static modm::Abandonment
test_assertion_handler2(const modm::AssertionInfo &info)
{
	serialStream << IFS("#2: '") << asFlash(info.name) << IFS("'!") << modm::endl;
	return modm::Abandonment::DontCare;
}
MODM_ASSERTION_HANDLER(test_assertion_handler2);

static modm::Abandonment
test_assertion_handler3(const modm::AssertionInfo &info)
{
	serialStream << IFS("#3: '") << asFlash(info.name) << IFS("'!") << modm::endl;
	return modm::Abandonment::DontCare;
}
MODM_ASSERTION_HANDLER(test_assertion_handler3);


// ----------------------------------------------------------------------------
int main()
{
	Board::initialize();
	Leds::setOutput();
	serialStream << IFS("Starting test...\n");

	// only fails for debug builds, but is ignored anyways
	modm_assert_continue_fail(false, "io.tx",
			"The IO transmit buffer is full!");

	modm_assert_continue_fail_debug(false, "uart.init", "UART init failed!");

	modm_assert(false, "can.init", "CAN init timed out!");

	while (true)
	{
		LedD13::toggle();
		modm::delay(500ms);
	}
}
