/*
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2014, 2017, Sascha Schade
 * Copyright (c) 2014-2017, 2019, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "../radio.hpp"

template< class Phy >
void
test()
{
	Phy::setRxAddress(Phy::Pipe::PIPE_0, 0xdeadb33f05);
	uint64_t addr = Phy::getRxAddress(Phy::Pipe::PIPE_0);
	MODM_LOG_INFO.printf("Setting RX_P0 address to:  0xdeadb33f05\n");
	MODM_LOG_INFO.printf("Reading RX_P0 address:     0x%" PRIx32 "%" PRIx32 "\n",
	                     uint32_t((addr >> 32) & 0xffffffff),
	                     uint32_t(addr & 0xffffffff));

	Phy::setTxAddress(0xabcdef55ff);
	addr = Phy::getTxAddress();
	MODM_LOG_INFO.printf("Setting TX address to:     0xabcdef55ff\n");
	MODM_LOG_INFO.printf("Reading TX address:        0x%" PRIx32 "%" PRIx32 "\n",
	                     uint32_t((addr >> 32) & 0xffffffff),
	                     uint32_t(addr & 0xffffffff));

	uint8_t rf_ch = Phy::readRegister(Phy::NrfRegister::RF_CH);
	MODM_LOG_INFO.printf("Expected output for RF_CH: 0x2\n");
	MODM_LOG_INFO.printf("Reading RF_CH:             0x%" PRIx8 "\n\n", rf_ch);
}

// Test SPI communication by writing and reading out registers on the
// nRF24L01+ module.
int main()
{
	Board::initialize();
	MODM_LOG_INFO << "Hello from nRF24-phy-test example" << modm::endl;

	initializeSpi();

	while (true)
	{
		MODM_LOG_INFO << "Testing PHY1" << modm::endl;
		test<Nrf1Phy>();

		MODM_LOG_INFO << "Testing PHY2" << modm::endl;
		test<Nrf2Phy>();

		modm::delay(1s);
	}

	return 0;
}
