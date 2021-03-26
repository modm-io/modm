/*
 * Copyright (c) 2016, Sascha Schade
 * Copyright (c) 2017, Niklas Hauser
 * Copyright (c) 2019, Raphael Lehmann
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

/*
 * Blinks the user LED with 1 Hz
 * or 5 Hz is the user button is pressed.
 */

int
main()
{
	Board::initialize();

	Led::set();

	while (true)
	{
		Led::toggle();
		modm::delay(Button::read() ? 100ms : 500ms);
	}

	return 0;
}
