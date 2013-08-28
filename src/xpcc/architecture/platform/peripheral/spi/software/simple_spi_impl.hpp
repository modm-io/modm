// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_SOFTWARE_SIMPLE_SPI_HPP
#	error	"Don't include this file directly, use 'simple_spi.hpp' instead!"
#endif

#include <xpcc/architecture/driver/atomic.hpp>

template <typename SCK, typename MOSI, typename MISO, uint32_t Frequency>
uint8_t xpcc::SoftwareSimpleSpi<SCK, MOSI, MISO, Frequency>::timingMode(0);

template <typename SCK, typename MOSI, typename MISO, uint32_t Frequency>
bool xpcc::SoftwareSimpleSpi<SCK, MOSI, MISO, Frequency>::finished;

template <typename SCK, typename MOSI, typename MISO, uint32_t Frequency>
uint8_t xpcc::SoftwareSimpleSpi<SCK, MOSI, MISO, Frequency>::result;


// ----------------------------------------------------------------------------
template <typename SCK, typename MOSI, typename MISO, uint32_t Frequency>
template< uint32_t baudrate >
void
xpcc::SoftwareSimpleSpi<SCK, MOSI, MISO, Frequency>::initialize(Mode mode)
{
	xpcc::atomic::Lock lock;
	timingMode = static_cast<uint8_t>(mode);
	SCK::set(timingMode & 0b10);
	MOSI::reset();
	finished = true;
}

template <typename SCK, typename MOSI, typename MISO, uint32_t Frequency>
uint8_t
xpcc::SoftwareSimpleSpi<SCK, MOSI, MISO, Frequency>::writeReadBlocking(uint8_t data)
{
	while (!isFinished())
		;
	finished = false;

	uint8_t input = 0;

	SCK::reset();
	for (uint_fast8_t ii = 0; ii < 8; ++ii)
	{
		// CPHA=1, sample on falling edge
		if (timingMode & 0b01)
			delay();

		input <<= 1;
		if (data & 0x80) {
			MOSI::set();
		}
		else {
			MOSI::reset();
		}
		// CPHA=0, sample on rising edge
		if (!(timingMode & 0b01))
			delay();

		// CPOL=0 -> High, CPOL=1 -> High
		SCK::set(!(timingMode & 0b10));

		// CPHA=1, sample on falling edge
		if (timingMode & 0b01)
			delay();

		if (MISO::read()) {
			input |= 1;
		}
		data <<= 1;

		// CPHA=0, sample on rising edge
		if (!(timingMode & 0b01))
			delay();

		// CPOL=0 -> Low, CPOL=1 -> High
		SCK::set(timingMode & 0b10);
	}
	finished = true;

	return input;
}

template <typename SCK, typename MOSI, typename MISO, uint32_t Frequency>
void
xpcc::SoftwareSimpleSpi<SCK, MOSI, MISO, Frequency>::writeBlocking(uint8_t data)
{
	writeReadBlocking(data);
}

template <typename SCK, typename MOSI, typename MISO, uint32_t Frequency>
bool
xpcc::SoftwareSimpleSpi<SCK, MOSI, MISO, Frequency>::write(uint8_t data)
{
	result = writeReadBlocking(data);
	return true;
}

template <typename SCK, typename MOSI, typename MISO, uint32_t Frequency>
uint8_t
xpcc::SoftwareSimpleSpi<SCK, MOSI, MISO, Frequency>::getResult()
{
	return result;
}

template <typename SCK, typename MOSI, typename MISO, uint32_t Frequency>
bool
xpcc::SoftwareSimpleSpi<SCK, MOSI, MISO, Frequency>::isFinished()
{
	return finished;
}

template <typename SCK, typename MOSI, typename MISO, uint32_t Frequency>
void
xpcc::SoftwareSimpleSpi<SCK, MOSI, MISO, Frequency>::delay()
{
	xpcc::delay_us(delayTime);
}

template <typename SCK, typename MOSI, typename MISO, uint32_t Frequency>
bool
xpcc::SoftwareSimpleSpi<SCK, MOSI, MISO, Frequency>::transfer(uint8_t *tx, uint8_t *rx, std::size_t length, BufferOptions options)
{
	if (!isFinished())
		return false;
	finished = false;

	uint8_t tx_byte = 0xff;
	uint8_t rx_byte;

	for (std::size_t i = 0; i < length; i++)
	{
		if (tx) tx_byte = tx[static_cast<uint8_t>(options) & 0b10 ? i : length-i-1];

		rx_byte = writeReadBlocking(tx_byte);

		if (rx) rx[static_cast<uint8_t>(options) & 0b01 ? i : length-i-1] = rx_byte;
	}

	finished = true;
	return true;
}
