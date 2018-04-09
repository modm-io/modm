/*
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2014, 2017, Sascha Schade
 * Copyright (c) 2014-2018, Niklas Hauser
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
#include <modm/processing/timer.hpp>

#include <modm/driver/radio/nrf24/nrf24_definitions.hpp>

using modm::Nrf24Register;
using modm::Nrf24Phy;

/*
 * Basic communication with least config possible
 *
 * In this example one module is configured as primary tx, so it will just send
 * packets and never receive anything. The second module is configured as
 * primary rx, so it will only receive packets.
 *
 *
 * Connect as follows
 *
 * STM32  NRF24 module 1
 * ------------
 * PB3  - SCK
 * PB4  - MISO
 * PB5  - MOSI
 * PB6  - CSN
 * PB7  - CE
 *
 * STM32  NRF24 module 2
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


typedef GpioOutputB7 Ce1;
typedef GpioOutputB6 Csn1;

typedef GpioOutputE11 Ce2;
typedef GpioOutputE12 Csn2;


typedef Nrf24Phy<SpiMaster1, Csn1, Ce1> nrf24ptx;
typedef Nrf24Phy<SpiMaster2, Csn2, Ce2> nrf24prx;

typedef nrf24ptx::NrfRegister Register;
typedef nrf24ptx::NrfRegister_t Register_t;

typedef nrf24ptx::Config Config;
typedef nrf24ptx::Status Status;
typedef nrf24ptx::Pipe Pipe;
typedef nrf24ptx::FifoStatus FifoStatus;

void
configureBoth(Register_t reg, uint8_t value)
{
	nrf24ptx::writeRegister(reg, value);
	nrf24prx::writeRegister(reg, value);
}

int
main()
{
	Board::initialize();

	Csn1::setOutput(modm::Gpio::High);
	Csn2::setOutput(modm::Gpio::High);
	Ce1::setOutput(modm::Gpio::Low);
	Ce2::setOutput(modm::Gpio::Low);

	// Enable SPI 1
	SpiMaster1::connect<GpioB5::Mosi, GpioB4::Miso, GpioB3::Sck>();
	SpiMaster1::initialize<Board::systemClock, 10500000, modm::Tolerance::Exact>();

	// Enable SPI 2
	SpiMaster2::connect<GpioB15::Mosi, GpioB14::Miso, GpioB13::Sck>();
	SpiMaster2::initialize<Board::systemClock, 10500000, modm::Tolerance::Exact>();

	// Enable UART 2
	Usart2::connect<GpioA2::Tx>();
	Usart2::initialize<Board::systemClock, 115200>();

	/* Configuration values for nRF24 */

	/* Use channel 2400 + 60 MHz = 2460 MHz */
	constexpr const uint8_t rf_channel = 60;

	/* 4 byte payload length */
	constexpr const uint8_t payload_length = 4;

	/* Address for ptx module. Not used here because ptx won't receive packets */
	// constexpr const uint64_t ptx_address = 0xB3B4B5B605;
	constexpr const uint64_t prx_address = 0xB3B4B5B607;

	/* Dummy payload */
	uint8_t payload[payload_length] = {
		0xaa,
		0xbb,
		0xcc,
		0xdd
	};

	// Initialize nRF24-HAL
	nrf24ptx::initialize(payload_length);
	nrf24prx::initialize(payload_length);


	MODM_LOG_INFO << "Hello from nrf24-basic-comm example" << modm::endl;

	/* set RF channel */
	nrf24ptx::writeRegister(nrf24ptx::NrfRegister::RF_CH, rf_channel);
	nrf24prx::writeRegister(nrf24prx::NrfRegister::RF_CH, rf_channel);

	/* Set payload length for pipe 1 on receiver */
	nrf24prx::writeRegister(Register::RX_PW_P1, payload_length);

	/* Set tx address of ptx device to prx's address and also set
	 * receive pipe 0 to the same address to receive ACK packets
	 * from prx.
	 */
	nrf24ptx::setTxAddress(prx_address);
	nrf24ptx::setRxAddress(Pipe::PIPE_0, prx_address);

	/* Set receive pipe 1 of prx device to receive packets from ptx */
	nrf24prx::setRxAddress(Pipe::PIPE_1, prx_address);

	/* Configure ptx as primary sender and power up */
	nrf24ptx::clearBits(Register::CONFIG, Config::PRIM_RX);
	nrf24ptx::setBits(Register::CONFIG, Config::PWR_UP);

	/* Configure prx as primary receiver and power up*/
	nrf24prx::setBits(Register::CONFIG, Config::PRIM_RX);
	nrf24prx::setBits(Register::CONFIG, Config::PWR_UP);

	/* CE always high on both, so the module don't go to standby */
	Ce1::set();
	Ce2::set();


	/* Timer to send packets every 1000ms */
	modm::ShortPeriodicTimer sendPacket(1000);

	/* Buffer for received payload */
	uint8_t received_data[payload_length];

	while (1)
	{
		// ------------------------- Primary sender ---------------------------

		/* Send packet every 1000ms */
		if(sendPacket.execute())
		{
			/* Copy packet into ptx device. Because CE is always high here, the
			 * packet will be transmitted immediately
			 */
			nrf24ptx::writeTxPayload(payload, payload_length);

			payload[0]++;

			Board::LedOrange::toggle();
		}

		/* Check if packet was sent successfully  */
		if(nrf24ptx::readStatus() & ((uint8_t)Status::TX_DS | (uint8_t)Status::MAX_RT))
		{
			if(nrf24ptx::readStatus() & (uint8_t)Status::MAX_RT)
			{
				MODM_LOG_INFO.printf("Packet lost, MAX_RT reached\n");
				MODM_LOG_INFO.printf("  Status: %x\n", nrf24ptx::readStatus());
				nrf24ptx::setBits(Register::STATUS, Status::MAX_RT);
				MODM_LOG_INFO.printf("  Status: %x\n", nrf24ptx::readStatus());
			} else
			{
				MODM_LOG_INFO.printf("Packet successfully sent\n");
				nrf24ptx::setBits(Register::STATUS, Status::TX_DS);
			}

			Board::LedBlue::toggle();
		}


		// ----------------------- Primary receiver ---------------------------

		/* Check for received bytes */
		if( (!(nrf24prx::readRegister(Register::FIFO_STATUS) & (uint8_t)FifoStatus::RX_EMPTY)) || (nrf24prx::readStatus() & ((uint8_t)Status::RX_DR)))
		{
			uint8_t pl = 0;

			/* Read payload of received packet */
			pl = nrf24prx::readRxPayload(received_data);

			/* Clear RX_DR flag after payload is read */
			nrf24prx::setBits(Register::STATUS, Status::RX_DR);

			MODM_LOG_INFO.printf("Received packet, pl=%d, data: %x %x %x %x\n", pl, received_data[3], received_data[2], received_data[1], received_data[0]);

		Board::LedGreen::toggle();
		}
	}

	return 0;
}
