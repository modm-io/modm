/*
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2013-2018, Niklas Hauser
 * Copyright (c) 2018, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>


int
main()
{
	Board::initialize();

	// Enable Uart SPI 1
	UartSpiMaster1::connect<GpioA8::Ck, GpioA9::Tx, GpioA10::Rx>();
	UartSpiMaster1::initialize<Board::SystemClock, 1_MHz, 0_pct>();

	while (true)
	{
		UartSpiMaster1::transferBlocking(0xF0);
	}

	return 0;
}
