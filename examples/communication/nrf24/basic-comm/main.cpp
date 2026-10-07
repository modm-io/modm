/*
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2014, 2017, Sascha Schade
 * Copyright (c) 2014-2019, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "../radio.hpp"

/*
 * Basic communication with least config possible
 *
 * In this example one module is configured as primary tx, so it will just send
 * packets and never receive anything. The second module is configured as
 * primary rx, so it will only receive packets.
 */

using Register = Nrf1Phy::NrfRegister;
using Status = Nrf1Phy::Status;
using Pipe = Nrf1Phy::Pipe;
using Config = Nrf1Phy::Config;
using FifoStatus = Nrf1Phy::FifoStatus;

int main()
{
	Board::initialize();
	MODM_LOG_INFO << "Hello from nrf24-basic-comm example" << modm::endl;

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

	initializeSpi();

	Nrf1Ce::set();
	Nrf2Ce::set();

	// Initialize nRF24-HAL
	Nrf1Phy::initialize(payload_length);
	Nrf2Phy::initialize(payload_length);

	/* set RF channel */
	Nrf1Phy::writeRegister(Register::RF_CH, rf_channel);
	Nrf2Phy::writeRegister(Register::RF_CH, rf_channel);

	/* Set payload length for pipe 1 on receiver */
	Nrf2Phy::writeRegister(Register::RX_PW_P1, payload_length);

	/* Set tx address of ptx device to prx's address and also set
	 * receive pipe 0 to the same address to receive ACK packets
	 * from prx.
	 */
	Nrf1Phy::setTxAddress(prx_address);
	Nrf1Phy::setRxAddress(Pipe::PIPE_0, prx_address);

	/* Set receive pipe 1 of prx device to receive packets from ptx */
	Nrf2Phy::setRxAddress(Pipe::PIPE_1, prx_address);

	/* Configure ptx as primary sender and power up */
	Nrf1Phy::clearBits(Register::CONFIG, Config::PRIM_RX);
	Nrf1Phy::setBits(Register::CONFIG, Config::PWR_UP);

	/* Configure prx as primary receiver and power up*/
	Nrf2Phy::setBits(Register::CONFIG, Config::PRIM_RX);
	Nrf2Phy::setBits(Register::CONFIG, Config::PWR_UP);


	/* Timer to send packets every 1s */
	modm::ShortPeriodicTimer sendPacket(1s);

	/* Buffer for received payload */
	uint8_t received_data[payload_length];

	while (true)
	{
		// ------------------------- Primary sender ---------------------------

		/* Send packet every 1s */
		if(sendPacket.execute())
		{
			/* Copy packet into ptx device. Because CE is always high here, the
			 * packet will be transmitted immediately
			 */
			Nrf1Phy::writeTxPayload(payload, payload_length);
			payload[0]++;

			Board::LedD13::toggle();
		}

		/* Check if packet was sent successfully  */
		if(Nrf1Phy::readStatus() & ((uint8_t)Status::TX_DS | (uint8_t)Status::MAX_RT))
		{
			if(Nrf1Phy::readStatus() & (uint8_t)Status::MAX_RT)
			{
				MODM_LOG_INFO.printf("Packet lost, MAX_RT reached\n");
				MODM_LOG_INFO.printf("  Status: %x\n", Nrf1Phy::readStatus());
				Nrf1Phy::setBits(Register::STATUS, Status::MAX_RT);
				MODM_LOG_INFO.printf("  Status: %x\n", Nrf1Phy::readStatus());
			} else
			{
				MODM_LOG_INFO.printf("Packet successfully sent\n");
				Nrf1Phy::setBits(Register::STATUS, Status::TX_DS);
			}

			Board::LedD13::toggle();
		}


		// ----------------------- Primary receiver ---------------------------

		/* Check for received bytes */
		if( (!(Nrf2Phy::readRegister(Register::FIFO_STATUS) & (uint8_t)FifoStatus::RX_EMPTY)) || (Nrf2Phy::readStatus() & ((uint8_t)Status::RX_DR)))
		{
			uint8_t pl = 0;

			/* Read payload of received packet */
			pl = Nrf2Phy::readRxPayload(received_data);

			/* Clear RX_DR flag after payload is read */
			Nrf2Phy::setBits(Register::STATUS, Status::RX_DR);

			MODM_LOG_INFO.printf("Received packet, pl=%d, data: %x %x %x %x\n", pl, received_data[3], received_data[2], received_data[1], received_data[0]);

			Board::LedD13::toggle();
		}
	}

	return 0;
}
