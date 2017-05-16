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

using namespace Board;

static modm::Abandonment
test_assertion_handler(const char * module,
					   const char * /* location */,
					   const char * /* failure */,
					   uintptr_t /* context */)
{
	if (!strcmp(module, "iobuffer")) {
		MODM_LOG_ERROR << "Ignoring iobuffer full!" << modm::endl;
		return modm::Abandonment::Ignore;
	}
	return modm::Abandonment::DontCare;
}

static modm::Abandonment
core_assertion_handler(const char * module,
					   const char * /* location */,
					   const char * failure,
					   uintptr_t context)
{
	if (!memcmp(module, "core\0nvic\0undefined", 19)) {
		MODM_LOG_ERROR.printf("Ignoring undefined IRQ handler %d!\n", context);
		return modm::Abandonment::Ignore;
	}
	if (!memcmp(module, "core\0heap", 9)) {
		MODM_LOG_ERROR.printf("Ignoring 'core.heap.%s' of size 0x%x!\n", failure, context);
		return modm::Abandonment::Ignore;
	}
	return modm::Abandonment::DontCare;
}
MODM_ASSERTION_HANDLER(core_assertion_handler);

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	// trigger an IRQ with undefined handler
	NVIC_EnableIRQ(RTC_Alarm_IRQn);
	NVIC_SetPendingIRQ(RTC_Alarm_IRQn);

	// trigger an out of memory
	// we definitely don't have 32MB RAM on this board
	// returns NULL, asserts in debug mode
	volatile void * ptr = malloc(1 << 25);
	// returns NULL, asserts in debug mode
	ptr = new (std::nothrow) uint8_t[1 << 25];
	// always asserts
	ptr = new uint8_t[1 << 25];
	(void) ptr;

	// does not fail, should not be optimized away
	volatile bool true_condition = true;
	modm_assert(true_condition, "can", "init", "timeout");

	// only fails for debug builds, but is ignored anyways
	modm_assert_debug(false, "iobuffer", "tx", "full");

	// "accidentally" return from main, without even returning properly!
	// This should be cought by the debug assert core.main.exit!
	// while(1) ;
	// return 0;
}
