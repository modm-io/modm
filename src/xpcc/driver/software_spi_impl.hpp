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
 * $Id: software_spi.hpp 354 2010-07-14 10:21:50Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__SOFTWARE_SPI_HPP
	#error	"Don't include this file directly, use 'software_spi.hpp' instead!"
#endif

template <typename CLK, typename MOSI, typename MISO, int32_t FREQUENCY>
xpcc::SoftwareSpi<CLK, MOSI, MISO, FREQUENCY>::SoftwareSpi()
{
	initialize();
}

template <typename CLK, typename MOSI, typename MISO, int32_t FREQUENCY>
void
xpcc::SoftwareSpi<CLK, MOSI, MISO, FREQUENCY>::initialize()
{
	SCLK::setOutput();
	MOSI::setOutput();
	MISO::setInput();
}

template <typename CLK, typename MOSI, typename MISO, int32_t FREQUENCY>
uint8_t
xpcc::SoftwareSpi<CLK, MOSI, MISO, FREQUENCY>::write(uint8_t output)
{
	uint8_t input = 0;
	
	CLK::reset();
	for (uint8_t i = 0; i < 8; ++i)
	{
		input <<= 1;
		if (output & 0x80) {
			MOSI::set();
		}
		else {
			MOSI::reset();
		}
		xpcc::delay_us(delay);
		
		CLK::set();
		xpcc::delay_us(delay);
		
		if (MISO::read()) {
			input |= 1;
		}
		output <<= 1;
		
		CLK::reset();
	}
	
	return in;
}
