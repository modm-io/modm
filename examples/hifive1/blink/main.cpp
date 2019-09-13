/*
 * Copyright (c) 2016-2017, Niklas Hauser
 * Copyright (c) 2017, Nick Sarten
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>


#define GPIO_OUT_EN  (*(reinterpret_cast<uint32_t*>(0x10012000+0x08)))
#define GPIO_OUT_VAL (*(reinterpret_cast<uint32_t*>(0x10012000+0x0C)))
#define GPIO_XOR     (*(reinterpret_cast<uint32_t*>(0x10012000+0x40)))

using namespace Board;


int
main()
{
	Board::initialize();

	constexpr uint32_t ALL_LEDS = 1<<19 | 1<<21 | 1<<22;

	GPIO_OUT_EN = ALL_LEDS;
	GPIO_XOR = ALL_LEDS;
	GPIO_OUT_VAL = ALL_LEDS;

	while (true)
	{

	}

	return 0;
}
