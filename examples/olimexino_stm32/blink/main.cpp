/*
 * Copyright (c) 2017, Carl Treudler
 * Copyright (c) 2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <modm/board/board.hpp>

int main()
{
	Board::initialize();
	Board::LedD3::setOutput();


	while (1)
	{
		Board::LedD3::toggle();
		modm::delayMilliseconds(200);
	}
	return 0;
}
