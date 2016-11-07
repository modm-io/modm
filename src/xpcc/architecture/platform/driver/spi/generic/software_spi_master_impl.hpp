/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2012-2016, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_SOFTWARE_SPI_MASTER_HPP
#	error	"Don't include this file directly, use 'simple_spi.hpp' instead!"
#endif

template <typename SCK, typename MOSI, typename MISO>
uint16_t
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO>::delayTime(1);

template <typename SCK, typename MOSI, typename MISO>
uint8_t
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO>::operationMode(0);

template <typename SCK, typename MOSI, typename MISO>
uint8_t
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO>::count(0);

template <typename SCK, typename MOSI, typename MISO>
void *
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO>::context(nullptr);

template <typename SCK, typename MOSI, typename MISO>
xpcc::Spi::ConfigurationHandler
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO>::configuration(nullptr);
// ----------------------------------------------------------------------------

template <typename SCK, typename MOSI, typename MISO>
template< class SystemClock, uint32_t baudrate, uint16_t tolerance >
void
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO>::initialize()
{
	delayTime = 500000000 / baudrate;
	if (delayTime == 0) delayTime = 1;

	SCK::reset();
	MOSI::reset();
}

template <typename SCK, typename MOSI, typename MISO>
void
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO>::setDataMode(DataMode mode)
{
	operationMode = (operationMode & ~0b11) | static_cast<uint8_t>(mode);
	SCK::set(operationMode & 0b10);
}

template <typename SCK, typename MOSI, typename MISO>
void
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO>::setDataOrder(DataOrder order)
{
	if (order == DataOrder::LsbFirst)
		operationMode |= 0b100;
	else
		operationMode &= ~0b100;
}
// ----------------------------------------------------------------------------

template <typename SCK, typename MOSI, typename MISO>
uint8_t
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO>::acquire(void *ctx, ConfigurationHandler handler)
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

template <typename SCK, typename MOSI, typename MISO>
uint8_t
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO>::release(void *ctx)
{
	if (ctx == context)
	{
		if (--count == 0)
			context = nullptr;
	}
	return count;
}
// ----------------------------------------------------------------------------

template <typename SCK, typename MOSI, typename MISO>
uint8_t
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO>::transferBlocking(uint8_t data)
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
		if (not (operationMode & 0b01))
			delay();

		// CPOL=0 -> High, CPOL=1 -> Low
		SCK::set(not (operationMode & 0b10));

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
		if (not (operationMode & 0b01))
			delay();

		// CPOL=0 -> Low, CPOL=1 -> High
		SCK::set(operationMode & 0b10);
	}

	return data;
}

template <typename SCK, typename MOSI, typename MISO>
void
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO>::transferBlocking(
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

template <typename SCK, typename MOSI, typename MISO>
xpcc::ResumableResult<uint8_t>
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO>::transfer(uint8_t data)
{
	data = transferBlocking(data);
	return {xpcc::rf::Stop, data};
}

template <typename SCK, typename MOSI, typename MISO>
xpcc::ResumableResult<void>
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO>::transfer(
		uint8_t *tx, uint8_t *rx, std::size_t length)
{
	transferBlocking(tx, rx, length);
	return {xpcc::rf::Stop, 0};
}

// ----------------------------------------------------------------------------

template <typename SCK, typename MOSI, typename MISO>
void xpcc_always_inline
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO>::delay()
{
	xpcc::delayNanoseconds(delayTime);
}
