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

#include <modm/platform.hpp>
#include <modm/architecture/interface/accessor.hpp>

//extern const int a PROGMEM;
//const int a = 12;

FLASH_STORAGE(int foo) = 12;

FLASH_STORAGE_STRING(string) = "Hallo Welt!\n";

FLASH_STORAGE(int32_t bla[4]) = {1,2,3,4};

volatile uint8_t out;

void
function(modm::accessor::Flash<char> s)
{
	char c;
	while ((c = *s++)) {
		out = c;
	}
}

int
main()
{
	modm::accessor::Flash<int> bar(&foo);

	out = *bar;

	function(modm::accessor::asFlash(string));

	modm::accessor::Flash<int32_t> blub(bla);

	out = blub[2];

	while (1) {

	}
}
