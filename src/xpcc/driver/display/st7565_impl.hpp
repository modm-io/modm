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

#ifndef XPCC__ST7565_HPP
	#error	"Don't include this file directly, use 'st7565.hpp' instead!"
#endif

#include "st7565_defines.hpp"

// ----------------------------------------------------------------------------
template <typename SPI, typename CS, typename A0, typename Reset, unsigned int Width, unsigned int Height, bool TopView>
void
xpcc::St7565<SPI, CS, A0, Reset, Width, Height, TopView>::update()
{
	cs.reset();
	for(uint8_t y = 0; y < (Height / 8); ++y)
	{
		// command mode
		a0.reset();
		spi.write(ST7565_PAGE_ADDRESS | y);		// Row select
		spi.write(ST7565_COL_ADDRESS_MSB);		// Column select high
		
		if (TopView) {
			spi.write(ST7565_COL_ADDRESS_LSB | 4);	// Column select low
		}
		else {
			spi.write(ST7565_COL_ADDRESS_LSB);	// Column select low
		}
		
		// switch to data mode
		a0.set();
		for(uint8_t x = 0; x < Width; ++x) {
			spi.write(this->buffer[x][y]);
		}
	}
	cs.set();
}

template <typename SPI, typename CS, typename A0, typename Reset, unsigned int Width, unsigned int Height, bool TopView>
void
xpcc::St7565<SPI, CS, A0, Reset, Width, Height, TopView>::setInvert(bool invert)
{
	cs.reset();
	a0.reset();
	
	if (invert) {
		spi.write(ST7565_REVERSE);
	}
	else {
		spi.write(ST7565_NORMAL);
	}
	cs.set();
}

// ----------------------------------------------------------------------------
template <typename SPI, typename CS, typename A0, typename Reset, unsigned int Width, unsigned int Height, bool TopView>
void
xpcc::St7565<SPI, CS, A0, Reset, Width, Height, TopView>::initialize(
		xpcc::accessor::Flash<uint8_t> configuration, uint8_t size)
{
	//spi.initialize();
	cs.set();
	cs.setOutput();
	
	a0.setOutput();
	
	// reset the controller
	reset.setOutput();
	reset.reset();
	xpcc::delay_ms(50);
	reset.set();
	
	cs.reset();
	a0.reset();
	
	// View direction
	if (TopView) {
		spi.write(ST7565_ADC_NORMAL);		// ADC normal
		spi.write(ST7565_SCAN_DIR_REVERSE);	// reverse COM0~COM63
	}
	else {
		spi.write(ST7565_ADC_REVERSE);
		spi.write(ST7565_SCAN_DIR_NORMAL);
	}
	
	for (uint8_t i = 0; i < size; ++i) {
		spi.write(configuration[i]);
	}
	
	cs.set();
	
	this->clear();
	this->update();
}
