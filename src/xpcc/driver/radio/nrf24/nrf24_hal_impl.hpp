// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__NRF24_HPP
#	error "Don't include this file directly, use 'nrf24_hal.hpp' instead!"
#endif

#include "nrf24_hal.hpp"


template <typename Spi, typename Csn>
uint8_t xpcc::Nrf24Hal<Spi, Csn>::status;

template <typename Spi, typename Csn>
uint8_t xpcc::Nrf24Hal<Spi, Csn>::payload_len;


// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn>
void
xpcc::Nrf24Hal<Spi, Csn>::writeCommandNoData(nrf24::Command cmd)
{
	Csn::reset();

	status = Spi::writeReadBlocking(cmd);

	Csn::set();
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn>
uint8_t
xpcc::Nrf24Hal<Spi, Csn>::writeCommandSingleData(nrf24::Command cmd, uint8_t data)
{
	Csn::reset();

	status = Spi::writeReadBlocking(cmd);

	uint8_t ret = Spi::writeReadBlocking(data);

	Csn::set();

	return ret;
}

// --------------------------------------------------------------------------------------------------------------------

/*
 * TODO: maybe optimize this
 */
template<typename Spi, typename Csn>
void
xpcc::Nrf24Hal<Spi, Csn>::writeCommandMultiData(nrf24::Command cmd, uint8_t* argv, uint8_t* retv, uint8_t argc)
{
	Csn::reset();

	status = Spi::writeReadBlocking(cmd);

	uint8_t i;
	for(i = 0; i < argc; i++)
	{
		uint8_t data;
		uint8_t arg = 0;

		if(argv != nullptr)
		{
			arg = argv[i];
		}

		data = Spi::writeReadBlocking(arg);

		if(retv != nullptr)
		{
			retv[i] = data;
		}
	}

	Csn::set();
}

// --------------------------------------------------------------------------------------------------------------------


template<typename Spi, typename Csn>
uint8_t
xpcc::Nrf24Hal<Spi, Csn>::readRegister(nrf24::Register reg)
{
	return writeCommandSingleData(toCommand(nrf24::Command::R_REGISTER, reg, 0), 0x00);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn>
void
xpcc::Nrf24Hal<Spi, Csn>::writeRegister(nrf24::Register reg, uint8_t data)
{
	writeCommandSingleData(toCommand(nrf24::Command::W_REGISTER, reg, 0), data);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn>
void
xpcc::Nrf24Hal<Spi, Csn>::setBits(nrf24::Register reg, uint8_t flags)
{
	uint8_t old = readRegister(reg);

	writeRegister(reg, old | flags);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn>
void
xpcc::Nrf24Hal<Spi, Csn>::clearBits(nrf24::Register reg, uint8_t flags)
{
	uint8_t old = readRegister(reg);

	writeRegister(reg, old & ~flags);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn>
void
xpcc::Nrf24Hal<Spi, Csn>::flushTxFifo()
{
	writeCommandNoData(nrf24::Command::FLUSH_TX);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn>
void
xpcc::Nrf24Hal<Spi, Csn>::flushRxFifo()
{
	writeCommandNoData(nrf24::Command::FLUSH_RX);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn>
uint8_t
xpcc::Nrf24Hal<Spi, Csn>::readRxPayloadWidth()
{
	return writeCommandSingleData(nrf24::Command::R_RX_PL_WID, 0x00);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn>
uint8_t
xpcc::Nrf24Hal<Spi, Csn>::readRxPayload(uint8_t* buffer)
{
	uint8_t cur_pl_len = payload_len;

	// cope with dynamic payload
	if(cur_pl_len == 0)
	{
		cur_pl_len = readRxPayloadWidth();
	}

	writeCommandMultiData(nrf24::Command::R_RX_PAYLOAD, nullptr, buffer, cur_pl_len);


	return cur_pl_len;
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn>
void
xpcc::Nrf24Hal<Spi, Csn>::writeTxPayload(uint8_t* buffer, uint8_t len)
{
	if(len > payload_len)
		return;

	writeCommandMultiData(nrf24::Command::W_TX_PAYLOAD, buffer, nullptr, len);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn>
void
xpcc::Nrf24Hal<Spi, Csn>::writeTxPayloadNoAck(uint8_t* buffer, uint8_t len)
{
	if(len > payload_len)
		return;
	writeCommandMultiData(nrf24::Command::W_TX_PAYLOAD_NOACK, buffer, nullptr, len);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn>
void
xpcc::Nrf24Hal<Spi, Csn>::writeAckPayload(uint8_t pipe, uint8_t* buffer, uint8_t len)
{
	if(len > payload_len)
		return;
	writeCommandMultiData(toCommand(nrf24::Command::W_ACK_PAYLOAD, pipe), buffer, nullptr, len);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn>
void
xpcc::Nrf24Hal<Spi, Csn>::reuseTxPayload()
{
	writeCommandNoData(nrf24::Command::REUSE_TX_PL);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn>
uint8_t
xpcc::Nrf24Hal<Spi, Csn>::readStatus()
{
	writeCommandNoData(nrf24::Command::NOP);

	return status;
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn>
void
xpcc::Nrf24Hal<Spi, Csn>::setRxAddress(uint8_t pipe, uint64_t address)
{
	if(pipe <= 1)
	{
		/* register RX_ADDR_P0|1 are 40 bit wide */

		uint8_t addr[address_size];

		/* assemble address */
		for(uint8_t i = 0; i < address_size; i++)
		{
			addr[i] = address & 0xff;
			address >>= 8;
		}

		writeCommandMultiData(toCommand(nrf24::Command::W_REGISTER, nrf24::Register::RX_ADDR_P0, pipe), addr, nullptr, address_size);

	} else if(pipe < rx_pipe_count)
	{
		/* register RX_ADDR_P2-5 are just 8 bit wide */
		writeRegister(toRegister(nrf24::Register::RX_ADDR_P0, pipe), static_cast<uint8_t>(address & 0xff));

	}
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn>
void
xpcc::Nrf24Hal<Spi, Csn>::setTxAddress(uint64_t address)
{
	uint8_t addr[address_size];

	for(uint8_t i = 0; i < address_size; i++)
	{
		addr[i] = address & 0xff;
		address >>= 8;
	}

	writeCommandMultiData(toCommand(nrf24::Command::W_REGISTER, nrf24::Register::TX_ADDR, 0), addr, nullptr, address_size);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn>
uint64_t
xpcc::Nrf24Hal<Spi, Csn>::getRxAddress(uint8_t pipe)
{
	if(pipe <= 1)
	{
		/* register RX_ADDR_P0|1 are 40 bit wide */

		uint8_t addr[address_size];
		uint64_t address = 0;

		writeCommandMultiData(toCommand(nrf24::Command::R_REGISTER, nrf24::Register::RX_ADDR_P0, pipe),
								nullptr, addr,  address_size);

		uint8_t i = address_size;
		do
		{
			i--;
			address <<= 8;
			address |= addr[i];
		} while(i != 0);

		return address;

	} else if(pipe <= address_size)
	{
		/* register RX_ADDR_P2-5 are just 8 bit wide */
		return readRegister(toRegister(nrf24::Register::RX_ADDR_P0, pipe));

	}

	/* Returning 0 as error indicator is totally okay because this address is not allowed by the datasheet anyway */
	return 0;
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn>
uint64_t
xpcc::Nrf24Hal<Spi, Csn>::getTxAddress()
{
	uint8_t addr[address_size];
	uint64_t address = 0;

	writeCommandMultiData(toCommand(nrf24::Command::R_REGISTER, nrf24::Register::TX_ADDR, 0),
							nullptr, addr,  address_size);

	uint8_t i = address_size;
	do
	{
		i--;
		address <<= 8;
		address |= addr[i];
	} while(i != 0);

	return address;
}
