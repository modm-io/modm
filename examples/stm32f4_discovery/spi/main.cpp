/*
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2013-2017, Niklas Hauser
 * Copyright (c) 2016, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board/board.hpp>

// If you use a different SPI instance, you may have to also choose different
// GPIOs to connect to.
using Sck = GpioOutputB13;
using Mosi = GpioOutputB15;
using Miso = GpioInputB14;
using SpiMaster = SpiMaster2;
// Note that you can also use a bit-banged SPI driver as a drop-in replacement
// using SpiMaster = BitBangSpiMaster<Sck, Mosi, Miso>;

int
main()
{
	Board::initialize();

	// Connect the GPIOs to the SPIs alternate function
	SpiMaster::connect<Miso::Miso, Mosi::Mosi, Sck::Sck>();
	// Initialize the SPI with a 20MHz clock
	SpiMaster::initialize<Board::systemClock, MHz20>();

	uint8_t data = 0;
	while (1)
	{
		// Connect Mosi to Miso to create a loopback
		data = SpiMaster::transferBlocking(data);
		data++;
		// This will be spamming the bus
	}

	return 0;
}
