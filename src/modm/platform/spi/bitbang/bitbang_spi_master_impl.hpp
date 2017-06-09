/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2012-2017, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_SOFTWARE_BITBANG_SPI_MASTER_HPP
#	error	"Don't include this file directly, use 'bitbang_spi_master.hpp' instead!"
#endif

template <typename Sck, typename Mosi, typename Miso>
uint16_t
modm::platform::BitBangSpiMaster<Sck, Mosi, Miso>::delayTime(1);

template <typename Sck, typename Mosi, typename Miso>
uint8_t
modm::platform::BitBangSpiMaster<Sck, Mosi, Miso>::operationMode(0);

template <typename Sck, typename Mosi, typename Miso>
uint8_t
modm::platform::BitBangSpiMaster<Sck, Mosi, Miso>::count(0);

template <typename Sck, typename Mosi, typename Miso>
void *
modm::platform::BitBangSpiMaster<Sck, Mosi, Miso>::context(nullptr);

template <typename Sck, typename Mosi, typename Miso>
modm::Spi::ConfigurationHandler
modm::platform::BitBangSpiMaster<Sck, Mosi, Miso>::configuration(nullptr);
// ----------------------------------------------------------------------------

template <typename Sck, typename Mosi, typename Miso>
template< class SystemClock, uint32_t baudrate, uint16_t tolerance >
void
modm::platform::BitBangSpiMaster<Sck, Mosi, Miso>::initialize()
{
	delayTime = 500000000 / baudrate;
	if (delayTime == 0) delayTime = 1;

	Sck::reset();
	Mosi::reset();
}

template <typename Sck, typename Mosi, typename Miso>
void
modm::platform::BitBangSpiMaster<Sck, Mosi, Miso>::setDataMode(DataMode mode)
{
	operationMode = (operationMode & ~0b11) | static_cast<uint8_t>(mode);
	Sck::set(operationMode & 0b10);
}

template <typename Sck, typename Mosi, typename Miso>
void
modm::platform::BitBangSpiMaster<Sck, Mosi, Miso>::setDataOrder(DataOrder order)
{
	if (order == DataOrder::LsbFirst)
		operationMode |= 0b100;
	else
		operationMode &= ~0b100;
}
// ----------------------------------------------------------------------------

template <typename Sck, typename Mosi, typename Miso>
uint8_t
modm::platform::BitBangSpiMaster<Sck, Mosi, Miso>::acquire(void *ctx, ConfigurationHandler handler)
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

template <typename Sck, typename Mosi, typename Miso>
uint8_t
modm::platform::BitBangSpiMaster<Sck, Mosi, Miso>::release(void *ctx)
{
	if (ctx == context)
	{
		if (--count == 0)
			context = nullptr;
	}
	return count;
}
// ----------------------------------------------------------------------------

template <typename Sck, typename Mosi, typename Miso>
uint8_t
modm::platform::BitBangSpiMaster<Sck, Mosi, Miso>::transferBlocking(uint8_t data)
{
	for (uint_fast8_t ii = 0; ii < 8; ++ii)
	{
		// CPHA=1, sample on falling edge
		if (operationMode & 0b01)
			delay();

		// if LSB first
		if (operationMode & 0b100) {
			Mosi::set(data & 0x01);
			data >>= 1;
		}
		else {
			Mosi::set(data & 0x80);
			data <<= 1;
		}

		// CPHA=0, sample on rising edge
		if (not (operationMode & 0b01))
			delay();

		// CPOL=0 -> High, CPOL=1 -> Low
		Sck::set(not (operationMode & 0b10));

		// CPHA=1, sample on falling edge
		if (operationMode & 0b01)
			delay();

		// if LSB first
		if (operationMode & 0b100) {
			if (Miso::read()) data |= 0x80;
		}
		else {
			if (Miso::read()) data |= 0x01;
		}

		// CPHA=0, sample on rising edge
		if (not (operationMode & 0b01))
			delay();

		// CPOL=0 -> Low, CPOL=1 -> High
		Sck::set(operationMode & 0b10);
	}

	return data;
}

template <typename Sck, typename Mosi, typename Miso>
void
modm::platform::BitBangSpiMaster<Sck, Mosi, Miso>::transferBlocking(
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

template <typename Sck, typename Mosi, typename Miso>
modm::ResumableResult<uint8_t>
modm::platform::BitBangSpiMaster<Sck, Mosi, Miso>::transfer(uint8_t data)
{
	data = transferBlocking(data);
	return {modm::rf::Stop, data};
}

template <typename Sck, typename Mosi, typename Miso>
modm::ResumableResult<void>
modm::platform::BitBangSpiMaster<Sck, Mosi, Miso>::transfer(
		uint8_t *tx, uint8_t *rx, std::size_t length)
{
	transferBlocking(tx, rx, length);
	return {modm::rf::Stop, 0};
}

// ----------------------------------------------------------------------------

template <typename Sck, typename Mosi, typename Miso>
void modm_always_inline
modm::platform::BitBangSpiMaster<Sck, Mosi, Miso>::delay()
{
	modm::delayNanoseconds(delayTime);
}
