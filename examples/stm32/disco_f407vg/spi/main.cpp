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

#include <modm/board.hpp>

// If you use a different SPI instance, you may have to also choose different
// GPIOs to connect to.
using SckH = GpioOutputB13;
using MosiH = GpioOutputB15;
using MisoH = GpioInputB14;
using SpiMasterH = SpiMaster2;

// You can also use a bit-banged SPI driver
using SckB = GpioOutputB10;
using MosiB = GpioOutputB11;
using SpiMasterB = BitBangSpiMaster<SckB, MosiB>;

// or the Uart driver in SPI mode
using SckU = GpioOutputA4;
using MosiU = GpioOutputA2;
using MisoU = GpioInputA3;
using SpiMasterU = UartSpiMaster2;


int main()
{
	Board::initialize();

	// Connect the GPIOs to the SPI and initialize with 20MHz
	SpiMasterH::connect<MisoH::Miso, MosiH::Mosi, SckH::Sck>();
	SpiMasterH::initialize<Board::systemClock, MHz20>();

	// Connect the GPIOs to the UART and initialize with 5.25MHz
	SpiMasterU::connect<SckU::Ck, MosiU::Tx, MisoU::Rx>();
	SpiMasterU::initialize<Board::systemClock, 5250000, modm::Tolerance::Exact>();

	// Connect the GPIOs to the BitBang at 1MHz
	SpiMasterB::connect<MosiB::BitBang, SckB::BitBang>();
	SpiMasterB::initialize<Board::systemClock, MHz1>();

	uint8_t data = 0;
	while (1)
	{
		// Connect Mosi to Miso to create a loopback
		data = SpiMasterH::transferBlocking(data);
		data++;
		data = SpiMasterU::transferBlocking(data);
		data++;
		data = SpiMasterB::transferBlocking(data);
		data++;
		// This will be spamming the bus
	}

	return 0;
}
