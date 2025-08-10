/*
 * Copyright (c) 2021, Jeff McBride
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "modm/board.hpp"

using namespace modm::platform;

int main()
{
	Board::initialize();

	while(true)
	{
		Board::Led::toggle();
		modm::delay_ms(500);
	}

}
