#ifndef XPCC__NRF24_HPP
#	error "Don't include this file directly, use 'nrf24.hpp' instead!"
#endif

#include "nrf24.hpp"

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
void
xpcc::Nrf24Hal<Spi, Csn, Ce>::writeCommandNoData(nrf24::Command cmd)
{
	Csn::reset();

	status = Spi::writeReadBlocking(cmd);

	Csn::set();
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
uint8_t
xpcc::Nrf24Hal<Spi, Csn, Ce>::writeCommandSingleData(nrf24::Command cmd, uint8_t data)
{
	Csn::reset();

	status = Spi::writeReadBlocking(cmd);

	uint8_t ret = Spi::writeReadBlocking(data);

	Csn::set();

	return ret;
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
void
xpcc::Nrf24Hal<Spi, Csn, Ce>::writeCommandMultiData(nrf24::Command cmd, uint8_t* argv, uint8_t* retv, uint8_t argc)
{
	Csn::reset();

	status = Spi::writeReadBlocking(cmd);

	uint8_t i;
	for(i = 0; i < argc; i++)
	{
		retv[i] = Spi::writeReadBlocking(argv[i]);
	}

	Csn::set();
}

// --------------------------------------------------------------------------------------------------------------------


template<typename Spi, typename Csn, typename Ce>
uint8_t
xpcc::Nrf24Hal<Spi, Csn, Ce>::readRegister(nrf24::Register reg)
{
	return writeCommandSingleData(nrf24::Command::R_REGISTER | reg, 0x00);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
void
xpcc::Nrf24Hal<Spi, Csn, Ce>::writeRegister(nrf24::Register reg, uint8_t data)
{
	writeCommandSingleData(nrf24::Command::W_REGISTER | reg, data);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
void
xpcc::Nrf24Hal<Spi, Csn, Ce>::flushTxFifo()
{
	writeCommandNoData(nrf24::Command::FLUSH_TX);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
void
xpcc::Nrf24Hal<Spi, Csn, Ce>::flushRxFifo()
{
	writeCommandNoData(nrf24::Command::FLUSH_RX);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
uint8_t
xpcc::Nrf24Hal<Spi, Csn, Ce>::readRxPayloadWidth()
{
	return writeCommandSingleData(nrf24::Command::R_RX_PL_WID, 0x00);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
uint8_t
xpcc::Nrf24Hal<Spi, Csn, Ce>::readRxPayload(uint8_t* buffer)
{
	uint8_t cur_pl_len = payload_length;

	// cope with dynamic payload
	if(payload_length < 0)
	{
		cur_pl_len = readRxPayloadWidth();
	}

	writeCommandMultiData(nrf24::Command::R_RX_PAYLOAD, buffer, buffer, cur_pl_len);


	return cur_pl_len;
}

// --------------------------------------------------------------------------------------------------------------------

template<typename Spi, typename Csn, typename Ce>
void
xpcc::Nrf24Hal<Spi, Csn, Ce>::writeTxPayload(uint8_t* buffer, uint8_t len)
{
	writeCommandMultiData(nrf24::Command::W_TX_PAYLOAD, buffer, buffer, len);
}
