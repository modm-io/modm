/*
 * Copyright (c) 2019, Benjamin Weps
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <modm/processing/timer.hpp>

using namespace Board;

int
main()
{
	Board::initialize();

	modm::ShortPeriodicTimer timer(500ms);

	LedRed::set();
	LedGreen::set();
	LedBlue::set();

	int phase = 1;
	while (true)
	{
		if(timer.execute())
		{
			switch(phase)
			{
				case 0:
				{
					LedRed::set();
					LedGreen::set();
					LedBlue::set();
				}
				break;
				case 1:
				{
					LedGreen::reset();
					LedBlue::reset();
				}
				break;
				case 2:
				{
					LedRed::reset();
					LedGreen::set();
				}
				break;
				case 3:
				{
					LedGreen::reset();
					LedBlue::set();
				}
				break;
			}
			phase = (phase + 1)%4;
		}
	}

	return 0;
}
