/*
 * Copyright (c) 2015, Daniel Krebs
 * Copyright (c) 2015, Sascha Schade
 * Copyright (c) 2015-2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board/board.hpp>
#include <modm/debug/logger.hpp>
#include <modm/processing/processing.hpp>

#include <modm/driver/radio/nrf24/nrf24_phy.hpp>
#include <modm/driver/radio/nrf24/nrf24_config.hpp>
#include <modm/driver/radio/nrf24/nrf24_data.hpp>


/*
 * This example showcases a simple usage of the data layer implementation for
 * the nRF24L01+ radio modules.
 *
 * Check output on UART.
 *
 * Connect as follows
 *
 * STM32  NRF24
 * ------------
 * PB13 - SCK
 * PB14 - MISO
 * PB15 - MOSI
 * PE11 - CE
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

/* nrf24 interface */
typedef SpiMaster2      Spi;
typedef GpioOutputE12   Csn;
typedef GpioOutputE11   Ce;

/* Setup and connect layers */
typedef modm::Nrf24Phy<Spi, Csn, Ce> nrf24phy;
typedef modm::Nrf24Config<nrf24phy> nrf24config;
typedef modm::Nrf24Data<nrf24phy> nrf24data;

int
main()
{
	Board::initialize();

	// Setup GPIOs
	Csn::setOutput(modm::Gpio::High);
	Ce::setOutput(modm::Gpio::Low);

	// Enable SPI 2
	Spi::connect<GpioB15::Mosi, GpioB14::Miso, GpioB13::Sck>();
	Spi::initialize<Board::systemClock, MHz10>();

	// Enable UART 2
	Usart2::connect<GpioA2::Tx>();
	Usart2::initialize<Board::systemClock, 115200>();

	MODM_LOG_INFO << "Hello from nrf24-data-rx example" << modm::endl;

	// Send alive message every 5 seconds
	modm::PeriodicTimer aliveTimer(5000);

	// Initialize physical layer with payload size of 6 bytes, this gives us
	// a payload size of 4 bytes in the data layer, as the header takes 2 bytes.
	nrf24phy::initialize(6);

	// This is the base address of our network, imagine this as an IP subnet
	// like 192.168.1.0/24. Coincidentally, our the network also has 256
	// addresses. The lower byte should be zero because addresses will be placed
	// there.
	nrf24data::BaseAddress base_address = 0xdeadbeef00;

	// We need to define a broadcast address, this can be any address.
	nrf24data::Address broadcast_address = 0xff;

	// Now we define the addresses of our 2 endpoints in this example.
	nrf24data::Address address_tx_side = 0x11;
	nrf24data::Address address_rx_side = 0x33;

	// Initialize data layer, this also rudimentary configures the physical
	// layer (e.g. speed, crc) so we just need to set a channel and are good to
	// go.
	nrf24data::initialize(base_address, address_rx_side, broadcast_address);
	nrf24config::setChannel(10);

	// Using the data layer, we send and receive packets, so we need to allocate
	// one. It's automatically allocated with the max. payload length configured.
	nrf24data::Packet packet;

	while(1)
	{
		if (nrf24data::getPacket(packet))
		{
			MODM_LOG_INFO.printf("Received packet from 0x%02x\n", packet.src);
			MODM_LOG_INFO.printf("Data: %02x %02x %02x %02x\n",
					packet.payload.data[3],
					packet.payload.data[2],
					packet.payload.data[1],
					packet.payload.data[0]);
		}

		if(aliveTimer.execute())
		{
			MODM_LOG_INFO << "Rx is still alive" << modm::endl;
		}

		nrf24data::update();
	}

	return 0;
}
