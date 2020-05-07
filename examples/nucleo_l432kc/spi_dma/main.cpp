/*
 * Copyright (c) 2020, Mike Wolfram
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>

using Mosi = GpioOutputB5;
using Miso = GpioInputB4;
using Sck = GpioOutputB3;
using DmaRx = Dma1::Channel2;
using DmaTx = Dma1::Channel3;
using Spi = SpiMaster1_Dma<DmaRx, DmaTx>;

int main()
{
	Board::initialize();

	// Enable DMA1 controller
    Dma1::enable();
    // Enable SPI1
    Spi::connect<Mosi::Mosi, Miso::Miso, Sck::Sck>();
    Spi::initialize<Board::SystemClock, 10_MHz>();

	while (true)
	{
		uint8_t sendBuffer[13] { "data to send" };
		uint8_t receiveBuffer[13];

		// send out 12 bytes, don't care about response
		Spi::transferBlocking(sendBuffer, nullptr, 12);

		// send out 12 bytes, read in 12 bytes
		Spi::transferBlocking(sendBuffer, receiveBuffer, 12);
	}

	return 0;
}
