/*
 * Copyright (c) 2020, Mike Wolfram
 * Copyright (c) 2021, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <cstring>

#include "../integration_test.hpp"

/**
 * Tests SPI transfers via DMA in a loopback:
 * connect Mosi (PB5) to Miso (PB4)!
 */

using Mosi = GpioOutputB5;
using Miso = GpioInputB4;
using Sck = GpioOutputB3;

// The DMA controller and its channels for SPI1 differ between the devices
#if defined STM32F4 or defined STM32F7
using Dma = Dma2;
using DmaRx = Dma::Channel0;
using DmaTx = Dma::Channel3;
#else
using Dma = Dma1;
using DmaRx = Dma::Channel2;
using DmaTx = Dma::Channel3;
#endif
using Spi = SpiMaster1_Dma<DmaRx, DmaTx>;

const uint8_t sendBuffer[13] {"data to send"};
uint8_t receiveBuffer[13];

int main()
{
	Board::initialize();

	Dma::enable();
	Spi::connect<Mosi::Mosi, Miso::Miso, Sck::Sck>();
	// the slowest SPI clock works on every device
	Spi::initialize<Board::SystemClock, Board::SystemClock::Spi1 / 256>();

	// send out 12 bytes, don't care about response
	Spi::transfer(sendBuffer, nullptr, 12);

	// send out 12 bytes, read in 12 bytes
	Spi::transfer(sendBuffer, receiveBuffer, 12);
	MODM_LOG_INFO << "Received '" << reinterpret_cast<const char*>(receiveBuffer) << "'" << modm::endl;

	return finishTest(memcmp(sendBuffer, receiveBuffer, 12) == 0);
}
