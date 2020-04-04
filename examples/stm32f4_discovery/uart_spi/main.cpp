/*
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2013-2018, Niklas Hauser
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

	// Enable Uart SPI 2
	UartSpiMaster2::connect<GpioA4::Ck, GpioA2::Tx, GpioA3::Rx>();
	UartSpiMaster2::initialize<Board::SystemClock, 5.25_MHz, 0_pct>();

	while (true)
	{
		UartSpiMaster2::transferBlocking(0xF0);
	}

	return 0;
}
