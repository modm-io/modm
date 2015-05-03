// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_SOFTWARE_SPI_MASTER_HPP
#	error	"Don't include this file directly, use 'simple_spi.hpp' instead!"
#endif

template <typename SCK, typename MOSI, typename MISO, uint32_t Baudrate>
uint8_t
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO, Baudrate>::operationMode(0);

template <typename SCK, typename MOSI, typename MISO, uint32_t Baudrate>
uint8_t
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO, Baudrate>::count(0);

template <typename SCK, typename MOSI, typename MISO, uint32_t Baudrate>
void *
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO, Baudrate>::context(nullptr);

template <typename SCK, typename MOSI, typename MISO, uint32_t Baudrate>
xpcc::Spi::ConfigurationHandler
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO, Baudrate>::configuration(nullptr);
// ----------------------------------------------------------------------------

template <typename SCK, typename MOSI, typename MISO, uint32_t Baudrate>
template< class clockSource, uint32_t baudrate, uint16_t tolerance >
void
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO, Baudrate>::initialize()
{
	SCK::reset();
	MOSI::reset();
}

template <typename SCK, typename MOSI, typename MISO, uint32_t Baudrate>
void
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO, Baudrate>::setDataMode(DataMode mode)
{
	operationMode = (operationMode & ~0b11) | static_cast<uint8_t>(mode);
	SCK::set(operationMode & 0b10);
}

template <typename SCK, typename MOSI, typename MISO, uint32_t Baudrate>
void
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO, Baudrate>::setDataOrder(DataOrder order)
{
	if (order == DataOrder::LsbFirst)
		operationMode |= 0b100;
	else
		operationMode &= ~0b100;
}
// ----------------------------------------------------------------------------

template <typename SCK, typename MOSI, typename MISO, uint32_t Baudrate>
uint8_t
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO, Baudrate>::aquire(void *ctx, ConfigurationHandler handler)
{
	if (ctx == nullptr)
	{
		context = ctx;
		count = 1;
		// if handler is not nullptr and is different from previous configuration
		if (handler and configuration != handler) {
			configuration = handler;
			configuration();
		}
		return 1;
	}

	if (ctx == context)
		return ++count;

	return 0;
}

template <typename SCK, typename MOSI, typename MISO, uint32_t Baudrate>
uint8_t
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO, Baudrate>::release(void *ctx)
{
	if (ctx == context)
	{
		if (--count == 0)
			context = nullptr;
	}
	return count;
}
// ----------------------------------------------------------------------------

template <typename SCK, typename MOSI, typename MISO, uint32_t Baudrate>
uint8_t
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO, Baudrate>::transferBlocking(uint8_t data)
{
	for (uint_fast8_t ii = 0; ii < 8; ++ii)
	{
		// CPHA=1, sample on falling edge
		if (operationMode & 0b01)
			delay();

		// if LSB first
		if (operationMode & 0b100) {
			MOSI::set(data & 0x01);
			data >>= 1;
		}
		else {
			MOSI::set(data & 0x80);
			data <<= 1;
		}

		// CPHA=0, sample on rising edge
		if (!(operationMode & 0b01))
			delay();

		// CPOL=0 -> High, CPOL=1 -> Low
		SCK::set(!(operationMode & 0b10));

		// CPHA=1, sample on falling edge
		if (operationMode & 0b01)
			delay();

		// if LSB first
		if (operationMode & 0b100) {
			if (MISO::read()) data |= 0x80;
		}
		else {
			if (MISO::read()) data |= 0x01;
		}

		// CPHA=0, sample on rising edge
		if (!(operationMode & 0b01))
			delay();

		// CPOL=0 -> Low, CPOL=1 -> High
		SCK::set(operationMode & 0b10);
	}

	return data;
}

template <typename SCK, typename MOSI, typename MISO, uint32_t Baudrate>
void
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO, Baudrate>::transferBlocking(
		uint8_t *tx, uint8_t *rx, std::size_t length)
{
	uint8_t tx_byte = 0xff;
	uint8_t rx_byte;

	for (std::size_t i = 0; i < length; i++)
	{
		if (tx) tx_byte = tx[i];

		rx_byte = transferBlocking(tx_byte);

		if (rx) rx[i] = rx_byte;
	}
}

template <typename SCK, typename MOSI, typename MISO, uint32_t Baudrate>
xpcc::co::ResumableResult<uint8_t>
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO, Baudrate>::transfer(uint8_t data)
{
	data = transferBlocking(data);
	return {xpcc::co::Stop, data};
}

template <typename SCK, typename MOSI, typename MISO, uint32_t Baudrate>
xpcc::co::ResumableResult<void>
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO, Baudrate>::transfer(
		uint8_t *tx, uint8_t *rx, std::size_t length)
{
	transferBlocking(tx, rx, length);
	return {xpcc::co::Stop, 0};
}

// ----------------------------------------------------------------------------

template <typename SCK, typename MOSI, typename MISO, uint32_t Baudrate>
void ALWAYS_INLINE
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO, Baudrate>::delay()
{
	xpcc::delayMicroseconds(delayTime);
}
