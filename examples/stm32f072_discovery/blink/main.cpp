/*
 * Copyright (c) 2016, Kevin Laeufer
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

using namespace Board;

int
main()
{
	Board::initialize();

	LedUp::set();
	LedDown::set();

	while (1) {
		LedRight::toggle();
		LedUp::toggle();
		LedLeft::toggle();
		LedDown::toggle();
		xpcc::delayMilliseconds(1000);
	}
}
