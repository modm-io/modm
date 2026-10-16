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

#include "radio.hpp"

#include "../integration_test.hpp"

/**
 * Tests the SPI communication with two nRF24L01+ modules by writing and
 * reading back their address registers.
 */

template< class Phy >
bool
test()
{
	constexpr uint64_t RxAddress = 0xdeadb33f05;
	constexpr uint64_t TxAddress = 0xabcdef55ff;

	Phy::setRxAddress(Phy::Pipe::PIPE_0, RxAddress);
	const bool rx = (Phy::getRxAddress(Phy::Pipe::PIPE_0) == RxAddress);
	MODM_LOG_INFO << "RX_P0 address " << (rx ? "matches" : "does not match!") << modm::endl;

	Phy::setTxAddress(TxAddress);
	const bool tx = (Phy::getTxAddress() == TxAddress);
	MODM_LOG_INFO << "TX address " << (tx ? "matches" : "does not match!") << modm::endl;

	// reset value of the RF channel
	const uint8_t rf_ch = Phy::readRegister(Phy::NrfRegister::RF_CH);
	MODM_LOG_INFO << "RF_CH is " << rf_ch << ", expected 2" << modm::endl;

	return rx and tx and (rf_ch == 2);
}

int main()
{
	Board::initialize();
	initializeSpi();

	MODM_LOG_INFO << "Testing PHY1" << modm::endl;
	bool passed = test<Nrf1Phy>();

	MODM_LOG_INFO << "Testing PHY2" << modm::endl;
	passed &= test<Nrf2Phy>();

	return finishTest(passed);
}
