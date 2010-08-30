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
 *
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__SOFTWARE_I2C_HPP
	#error	"Don't include this file directly, use 'software_i2c.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename Scl, typename Sda, int32_t Frequency>
Scl xpcc::SoftwareI2C<Scl, Sda, Frequency>::scl;

template <typename Scl, typename Sda, int32_t Frequency>
Sda xpcc::SoftwareI2C<Scl, Sda, Frequency>::sda;

// ----------------------------------------------------------------------------
template <typename Scl, typename Sda, int32_t Frequency>
void
xpcc::SoftwareI2C<Scl, Sda, Frequency>::initialize()
{
	scl.setInput();
	scl.reset();
	
	sda.setInput();
	sda.reset();
}

template <typename Scl, typename Sda, int32_t Frequency>
bool
xpcc::SoftwareI2C<Scl, Sda, Frequency>::start(uint8_t address)
{
	sda.setInput();
	scl.setInput();
	delay();
	
	sda.setOutput();
	delay();
	scl.setOutput();
	delay();
	
	return write(address);
}

template <typename Scl, typename Sda, int32_t Frequency>
bool
xpcc::SoftwareI2C<Scl, Sda, Frequency>::repeatedStart(uint8_t address)
{
	scl.setOutput();
	delay();
	sda.setInput();
	delay();
	scl.setInput();
	delay();
	sda.setOutput();
	delay();
	scl.setOutput();
	delay();
	
	return write(address);
}

template <typename Scl, typename Sda, int32_t Frequency>
void
xpcc::SoftwareI2C<Scl, Sda, Frequency>::stop()
{
	scl.setOutput();
	sda.setOutput();
	
	delay();
	scl.setInput();
	delay();
	sda.setInput();
	delay();
}

// ----------------------------------------------------------------------------
template <typename Scl, typename Sda, int32_t Frequency>
bool
xpcc::SoftwareI2C<Scl, Sda, Frequency>::write(uint8_t data)
{
	for(uint8_t i = 0; i < 8; ++i)
	{
		writeBit(data & 0x80);
		data <<= 1;
	}
	
	// return acknowledge bit
	return !readBit();
}

template <typename Scl, typename Sda, int32_t Frequency>
uint8_t
xpcc::SoftwareI2C<Scl, Sda, Frequency>::read(bool ack)
{
	sda.setInput();
	
	uint8_t data = 0;
	for(uint8_t i = 0; i < 8; ++i)
	{
		data <<= 1;
		if (readBit()) {
			data |= 0x01;
		}
	}
	
	// generate acknowledge bit
	writeBit(!ack);
	
	return data;
}

// ----------------------------------------------------------------------------
template <typename Scl, typename Sda, int32_t Frequency>
bool
xpcc::SoftwareI2C<Scl, Sda, Frequency>::readBit()
{
	scl.setInput();
	delay();
	
	while (!scl.read())
		;
	
	bool bit = sda.read();
	
	scl.setOutput();
	delay();
	
	return bit;
}

template <typename Scl, typename Sda, int32_t Frequency>
void
xpcc::SoftwareI2C<Scl, Sda, Frequency>::writeBit(bool bit)
{
	if (bit) {
		sda.setOutput();
	}
	else {
		sda.setInput();
	}
	
	scl.setInput();
	delay();
	
	while (!scl.read())
		;
	
	scl.setOutput();
	delay();
	
	sda.setInput();
}

// ----------------------------------------------------------------------------
template <typename Scl, typename Sda, int32_t Frequency>
void
xpcc::SoftwareI2C<Scl, Sda, Frequency>::delay()
{
	xpcc::delay_us(delayTime);
}
