/*
 * Copyright (c) 2011, Fabian Greif
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
#include <modm/architecture/interface/accessor.hpp>
#include <string.h>

#include "../integration_test.hpp"

/**
 * Tests reading data that is stored in the Flash of an AVR.
 */

FLASH_STORAGE(int foo) = 12;

FLASH_STORAGE_STRING(string) = "Hallo Welt!\n";

FLASH_STORAGE(int32_t bla[4]) = {1,2,3,4};

static bool
equals(modm::accessor::Flash<char> s, const char *expected)
{
	char c;
	while ((c = *s++)) {
		if (c != *expected++) return false;
	}
	return *expected == 0;
}

int
main()
{
	Board::initialize();
	bool passed = true;

	modm::accessor::Flash<int> bar(&foo);
	passed &= (*bar == 12);

	passed &= equals(modm::accessor::asFlash(string), "Hallo Welt!\n");

	modm::accessor::Flash<int32_t> blub(bla);
	passed &= (blub[0] == 1) and (blub[2] == 3) and (blub[3] == 4);

	return finishTest(passed);
}
