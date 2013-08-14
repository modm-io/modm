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

template <typename Clk, typename Mosi, typename Miso, uint32_t Frequency>
bool xpcc::SoftwareSimpleSpi<Clk, Mosi, Miso, Frequency>::finished;

template <typename Clk, typename Mosi, typename Miso, uint32_t Frequency>
uint8_t xpcc::SoftwareSimpleSpi<Clk, Mosi, Miso, Frequency>::result;


// ----------------------------------------------------------------------------
template <typename Clk, typename Mosi, typename Miso, uint32_t Frequency>
void
xpcc::SoftwareSimpleSpi<Clk, Mosi, Miso, Frequency>::initialize()
{
	Clk::setOutput();
	Mosi::setOutput();
	Miso::setInput();
	finished = true;
}

template <typename Clk, typename Mosi, typename Miso, uint32_t Frequency>
uint8_t
xpcc::SoftwareSimpleSpi<Clk, Mosi, Miso, Frequency>::writeReadBlocking(uint8_t data)
{
	while (!isFinished())
		;
	finished = false;

	uint8_t input = 0;

	Clk::reset();
	for (uint_fast8_t ii = 0; ii < 8; ++ii)
	{
		input <<= 1;
		if (data & 0x80) {
			Mosi::set();
		}
		else {
			Mosi::reset();
		}
		delay();

		Clk::set();
		delay();

		if (Miso::read()) {
			input |= 1;
		}
		data <<= 1;

		Clk::reset();
	}
	finished = true;

	return input;
}

template <typename Clk, typename Mosi, typename Miso, uint32_t Frequency>
void
xpcc::SoftwareSimpleSpi<Clk, Mosi, Miso, Frequency>::writeBlocking(uint8_t data)
{
	writeReadBlocking(data);
}

template <typename Clk, typename Mosi, typename Miso, uint32_t Frequency>
bool
xpcc::SoftwareSimpleSpi<Clk, Mosi, Miso, Frequency>::write(uint8_t data)
{
	result = writeReadBlocking(data);
	return true;
}

template <typename Clk, typename Mosi, typename Miso, uint32_t Frequency>
uint8_t
xpcc::SoftwareSimpleSpi<Clk, Mosi, Miso, Frequency>::getResult()
{
	return result;
}

template <typename Clk, typename Mosi, typename Miso, uint32_t Frequency>
bool
xpcc::SoftwareSimpleSpi<Clk, Mosi, Miso, Frequency>::isFinished()
{
	return finished;
}

template <typename Clk, typename Mosi, typename Miso, uint32_t Frequency>
void
xpcc::SoftwareSimpleSpi<Clk, Mosi, Miso, Frequency>::delay()
{
	xpcc::delay_us(delayTime);
}

template <typename Clk, typename Mosi, typename Miso, uint32_t Frequency>
bool
xpcc::SoftwareSimpleSpi<Clk, Mosi, Miso, Frequency>::transfer(uint8_t *tx, uint8_t *rx, std::size_t length, BufferOptions options)
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
