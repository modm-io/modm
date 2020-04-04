/*
 * Copyright (c) 2014-2015, 2018, Daniel Krebs
 * Copyright (c) 2015, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_NRF24_CONFIG_HPP
#   error "Don't include this file directly, use 'nrf24_config.hpp' instead!"
#endif

#include "nrf24_config.hpp"
#include "nrf24_definitions.hpp"

#include <stdint.h>

// -----------------------------------------------------------------------------
template<typename Nrf24Phy>
void
modm::Nrf24Config<Nrf24Phy>::setMode(Mode mode)
{
	if(mode == currentMode) {
		// already in this mode
		return;
	}

	switch(mode) {
	case Mode::Rx:
//		MODM_LOG_DEBUG << "[nrf24] Set mode Rx" << modm::endl;
//		Nrf24Phy::resetCe();
//		modm::delay_us(30);
		Nrf24Phy::flushRxFifo();
		Nrf24Phy::setBits(NrfRegister::CONFIG, Config::PRIM_RX);

		break;
	case Mode::Tx:
//		MODM_LOG_DEBUG << "[nrf24] Set mode Tx" << modm::endl;

		Nrf24Phy::flushTxFifo();
		Nrf24Phy::clearBits(NrfRegister::CONFIG, Config::PRIM_RX);

		// pulsing CE seems to be necessary to enter TX mode
		Nrf24Phy::pulseCe();
		break;
	case Mode::Invalid:
		// do nothing
		return;
	}

	// don't go to standby
	Nrf24Phy::setCe();

	currentMode = mode;
}

// -----------------------------------------------------------------------------

template<typename Nrf24Phy>
void
modm::Nrf24Config<Nrf24Phy>::setSpeed(Speed speed)
{
	switch (speed) {
	case Speed::kBps250:
		Nrf24Phy::clearBits(NrfRegister::RF_SETUP, RfSetup::RF_DR_HIGH);
		Nrf24Phy::setBits  (NrfRegister::RF_SETUP, RfSetup::RF_DR_LOW);
		break;
	case Speed::MBps1:
		Nrf24Phy::clearBits(NrfRegister::RF_SETUP, RfSetup::RF_DR_HIGH);
		Nrf24Phy::clearBits(NrfRegister::RF_SETUP, RfSetup::RF_DR_LOW);
		break;
	case Speed::MBps2:
		Nrf24Phy::setBits  (NrfRegister::RF_SETUP, RfSetup::RF_DR_HIGH);
		Nrf24Phy::clearBits(NrfRegister::RF_SETUP, RfSetup::RF_DR_LOW);
		break;
	}
}

// -----------------------------------------------------------------------------

template<typename Nrf24Phy>
typename modm::Nrf24Config<Nrf24Phy>::Speed
modm::Nrf24Config<Nrf24Phy>::getSpeed()
{
	const uint8_t rf_setup = Nrf24Phy::readRegister(NrfRegister::RF_SETUP);

	if (rf_setup & static_cast<uint8_t>(RfSetup::RF_DR_LOW)) {
		return Speed::kBps250;
	}
	if(rf_setup & static_cast<uint8_t>(RfSetup::RF_DR_HIGH)) {
		return Speed::MBps2;
	}
	return Speed::MBps1;
}

// -----------------------------------------------------------------------------

template<typename Nrf24Phy>
void modm::Nrf24Config<Nrf24Phy>::setCrc(Crc crc)
{
	if(crc == Crc::NoCrc) {
		Nrf24Phy::clearBits(NrfRegister::CONFIG, Config::EN_CRC);
	} else {
		Nrf24Phy::setBits(NrfRegister::CONFIG, Config::EN_CRC);

		if (crc == Crc::Crc1Byte) {
			Nrf24Phy::clearBits(NrfRegister::CONFIG, Config::CRC0);
		} else if (crc == Crc::Crc2Byte) {
			Nrf24Phy::setBits(NrfRegister::CONFIG, Config::CRC0);
		}
	}
}

// -----------------------------------------------------------------------------

template<typename Nrf24Phy>
void modm::Nrf24Config<Nrf24Phy>::setRfPower(RfPower power)
{
	uint8_t reg = Nrf24Phy::readRegister(NrfRegister::RF_SETUP);
	reg &= ~(static_cast<uint8_t>(RfSetup::RF_PWR));	// Clear bits
	reg |=  ((static_cast<uint8_t>(power)) << 1); 		// Set bits
	Nrf24Phy::writeRegister(NrfRegister::RF_SETUP, reg);
}

// -----------------------------------------------------------------------------

template<typename Nrf24Phy>
void modm::Nrf24Config<Nrf24Phy>::setAutoRetransmitDelay(AutoRetransmitDelay delay)
{
	uint8_t reg = Nrf24Phy::readRegister(NrfRegister::SETUP_RETR);
	reg &= ~(static_cast<uint8_t>(SetupRetr::ARD));		// Clear bits
	reg |=  ((static_cast<uint8_t>(delay)) << 4); 		// Set bits
	Nrf24Phy::writeRegister(NrfRegister::SETUP_RETR, reg);
}

// -----------------------------------------------------------------------------

template<typename Nrf24Phy>
void modm::Nrf24Config<Nrf24Phy>::setAutoRetransmitCount(AutoRetransmitCount count)
{
	uint8_t reg = Nrf24Phy::readRegister(NrfRegister::SETUP_RETR);
	reg &= ~(static_cast<uint8_t>(SetupRetr::ARC));		// Clear bits
	reg |=  (static_cast<uint8_t>(count)); 				// Set bits
	Nrf24Phy::writeRegister(NrfRegister::SETUP_RETR, reg);
}

// -----------------------------------------------------------------------------

template<typename Nrf24Phy>
void
modm::Nrf24Config<Nrf24Phy>::enablePipe(Pipe_t pipe, bool enableAutoAck)
{
	// Set payload width
	NrfRegister_t reg = NrfRegister::RX_PW_P0;
	reg.value += pipe.value;
	Nrf24Phy::writeRegister(reg, Nrf24Phy::getPayloadLength());

	const Flags_t pipeBit(1 << pipe.value);

	// Enable or disable auto acknowledgment
	if(enableAutoAck) {
		Nrf24Phy::setBits(NrfRegister::EN_AA, pipeBit);
	} else {
		Nrf24Phy::clearBits(NrfRegister::EN_AA, pipeBit);
	}

	// Enable pipe
	Nrf24Phy::setBits(NrfRegister::EN_RX_ADDR, pipeBit);
}

// -----------------------------------------------------------------------------

template<typename Nrf24Phy>
typename modm::Nrf24Config<Nrf24Phy>::Pipe_t
modm::Nrf24Config<Nrf24Phy>::getPayloadPipe()
{
	const uint8_t status = Nrf24Phy::readStatus();
	return Pipe_t((status & static_cast<uint8_t>(Status::RX_P_NO)) >> 1);
}
