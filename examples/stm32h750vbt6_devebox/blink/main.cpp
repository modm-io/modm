/*
 * Copyright (c) 2021, Henrik Hose
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

	while(true)
	{
		LedGreen::toggle();
		modm::delay(100ms + ButtonK1::read()*500ms + ButtonK2::read()*1000ms);
	}

	return 0;
}
