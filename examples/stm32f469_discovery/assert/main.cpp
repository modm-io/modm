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

#include <modm/board.hpp>
#include <modm/architecture/interface/assert.hpp>
#include <string>

using namespace std::string_literals;
using namespace Board;

static modm::Abandonment
test_assertion_handler(const modm::AssertionInfo &info)
{
	if (not strncmp(info.name, "io.", 3)) {
		MODM_LOG_ERROR << "Ignoring all io.* assertions!" << modm::endl;
		return modm::Abandonment::Ignore;
	}
	return modm::Abandonment::DontCare;
}
MODM_ASSERTION_HANDLER(test_assertion_handler);

static modm::Abandonment
core_assertion_handler(const modm::AssertionInfo &info)
{
	if (info.name == "nvic.undef"s) {
		MODM_LOG_ERROR.printf("Ignoring undefined IRQ handler %d!\n", info.context);
		return modm::Abandonment::Ignore;
	}
	if ((info.name == "new"s) or (info.name == "malloc"s)) {
		MODM_LOG_ERROR.printf("Ignoring '%s' of size 0x%x!\n", info.name, info.context);
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
	MODM_LOG_INFO << "\n=== RESTART ===\n" << modm::flush;

	// trigger an IRQ with undefined handler
	NVIC_EnableIRQ(OTG_FS_WKUP_IRQn);
	NVIC_SetPendingIRQ(OTG_FS_WKUP_IRQn);


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
	modm_assert(true_condition, "can.init",
	            "Can::init() function has timed out!");

	// only fails for debug builds, but is ignored anyways
	modm_assert_continue_fail_debug(false, "io.tx",
			"The IO transmit buffer is full!");

	MODM_LOG_ERROR << modm::flush;
	// "accidentally" return from main, without even returning properly!
	// This should be caught by the debug assert main.exit!
	// while (true)
	//     {};
	// return 0;
}
