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

RtGpio north     = Board::LedNorth();
RtGpio northEast = Board::LedNorthEast();
RtGpio east      = Board::LedEast();
RtGpio southEast = Board::LedSouthEast();
RtGpio south     = Board::LedSouth();
RtGpio southWest = Board::LedSouthWest();
RtGpio west      = Board::LedWest();
RtGpio northWest = Board::LedNorthWest();

modm::Gpio &gnorth = north;

int
main()
{
	Board::initialize();

	using Config = modm::Gpio::Config;
	gnorth.configure(Config::Output | Config::OpenDrain |
	                 Config::PullUp | Config::SpeedLow |
	                 Config::LogicInverted);
	gnorth.set(true);

	while (true)
	{
		north.toggle();
		modm::delay(100ms);
		northEast.toggle();
		modm::delay(100ms);
		east.toggle();
		modm::delay(100ms);
		southEast.toggle();
		modm::delay(100ms);
		south.toggle();
		modm::delay(100ms);
		southWest.toggle();
		modm::delay(100ms);
		west.toggle();
		modm::delay(100ms);
		northWest.toggle();
		modm::delay(100ms);
	}

	return 0;
}
