/*
 * Copyright (c) 2017, Niklas Hauser
 * Copyright (c) 2017, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <modm/board.hpp>

int main()
{
	Board::initialize();
	Board::Leds::setOutput();

	while (true)
	{
		Board::Leds::toggle();
		modm::delayMilliseconds(Board::Button::read() ? 250 : 500);
#ifdef MODM_BOARD_HAS_LOGGER
		static uint32_t counter(0);
		MODM_LOG_INFO << "Loop counter: " << (counter++) << modm::endl;
#endif
	}
	return 0;
}
