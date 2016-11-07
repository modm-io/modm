/*
 * Copyright (c) 2013-2014, Sascha Schade
 * Copyright (c) 2013, Kevin Laeufer
 * Copyright (c) 2013, 2015-2016, Niklas Hauser
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

	Board::LedNorth::set();

	while (1)
	{
		Board::LedNorth::toggle();
		xpcc::delayMilliseconds(100);
		Board::LedNorthEast::toggle();
		xpcc::delayMilliseconds(100);
		Board::LedEast::toggle();
		xpcc::delayMilliseconds(100);
		Board::LedSouthEast::toggle();
		xpcc::delayMilliseconds(100);
		Board::LedSouth::toggle();
		xpcc::delayMilliseconds(100);
		Board::LedSouthWest::toggle();
		xpcc::delayMilliseconds(100);
		Board::LedWest::toggle();
		xpcc::delayMilliseconds(100);
		Board::LedNorthWest::toggle();
		xpcc::delayMilliseconds(100);
	}

	return 0;
}
