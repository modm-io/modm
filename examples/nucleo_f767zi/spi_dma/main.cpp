/*
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

using Mosi = GpioOutputB5;
using Miso = GpioInputB4;
using Sck = GpioOutputB3;
using DmaRx = Dma2::Channel0;
using DmaTx = Dma2::Channel3;
using Spi = SpiMaster1_Dma<DmaRx, DmaTx>;

const uint8_t sendBuffer[13] {"data to send"};
uint8_t receiveBuffer[13];

int main()
{
	Board::initialize();

	MODM_LOG_INFO << "Hello from SPI-DMA example on Nucleo-F767ZI!" << modm::endl;

	// Enable DMA controller
	Dma2::enable();

	// Enable and initialize SPI
	Spi::connect<Mosi::Mosi, Miso::Miso, Sck::Sck>();
	Spi::initialize<Board::SystemClock, 422_kHz>();

	while (true)
	{
		MODM_LOG_INFO << "sendBuffer adress: " << modm::hex << sendBuffer << modm::endl;
		MODM_LOG_INFO << "receiveBuffer adress: " << modm::hex << receiveBuffer << modm::endl;
		MODM_LOG_INFO << "Info: 0x20000000 is start of SRAM1" << modm::endl;

		MODM_LOG_INFO << "Before first transfer" << modm::endl;

		// send out 12 bytes, don't care about response
		Spi::transfer(sendBuffer, nullptr, 12);

		MODM_LOG_INFO << "After first transfer" << modm::endl;

		// send out 12 bytes, read in 12 bytes
		Spi::transfer(sendBuffer, receiveBuffer, 12);

		MODM_LOG_INFO << "After second transfer" << modm::endl << modm::endl;

		modm::delay(500ms);
	}

	return 0;
}
