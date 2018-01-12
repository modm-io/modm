/*
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2014-2017, Niklas Hauser
 * Copyright (c) 2014, 2017, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board/board.hpp>
#include <modm/driver/radio/nrf24/nrf24_phy.hpp>
#include <modm/debug/logger.hpp>
#include <inttypes.h>

/*
 * Test SPI communication by writing and reading out registers on the
 * nRF24L01+ module. Check output on UART
 *
 * Connect as follows
 *
 * STM32  NRF24
 * ------------
 * PB13 - SCK
 * PB14 - MISO
 * PB15 - MOSI
 * PE12 - CSN
 *
 * STM32  USB2UART
 * ---------------
 * PA2  - TXD
 */

#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::INFO


// Create an IODeviceWrapper around the Uart Peripheral we want to use
modm::IODeviceWrapper< Usart2, modm::IOBuffer::BlockIfFull > loggerDevice;

// Set all four logger streams to use the UART
modm::log::Logger modm::log::debug(loggerDevice);
modm::log::Logger modm::log::info(loggerDevice);
modm::log::Logger modm::log::warning(loggerDevice);
modm::log::Logger modm::log::error(loggerDevice);

typedef GpioOutputE12 Csn;

// just needed to make Nrf24Phy happy
typedef GpioOutputA1  Ce;


typedef modm::Nrf24Phy<SpiMaster2, Csn, Ce> nrf24phy;

int
main()
{
	Board::initialize();

	Csn::setOutput(modm::Gpio::High);

	// Enable SPI 2
	SpiMaster2::connect<GpioB15::Mosi, GpioB14::Miso, GpioB13::Sck>();
	SpiMaster2::initialize<Board::systemClock, MHz10>();

	// Enable UART 2
	Usart2::connect<GpioA2::Tx>();
	Usart2::initialize<Board::systemClock, 115200>();


	// Initialize nRF24-Phy
	nrf24phy::initialize();

	MODM_LOG_INFO << "Hello from nRF24-phy-test example" << modm::endl;


	uint8_t rf_ch;
	uint64_t addr;


	while (1)
	{

		nrf24phy::setRxAddress(nrf24phy::Pipe::PIPE_0, 0xdeadb33f05);
		addr = nrf24phy::getRxAddress(nrf24phy::Pipe::PIPE_0);
		MODM_LOG_INFO.printf("Setting RX_P0 address to:  0xDEADB33F05\n");
		MODM_LOG_INFO.printf("Reading RX_P0 address:     0x%" PRIx32 "%" PRIx32 "\n", static_cast<uint32_t>((addr >> 32) & 0xffffffff), static_cast<uint32_t>(addr & 0xffffffff));

		nrf24phy::setTxAddress(0xabcdef55ff);
		addr = nrf24phy::getTxAddress();
		MODM_LOG_INFO.printf("Setting TX address to:     0xABCDEF55FF\n");
		MODM_LOG_INFO.printf("Reading TX address:        0x%" PRIx32 "%" PRIx32 "\n", static_cast<uint32_t>((addr >> 32) & 0xffffffff), static_cast<uint32_t>(addr & 0xffffffff));

		rf_ch = nrf24phy::readRegister(nrf24phy::NrfRegister::RF_CH);
		MODM_LOG_INFO.printf("Expected output for RF_CH: 0x2\n");
		MODM_LOG_INFO.printf("Reading RF_CH:             0x%" PRIx8 "\n\n", rf_ch);

		modm::delayMilliseconds(1000);
	}

	return 0;
}
