/*
 * Copyright (c) 2016, Kevin Läufer
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

using namespace Board;

int
main()
{
	Board::initialize();

	LedUp::set();
	LedDown::set();

	while (true) {
		LedRight::toggle();
		LedUp::toggle();
		LedLeft::toggle();
		LedDown::toggle();
		modm::delay(1000ms);
	}
}
