// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__NRF24_PHY_HPP
#   error "Don't include this file directly, use 'nrf24_phy.hpp' instead!"
#endif

#include "nrf24_phy.hpp"
#include "nrf24_definitions.hpp"

template <typename Spi, typename Csn, typename Ce>
uint8_t xpcc::Nrf24Phy<Spi, Csn, Ce>::status;

template <typename Spi, typename Csn, typename Ce>
uint8_t xpcc::Nrf24Phy<Spi, Csn, Ce>::payload_len;


// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
void
xpcc::Nrf24Phy<Spi, Csn, Ce>::writeCommandNoData(Command_t cmd)
{
	Csn::reset();

	status = Spi::transferBlocking(cmd.value);

	Csn::set();
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
uint8_t
xpcc::Nrf24Phy<Spi, Csn, Ce>::writeCommandSingleData(Command_t cmd, uint8_t data)
{
	Csn::reset();

	status = Spi::transferBlocking(cmd.value);

	uint8_t ret = Spi::transferBlocking(data);

	Csn::set();

	return ret;
}

// --------------------------------------------------------------------------------------------------------------------

/*
 * TODO: maybe optimize this
 */
template<typename Spi, typename Csn, typename Ce>
void
xpcc::Nrf24Phy<Spi, Csn, Ce>::writeCommandMultiData(Command_t cmd, uint8_t* argv, uint8_t* retv, uint8_t argc)
{
	Csn::reset();

	status = Spi::transferBlocking(cmd.value);

	uint8_t i;
	for(i = 0; i < argc; i++)
	{
		uint8_t data;
		uint8_t arg = 0;

		if(argv != nullptr)
		{
			arg = argv[i];
		}

		data = Spi::transferBlocking(arg);

		if(retv != nullptr)
		{
			retv[i] = data;
		}
	}

	Csn::set();
}

// --------------------------------------------------------------------------------------------------------------------


template<typename Spi, typename Csn, typename Ce>
uint8_t
xpcc::Nrf24Phy<Spi, Csn, Ce>::readRegister(NrfRegister_t reg)
{
	Command_t cmd = Command::R_REGISTER;
	cmd.value |= reg.value;
	return writeCommandSingleData(cmd, 0x00);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
void
xpcc::Nrf24Phy<Spi, Csn, Ce>::writeRegister(NrfRegister_t reg, uint8_t data)
{
	Command_t cmd = Command::W_REGISTER;
	cmd.value |= reg.value;
	writeCommandSingleData(cmd, data);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
void
xpcc::Nrf24Phy<Spi, Csn, Ce>::setBits(NrfRegister_t reg, Flags_t flags)
{
	uint8_t old = readRegister(reg);

	writeRegister(reg, old | flags.value);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
void
xpcc::Nrf24Phy<Spi, Csn, Ce>::clearBits(NrfRegister_t reg, Flags_t flags)
{
	uint8_t old = readRegister(reg);

	writeRegister(reg, old & ~flags.value);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
void
xpcc::Nrf24Phy<Spi, Csn, Ce>::flushTxFifo()
{
	writeCommandNoData(Command::FLUSH_TX);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
void
xpcc::Nrf24Phy<Spi, Csn, Ce>::flushRxFifo()
{
	writeCommandNoData(Command::FLUSH_RX);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
uint8_t
xpcc::Nrf24Phy<Spi, Csn, Ce>::readRxPayloadWidth()
{
	return writeCommandSingleData(Command::R_RX_PL_WID, 0x00);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
uint8_t
xpcc::Nrf24Phy<Spi, Csn, Ce>::readRxPayload(uint8_t* buffer)
{
	uint8_t cur_pl_len = payload_len;

	// cope with dynamic payload
	if(cur_pl_len == 0)
	{
		cur_pl_len = readRxPayloadWidth();
	}

	writeCommandMultiData(Command::R_RX_PAYLOAD, nullptr, buffer, cur_pl_len);


	return cur_pl_len;
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
void
xpcc::Nrf24Phy<Spi, Csn, Ce>::writeTxPayload(uint8_t* buffer, uint8_t len)
{
	if(len > payload_len)
		return;

	writeCommandMultiData(Command::W_TX_PAYLOAD, buffer, nullptr, len);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
void
xpcc::Nrf24Phy<Spi, Csn, Ce>::writeTxPayloadNoAck(uint8_t* buffer, uint8_t len)
{
	if(len > payload_len)
		return;
	writeCommandMultiData(Command::W_TX_PAYLOAD_NOACK, buffer, nullptr, len);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
void
xpcc::Nrf24Phy<Spi, Csn, Ce>::writeAckPayload(Pipe_t pipe, uint8_t* buffer, uint8_t len)
{
	if(len > payload_len)
		return;
	Command_t cmd = Command::W_ACK_PAYLOAD;
	cmd.value |= pipe.value;
	writeCommandMultiData(cmd, buffer, nullptr, len);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
void
xpcc::Nrf24Phy<Spi, Csn, Ce>::reuseTxPayload()
{
	writeCommandNoData(Command::REUSE_TX_PL);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
uint8_t
xpcc::Nrf24Phy<Spi, Csn, Ce>::readStatus()
{
	writeCommandNoData(Command::NOP);
	return status;
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
uint8_t
xpcc::Nrf24Phy<Spi, Csn, Ce>::readFifoStatus()
{
	return readRegister(NrfRegister::FIFO_STATUS);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
void
xpcc::Nrf24Phy<Spi, Csn, Ce>::setRxAddress(Pipe_t pipe, uint64_t address)
{

	Command_t cmd = Command::W_REGISTER;
	cmd.value |= static_cast<uint8_t>(NrfRegister::RX_ADDR_P0);

	// add pipe value so register will be RX_ADDR_Px if pipe is 'x'
	cmd.value += pipe.value;

	if(pipe.value <= 1)
	{
		/* register RX_ADDR_P0|1 are 40 bit wide */

		uint8_t addr[address_size];

		/* assemble address */
		for(uint8_t i = 0; i < address_size; i++)
		{
			addr[i] = address & 0xff;
			address >>= 8;
		}

		writeCommandMultiData(cmd, addr, nullptr, address_size);

	} else if(pipe.value < rx_pipe_count)
	{
		NrfRegister_t reg = NrfRegister::RX_ADDR_P0;
		reg.value += pipe.value;

		/* register RX_ADDR_P2-5 are just 8 bit wide */
		writeRegister(reg, static_cast<uint8_t>(address & 0xff));

	}
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
void
xpcc::Nrf24Phy<Spi, Csn, Ce>::setTxAddress(uint64_t address)
{
	uint8_t addr[address_size];

	for(uint8_t i = 0; i < address_size; i++)
	{
		addr[i] = address & 0xff;
		address >>= 8;
	}

	Command_t cmd = Command::W_REGISTER;
	cmd.value |= static_cast<uint8_t>(NrfRegister::TX_ADDR);

	writeCommandMultiData(cmd, addr, nullptr, address_size);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
uint64_t
xpcc::Nrf24Phy<Spi, Csn, Ce>::getRxAddress(Pipe_t pipe)
{
	if(pipe.value <= 1)
	{
		/* register RX_ADDR_P0|1 are 40 bit wide, so they need special treatment */

		Command_t cmd = Command::R_REGISTER;
		cmd.value |= static_cast<uint8_t>(NrfRegister::RX_ADDR_P0);

		// add pipe value so register will be RX_ADDR_Px if pipe is 'x'
		cmd.value += pipe.value;

		uint8_t addr[address_size];
		uint64_t address = 0;

		writeCommandMultiData(cmd, nullptr, addr,  address_size);

		uint8_t i = address_size;
		do
		{
			i--;
			address <<= 8;
			address |= addr[i];
		} while(i != 0);

		return address;

	} else if(pipe.value <= address_size)
	{
		NrfRegister_t reg = NrfRegister::RX_ADDR_P0;
		reg.value += pipe.value;

		/* register RX_ADDR_P2-5 are just 8 bit wide */
		return readRegister(reg);

	}

	/* Returning 0 as error indicator is totally okay because this address is not allowed by the datasheet anyway */
	return 0;
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
uint64_t
xpcc::Nrf24Phy<Spi, Csn, Ce>::getTxAddress()
{
	uint8_t addr[address_size];
	uint64_t address = 0;

	Command_t cmd = Command::R_REGISTER;
	cmd.value |= static_cast<uint8_t>(NrfRegister::TX_ADDR);

	writeCommandMultiData(cmd, nullptr, addr, address_size);

	uint8_t i = address_size;
	do
	{
		i--;
		address <<= 8;
		address |= addr[i];
	} while(i != 0);

	return address;
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
void
xpcc::Nrf24Phy<Spi, Csn, Ce>::pulseCe()
{
	Ce::set();

	xpcc::delayMicroseconds(10);

	Ce::reset();
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
void
xpcc::Nrf24Phy<Spi, Csn, Ce>::setCe()
{
	Ce::set();
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
void
xpcc::Nrf24Phy<Spi, Csn, Ce>::resetCe()
{
	Ce::reset();
}
