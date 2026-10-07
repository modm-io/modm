/*
 * Copyright (c) 2021, Raphael Lehmann
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
#endif
#include <modm/architecture/interface/assert.hpp>
#include <etl/unordered_map.h>
#include <numbers>
#include <cstring>
#include <cstdlib>

#include "../integration_test.hpp"

/**
 * Tests that the Embedded Template Library reports its errors as assertions.
 * The test passes when the `etl` assertion fails.
 */

static modm::Abandonment
etl_handler(const modm::AssertionInfo &info)
{
	if (strcmp(info.name, "etl") == 0)
	{
		// does not return on microcontrollers
		exit(finishTest(true));
	}
	return modm::Abandonment::DontCare;
}
MODM_ASSERTION_HANDLER(etl_handler);

int main()
{
#if __has_include(<modm/board.hpp>)
	Board::initialize();
#endif

	etl::unordered_map<uint8_t, double, 3> map = {
		{15, std::numbers::pi},
		{42, std::numbers::e},
		{87, std::numbers::sqrt2}
	};
	bool passed = true;
	for (const auto& [key, value] : map) {
		MODM_LOG_INFO << "Key:[" << key << "] Value:[" << value << "]" << modm::endl;
		passed &= (key == 15 or key == 42 or key == 87);
	}
	passed &= (map.size() == 3);
	if (not passed) return finishTest(false);

	// The map is full: accessing a non-existent element causes an assertion
	MODM_LOG_INFO << map[12];

	// we should not get here
	return finishTest(false);
}
