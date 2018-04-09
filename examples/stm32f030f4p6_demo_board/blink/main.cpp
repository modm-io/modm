/*
 * Copyright (c) 2018, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board/board.hpp>

using namespace Board;

/*
 * Blinks the orange user LED with 1 Hz.
 * It is on for 90% of the time and off for 10% of the time.
 */

int
main()
{
	Board::initialize();

	LedOrange::set();

	while (true)
	{
		LedOrange::set();
		modm::delayMilliseconds(900);

		LedOrange::reset();
		modm::delayMilliseconds(100);
	}

	return 0;
}
