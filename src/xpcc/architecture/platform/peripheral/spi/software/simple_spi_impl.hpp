// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__SOFTWARE_SPI_HPP
	#error	"Don't include this file directly, use 'software_spi.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename Clk, typename Mosi, typename Miso, int32_t Frequency>
Clk xpcc::SoftwareSpiMaster<Clk, Mosi, Miso, Frequency>::clk;

template <typename Clk, typename Mosi, typename Miso, int32_t Frequency>
Mosi xpcc::SoftwareSpiMaster<Clk, Mosi, Miso, Frequency>::mosi;

template <typename Clk, typename Mosi, typename Miso, int32_t Frequency>
Miso xpcc::SoftwareSpiMaster<Clk, Mosi, Miso, Frequency>::miso;

// ----------------------------------------------------------------------------
template <typename Clk, typename Mosi, typename Miso, int32_t Frequency>
bool xpcc::SoftwareSpiMaster<Clk, Mosi, Miso, Frequency>::finished;

template <typename Clk, typename Mosi, typename Miso, int32_t Frequency>
uint8_t xpcc::SoftwareSpiMaster<Clk, Mosi, Miso, Frequency>::result;


// ----------------------------------------------------------------------------
template <typename Clk, typename Mosi, typename Miso, int32_t Frequency>
void
xpcc::SoftwareSpiMaster<Clk, Mosi, Miso, Frequency>::initialize()
{
	clk.setOutput();
	mosi.setOutput();
	miso.setInput();
	finished = true;
}

// ----------------------------------------------------------------------------
template <typename Clk, typename Mosi, typename Miso, int32_t Frequency>
uint8_t
xpcc::SoftwareSpiMaster<Clk, Mosi, Miso, Frequency>::writeReadBlocking(uint8_t data)
{
	if (!isFinished())
		return 0;
	finished = false;

	uint8_t input = 0;

	clk.reset();
	for (uint8_t i = 0; i < 8; ++i)
	{
		input <<= 1;
		if (data & 0x80) {
			mosi.set();
		}
		else {
			mosi.reset();
		}
		delay();

		clk.set();
		delay();

		if (miso.read()) {
			input |= 1;
		}
		data <<= 1;

		clk.reset();
	}
	finished = true;
	
	return input;
}

template <typename Clk, typename Mosi, typename Miso, int32_t Frequency>
void
xpcc::SoftwareSpiMaster<Clk, Mosi, Miso, Frequency>::writeBlocking(uint8_t data)
{
	writeReadBlocking(data);
}

template <typename Clk, typename Mosi, typename Miso, int32_t Frequency>
bool
xpcc::SoftwareSpiMaster<Clk, Mosi, Miso, Frequency>::write(uint8_t data)
{
	result = writeReadBlocking(data);
	return true;
}

template <typename Clk, typename Mosi, typename Miso, int32_t Frequency>
uint8_t
xpcc::SoftwareSpiMaster<Clk, Mosi, Miso, Frequency>::getResult()
{
	return result;
}

template <typename Clk, typename Mosi, typename Miso, int32_t Frequency>
bool
xpcc::SoftwareSpiMaster<Clk, Mosi, Miso, Frequency>::isFinished()
{
	return finished;
}

template <typename Clk, typename Mosi, typename Miso, int32_t Frequency>
void
xpcc::SoftwareSpiMaster<Clk, Mosi, Miso, Frequency>::delay()
{
	xpcc::delay_us(delayTime);
}


// ============================================================================
template <typename Clk, typename Mosi, typename Miso, int32_t Frequency>
bool xpcc::SoftwareSpiBlockMaster<Clk, Mosi, Miso, Frequency>::finished;


// ----------------------------------------------------------------------------
template <typename Clk, typename Mosi, typename Miso, int32_t Frequency>
void
xpcc::SoftwareSpiBlockMaster<Clk, Mosi, Miso, Frequency>::initialize()
{
	SoftwareSpiMaster< Clk, Mosi, Miso, Frequency >::initialize();
	finished = true;
}

template <typename Clk, typename Mosi, typename Miso, int32_t Frequency>
bool
xpcc::SoftwareSpiBlockMaster<Clk, Mosi, Miso, Frequency>::start(uint8_t * tx, uint8_t * rx, std::size_t length, BufferOptions options)
{
	if (!isFinished())
		return false;
	finished = false;

	uint8_t tx_byte = 0xff;
	uint8_t rx_byte;

	for (std::size_t i = 0; i < length; i++)
	{
		if (tx) tx_byte = tx[static_cast<uint8_t>(options) & 0b10 ? i : length-i-1];

		rx_byte = SoftwareSpiMaster< Clk, Mosi, Miso, Frequency >::writeReadBlocking(tx_byte);

		if (rx) rx[static_cast<uint8_t>(options) & 0b01 ? i : length-i-1] = rx_byte;
	}

	finished = true;
	return true;
}

template <typename Clk, typename Mosi, typename Miso, int32_t Frequency>
bool
xpcc::SoftwareSpiBlockMaster<Clk, Mosi, Miso, Frequency>::isFinished()
{
	return finished;
}
