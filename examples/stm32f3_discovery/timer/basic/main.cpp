/*
 * Copyright (c) 2013-2014, Kevin Läufer
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

#include <modm/board/board.hpp>

int
main()
{
	Board::initialize();
	Board::LedNorth::set();


	Timer1::enable();
	//Timer1::setMode(Timer1::Mode::CenterAligned3);
	Timer1::setMode(Timer1::Mode::UpCounter);
	Timer1::setOverflow(0xff);
	//Timer1::setPeriod(4);
	Timer1::applyAndReset();
	Timer1::start();

	Timer1::connect<Board::LedNorth::Ch1, Board::LedNorthWest::Ch1n>();
	Timer1::setCompareValue(1, 0);
	Timer1::configureOutputChannel(1,
			static_cast<uint32_t>(Timer1::OutputCompareMode::Pwm) | 0b0101);
	Timer1::setDeadTime(7);

	Timer1::enableOutput();

	uint8_t i = 0;
	while (1){
		Timer1::setCompareValue(1, ++i);
		modm::delayMilliseconds(10);
	}

	return 0;
}
