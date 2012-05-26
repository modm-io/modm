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
uint8_t* xpcc::SoftwareSpi<Clk, Mosi, Miso, Frequency>::transmitBuffer(0);

template <typename Clk, typename Mosi, typename Miso, int32_t Frequency>
uint8_t* xpcc::SoftwareSpi<Clk, Mosi, Miso, Frequency>::receiveBuffer(0);

template <typename Clk, typename Mosi, typename Miso, int32_t Frequency>
uint16_t xpcc::SoftwareSpi<Clk, Mosi, Miso, Frequency>::bufferLength(0);

template <typename Clk, typename Mosi, typename Miso, int32_t Frequency>
uint8_t xpcc::SoftwareSpi<Clk, Mosi, Miso, Frequency>::status(0);

// ----------------------------------------------------------------------------
template <typename Clk, typename Mosi, typename Miso, int32_t Frequency>
Clk xpcc::SoftwareSpi<Clk, Mosi, Miso, Frequency>::clk;

template <typename Clk, typename Mosi, typename Miso, int32_t Frequency>
Mosi xpcc::SoftwareSpi<Clk, Mosi, Miso, Frequency>::mosi;

template <typename Clk, typename Mosi, typename Miso, int32_t Frequency>
Miso xpcc::SoftwareSpi<Clk, Mosi, Miso, Frequency>::miso;

// ----------------------------------------------------------------------------
template <typename Clk, typename Mosi, typename Miso, int32_t Frequency>
void
xpcc::SoftwareSpi<Clk, Mosi, Miso, Frequency>::initialize()
{
	clk.setOutput();
	mosi.setOutput();
	miso.setInput();
}

// ----------------------------------------------------------------------------
template <typename Clk, typename Mosi, typename Miso, int32_t Frequency>
bool
xpcc::SoftwareSpi<Clk, Mosi, Miso, Frequency>::setBuffer(uint16_t length, uint8_t* transmit, uint8_t* receive, BufferIncrease bufferIncrease)
{
	if (!isFinished()) return false;
	
	transmitBuffer = transmit;
	receiveBuffer = receive ? receive : transmit;
	bufferLength = length;
	status &= ~(BUFFER_TRANSMIT_INCR_bm | BUFFER_RECEIVE_INCR_bm);
	status |= bufferIncrease;
	
	return true;
}

template <typename Clk, typename Mosi, typename Miso, int32_t Frequency>
bool
xpcc::SoftwareSpi<Clk, Mosi, Miso, Frequency>::transfer(TransferOptions options)
{
	if (status & BUFFER_IS_BUSY_SYNC_bm)
		return false;
	
	uint8_t rx(0), tx(0xff);
	// send the buffer out, blocking
	status |= BUFFER_IS_BUSY_SYNC_bm;
	// check if we have to use a dummy buffer
	bool transmit = (options & TRANSFER_SEND_BUFFER_DISCARD_RECEIVE) & static_cast<bool>(transmitBuffer);
	bool receive = (options & TRANSFER_SEND_DUMMY_SAVE_RECEIVE) & static_cast<bool>(receiveBuffer);
	
	for(uint_fast16_t i=0; i < bufferLength; ++i)
	{
		if (transmit) {
			tx = transmitBuffer[(status & BUFFER_TRANSMIT_INCR_bm) ? i : bufferLength-1-i];
		}
		
		rx = write(tx);
		
		if (receive) {
			receiveBuffer[(status & BUFFER_RECEIVE_INCR_bm) ? i : bufferLength-1-i] = rx;
		}
	}
	
	status &= ~BUFFER_IS_BUSY_SYNC_bm;
	
	return true;
}

template <typename Clk, typename Mosi, typename Miso, int32_t Frequency>
bool
xpcc::SoftwareSpi<Clk, Mosi, Miso, Frequency>::transferSync(TransferOptions options)
{
	return transfer(options);
}

template <typename Clk, typename Mosi, typename Miso, int32_t Frequency>
bool
xpcc::SoftwareSpi<Clk, Mosi, Miso, Frequency>::isFinished()
{
	return !(status & BUFFER_IS_BUSY_SYNC_bm);
}

// ----------------------------------------------------------------------------
template <typename Clk, typename Mosi, typename Miso, int32_t Frequency>
uint8_t
xpcc::SoftwareSpi<Clk, Mosi, Miso, Frequency>::write(uint8_t output)
{
	uint8_t input = 0;
	
	clk.reset();
	for (uint8_t i = 0; i < 8; ++i)
	{
		input <<= 1;
		if (output & 0x80) {
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
		output <<= 1;
		
		clk.reset();
	}
	
	return input;
}

template <typename Clk, typename Mosi, typename Miso, int32_t Frequency>
void
xpcc::SoftwareSpi<Clk, Mosi, Miso, Frequency>::delay()
{
	xpcc::delay_us(delayTime);
}
