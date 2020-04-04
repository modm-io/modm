/*
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2013-2014, Sascha Schade
 * Copyright (c) 2013, 2015-2017, Niklas Hauser
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

	Board::LedNorth::set();

	while (true)
	{
		Board::LedNorth::toggle();
		modm::delay(100ms);
		Board::LedNorthEast::toggle();
		modm::delay(100ms);
		Board::LedEast::toggle();
		modm::delay(100ms);
		Board::LedSouthEast::toggle();
		modm::delay(100ms);
		Board::LedSouth::toggle();
		modm::delay(100ms);
		Board::LedSouthWest::toggle();
		modm::delay(100ms);
		Board::LedWest::toggle();
		modm::delay(100ms);
		Board::LedNorthWest::toggle();
		modm::delay(100ms);
	}

	return 0;
}
