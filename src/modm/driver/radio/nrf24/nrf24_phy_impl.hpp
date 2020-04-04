/*
 * Copyright (c) 2014, Niklas Hauser
 * Copyright (c) 2014-2015, Daniel Krebs
 * Copyright (c) 2015, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_NRF24_PHY_HPP
#   error "Don't include this file directly, use 'nrf24_phy.hpp' instead!"
#endif

#include "nrf24_phy.hpp"
#include "nrf24_definitions.hpp"

// -----------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
void
modm::Nrf24Phy<Spi, Csn, Ce>::writeCommandNoData(Command_t cmd)
{
	Csn::reset();

	status = Spi::transferBlocking(cmd.value);

	Csn::set();
}

// -----------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
uint8_t
modm::Nrf24Phy<Spi, Csn, Ce>::writeCommandSingleData(Command_t cmd, uint8_t data)
{
	Csn::reset();

	status = Spi::transferBlocking(cmd.value);

	uint8_t ret = Spi::transferBlocking(data);

	Csn::set();

	return ret;
}

// -----------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
void
modm::Nrf24Phy<Spi, Csn, Ce>::writeCommandMultiData(Command_t cmd, uint8_t* argv, uint8_t* retv, uint8_t argc)
{
	Csn::reset();

	status = Spi::transferBlocking(cmd.value);

	for(uint8_t i = 0; i < argc; i++) {
		uint8_t arg = (argv == nullptr) ? 0 : argv[i];
		uint8_t data = Spi::transferBlocking(arg);

		if(retv != nullptr) {
			retv[i] = data;
		}
	}

	Csn::set();
}

// -----------------------------------------------------------------------------


template<typename Spi, typename Csn, typename Ce>
uint8_t
modm::Nrf24Phy<Spi, Csn, Ce>::readRegister(NrfRegister_t reg)
{
	Command_t cmd = Command::R_REGISTER;
	cmd.value |= reg.value;
	return writeCommandSingleData(cmd, 0x00);
}

// -----------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
void
modm::Nrf24Phy<Spi, Csn, Ce>::writeRegister(NrfRegister_t reg, uint8_t data)
{
	Command_t cmd = Command::W_REGISTER;
	cmd.value |= reg.value;
	writeCommandSingleData(cmd, data);
}

// -----------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
void
modm::Nrf24Phy<Spi, Csn, Ce>::setBits(NrfRegister_t reg, Flags_t flags)
{
	uint8_t old = readRegister(reg);
	writeRegister(reg, old | flags.value);
}

// -----------------------------------------------------------------------------


template<typename Spi, typename Csn, typename Ce>
void
modm::Nrf24Phy<Spi, Csn, Ce>::clearInterrupt(InterruptFlag_t flag)
{
	writeRegister(NrfRegister::STATUS, flag.value);
}

// -----------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
void
modm::Nrf24Phy<Spi, Csn, Ce>::clearBits(NrfRegister_t reg, Flags_t flags)
{
	uint8_t old = readRegister(reg);
	writeRegister(reg, old & ~flags.value);
}

// -----------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
uint8_t
modm::Nrf24Phy<Spi, Csn, Ce>::readRxPayload(uint8_t* buffer)
{
	uint8_t cur_pl_len = payload_len;

	// cope with dynamic payload
	if(cur_pl_len == 0) {
		// TODO: verify that this really works!
		cur_pl_len = readRxPayloadWidth();
	}

	writeCommandMultiData(Command::R_RX_PAYLOAD, nullptr, buffer, cur_pl_len);

	return cur_pl_len;
}

// -----------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
void
modm::Nrf24Phy<Spi, Csn, Ce>::writeTxPayload(uint8_t* buffer, uint8_t len)
{
	if(len > payload_len) {
		return;
	}

	writeCommandMultiData(Command::W_TX_PAYLOAD, buffer, nullptr, len);
}

// -----------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
void
modm::Nrf24Phy<Spi, Csn, Ce>::writeTxPayloadNoAck(uint8_t* buffer, uint8_t len)
{
	if(len > payload_len) {
		return;
	}

	writeCommandMultiData(Command::W_TX_PAYLOAD_NOACK, buffer, nullptr, len);
}

// -----------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
void
modm::Nrf24Phy<Spi, Csn, Ce>::writeAckPayload(Pipe_t pipe, uint8_t* buffer, uint8_t len)
{
	if(len > payload_len) {
		return;
	}

	Command_t cmd = Command::W_ACK_PAYLOAD;
	cmd.value |= pipe.value;
	writeCommandMultiData(cmd, buffer, nullptr, len);
}

// -----------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
uint8_t
modm::Nrf24Phy<Spi, Csn, Ce>::readStatus()
{
	writeCommandNoData(Command::NOP);
	return status;
}

// -----------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
void
modm::Nrf24Phy<Spi, Csn, Ce>::setRxAddress(Pipe_t pipe, uint64_t address)
{
	Command_t cmd = Command::W_REGISTER;
	cmd.value |= static_cast<uint8_t>(NrfRegister::RX_ADDR_P0);

	// add pipe value so register will be RX_ADDR_Px if pipe is 'x'
	cmd.value += pipe.value;

	if(pipe.value <= 1) {
		// registers RX_ADDR_P0|1 are 40 bit wide

		// assemble address
		uint8_t addr[max_address_size];
		for(uint8_t i = 0; i < max_address_size; i++) {
			addr[i] = address & 0xff;
			address >>= 8;
		}

		writeCommandMultiData(cmd, addr, nullptr, max_address_size);
	} else if(pipe.value < rx_pipe_count) {
		// registers RX_ADDR_P2-5 are just 8 bit wide

		NrfRegister_t reg = NrfRegister::RX_ADDR_P0;
		reg.value += pipe.value;

		writeRegister(reg, static_cast<uint8_t>(address & 0xff));
	}
}

// -----------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
void
modm::Nrf24Phy<Spi, Csn, Ce>::setTxAddress(uint64_t address)
{
	uint8_t addr[max_address_size];

	for(uint8_t i = 0; i < max_address_size; i++) {
		addr[i] = address & 0xff;
		address >>= 8;
	}

	Command_t cmd = Command::W_REGISTER;
	cmd.value |= static_cast<uint8_t>(NrfRegister::TX_ADDR);

	writeCommandMultiData(cmd, addr, nullptr, max_address_size);
}

// -----------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
uint64_t
modm::Nrf24Phy<Spi, Csn, Ce>::readAddressRegister(Nrf24Register::NrfRegister_t reg)
{
	switch(static_cast<Nrf24Register::NrfRegister>(reg.value)) {

	// register RX_ADDR_P0|1 and TX_ADDR are 40 bit wide, so they need special treatment
	case NrfRegister::RX_ADDR_P0:
	case NrfRegister::RX_ADDR_P1:
	case NrfRegister::TX_ADDR:
	{
		Command_t cmd = Command::R_REGISTER;
		cmd.value |= reg.value;

		// get address from module
		uint8_t addr[max_address_size];
		writeCommandMultiData(cmd, nullptr, addr,  max_address_size);

		// assemble into uint64_t address
		uint64_t address = 0;
		for(int8_t i = max_address_size - 1; i >= 0; i--) {
			address <<= 8;
			address |= addr[i];
		}

		return address;
	}

	// register RX_ADDR_P2-5 are just 8 bit wide
	case NrfRegister::RX_ADDR_P2:
	case NrfRegister::RX_ADDR_P3:
	case NrfRegister::RX_ADDR_P4:
	case NrfRegister::RX_ADDR_P5:
		return readRegister(reg);

	// other registers will obviously not be handled here
	default:
		// returning 0 as error indicator is totally okay because this address
		// is not allowed by the datasheet anyway
		return 0;
	}
}

// -----------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
uint64_t
modm::Nrf24Phy<Spi, Csn, Ce>::getRxAddress(Pipe_t pipe)
{
	NrfRegister_t reg = NrfRegister::RX_ADDR_P0;
	reg.value += pipe.value;

	return readAddressRegister(reg);
}

// -----------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
uint64_t
modm::Nrf24Phy<Spi, Csn, Ce>::getTxAddress()
{
	return readAddressRegister(NrfRegister::TX_ADDR);
}

// -----------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
void
modm::Nrf24Phy<Spi, Csn, Ce>::pulseCe()
{
	Ce::toggle();

	// delay might not be precise enough
	modm::delay_us(15);

	Ce::toggle();
}

template<typename Spi, typename Csn, typename Ce>
void
modm::Nrf24Phy<Spi, Csn, Ce>::dumpRegisters()
{
	MODM_LOG_DEBUG.printf(" --------------------- Register Dump Begin -------------------------\n");

	MODM_LOG_DEBUG.printf("CONFIG          0x%02x\n", readRegister(NrfRegister::CONFIG));
	MODM_LOG_DEBUG.printf("EN_AA           0x%02x\n", readRegister(NrfRegister::EN_AA));
	MODM_LOG_DEBUG.printf("EN_RX_ADDR      0x%02x\n", readRegister(NrfRegister::EN_RX_ADDR));
	MODM_LOG_DEBUG.printf("SETUP_AW        0x%02x\n", readRegister(NrfRegister::SETUP_AW));
	MODM_LOG_DEBUG.printf("SETUP_RETR      0x%02x\n", readRegister(NrfRegister::SETUP_RETR));
	MODM_LOG_DEBUG.printf("RF_CH           0x%02x\n", readRegister(NrfRegister::RF_CH));
	MODM_LOG_DEBUG.printf("RF_SETUP        0x%02x\n", readRegister(NrfRegister::RF_SETUP));
	MODM_LOG_DEBUG.printf("STATUS          0x%02x\n", readRegister(NrfRegister::STATUS));
	MODM_LOG_DEBUG.printf("OBSERVE_TX      0x%02x\n", readRegister(NrfRegister::OBSERVE_TX));
	MODM_LOG_DEBUG.printf("RPD             0x%02x\n", readRegister(NrfRegister::RPD));
	auto addr = getRxAddress(Pipe::PIPE_0);
	MODM_LOG_DEBUG.printf("RX_ADDR_P0      0x%02x %02x %02x %02x %02x\n",
	                      (uint8_t)(addr >> 32), (uint8_t)(addr >> 24),
	                      (uint8_t)(addr >> 16), (uint8_t)(addr >> 8),
	                      (uint8_t)(addr >> 0));
	addr = getRxAddress(Pipe::PIPE_1);
	MODM_LOG_DEBUG.printf("RX_ADDR_P1      0x%02x %02x %02x %02x %02x\n",
	                      (uint8_t)(addr >> 32), (uint8_t)(addr >> 24),
	                      (uint8_t)(addr >> 16), (uint8_t)(addr >> 8),
	                      (uint8_t)(addr >> 0));
	MODM_LOG_DEBUG.printf("RX_ADDR_P2      0x%02x\n", getRxAddress(Pipe::PIPE_2));
	MODM_LOG_DEBUG.printf("RX_ADDR_P3      0x%02x\n", getRxAddress(Pipe::PIPE_3));
	MODM_LOG_DEBUG.printf("RX_ADDR_P4      0x%02x\n", getRxAddress(Pipe::PIPE_4));
	MODM_LOG_DEBUG.printf("RX_ADDR_P5      0x%02x\n", getRxAddress(Pipe::PIPE_5));
	addr = getTxAddress();
	MODM_LOG_DEBUG.printf("TX_ADDR         0x%02x %02x %02x %02x %02x\n",
	                      (uint8_t)(addr >> 32), (uint8_t)(addr >> 24),
	                      (uint8_t)(addr >> 16), (uint8_t)(addr >> 8),
	                      (uint8_t)(addr >> 0));
	MODM_LOG_DEBUG.printf("RX_PW_P0        0x%02x\n", readRegister(NrfRegister::RX_PW_P0));
	MODM_LOG_DEBUG.printf("RX_PW_P1        0x%02x\n", readRegister(NrfRegister::RX_PW_P1));
	MODM_LOG_DEBUG.printf("RX_PW_P2        0x%02x\n", readRegister(NrfRegister::RX_PW_P2));
	MODM_LOG_DEBUG.printf("RX_PW_P3        0x%02x\n", readRegister(NrfRegister::RX_PW_P3));
	MODM_LOG_DEBUG.printf("RX_PW_P4        0x%02x\n", readRegister(NrfRegister::RX_PW_P4));
	MODM_LOG_DEBUG.printf("RX_PW_P5        0x%02x\n", readRegister(NrfRegister::RX_PW_P5));
	MODM_LOG_DEBUG.printf("FIFO_STATUS     0x%02x\n", readRegister(NrfRegister::FIFO_STATUS));
	MODM_LOG_DEBUG.printf("DYNPD           0x%02x\n", readRegister(NrfRegister::DYNPD));
	MODM_LOG_DEBUG.printf("FEATURE         0x%02x\n", readRegister(NrfRegister::FEATURE));

	MODM_LOG_DEBUG.printf(" ---------------------- Register Dump End --------------------------\n");
}
