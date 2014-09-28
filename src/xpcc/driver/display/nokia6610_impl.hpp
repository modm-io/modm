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

#ifndef XPCC__NOKIA6610_HPP
#error	"Don't include this file directly, use 'nokia6610.hpp' instead!"
#endif

#include "nokia6610_defines.hpp"

// ----------------------------------------------------------------------------
template <typename SPI, typename CS, typename Reset, bool GE12>
void
xpcc::Nokia6610<SPI, CS, Reset, GE12>::initialize()
{

	// CS pin
	CS::set();
	CS::setOutput();


	// Reset pin
	Reset::setOutput();
	Reset::reset();
	xpcc::delayMilliseconds(1);
	Reset::set();
	xpcc::delayMilliseconds(10);


	lcdSettings();

	this->clear();
//	this->update();
}

template <typename SPI, typename CS, typename Reset, bool GE12>
void
xpcc::Nokia6610<SPI, CS, Reset, GE12>::writeSpi9Bit(uint16_t data){
	// add new bits to temp
	temp = temp | ((data&0x1ff)<< (7-countValidBits));
	countValidBits += 9;

	while(countValidBits >= 8){
		SPI::write(temp>>8);
		countValidBits -= 8;
		temp <<= 8;
	}
}

template <typename SPI, typename CS, typename Reset, bool GE12>
void
xpcc::Nokia6610<SPI, CS, Reset, GE12>::writeSpi9BitFlush(){
	if (countValidBits > 0){
		SPI::write(temp>>8);
	}
	countValidBits = 0;
	temp = 0;
}


template <typename SPI, typename CS, typename Reset, bool GE12>
void
xpcc::Nokia6610<SPI, CS, Reset, GE12>::writeSpiCommand(uint16_t data){
	writeSpi9Bit(data & ~0x0100);
}

template <typename SPI, typename CS, typename Reset, bool GE12>
void
xpcc::Nokia6610<SPI, CS, Reset, GE12>::writeSpiData(uint16_t data){
	writeSpi9Bit(data | 0x0100);
}

template <typename SPI, typename CS, typename Reset, bool GE12>
void
xpcc::Nokia6610<SPI, CS, Reset, GE12>::setContrast(uint8_t contrast) {
	CS::reset();

	if (GE12){
	}
	else{
		writeSpiCommand(nokia::NOKIA_GE8_VOLCTR);
		writeSpiData(contrast); // contrast
		writeSpiData(3);
	}

	writeSpi9BitFlush();

	CS::set();
}

template <typename SPI, typename CS, typename Reset, bool GE12>
void
xpcc::Nokia6610<SPI, CS, Reset, GE12>::lcdSettings() {
	CS::reset();

	if( GE12){
	}
	else{
		// Hardware reset
		Reset::reset();
		xpcc::delayMilliseconds(50);
		Reset::set();
		xpcc::delayMilliseconds(50);

		// Display vontrol
		writeSpiCommand(nokia::NOKIA_GE8_DISCTL);
		writeSpiData(0x00); // default
		writeSpiData(0x20); // (32 + 1) * 4 = 132 lines (of which 130 are visible)
		writeSpiData(0x0a); // default

		// COM scan
		writeSpiCommand(nokia::NOKIA_GE8_COMSCN);
		writeSpiData(0x00);  // Scan 1-80

		// Internal oscilator ON
		writeSpiCommand(nokia::NOKIA_GE8_OSCON);

		writeSpi9BitFlush();

		CS::set();
		// wait aproximetly 100ms
		xpcc::delayMilliseconds(100);
		CS::reset();

		// Sleep out
		writeSpiCommand(nokia::NOKIA_GE8_SLPOUT);

		// Voltage control
		writeSpiCommand(nokia::NOKIA_GE8_VOLCTR);
		writeSpiData(43); // middle value of V1
		writeSpiData(0x03); // middle value of resistance value

		// Power control
		writeSpiCommand(nokia::NOKIA_GE8_PWRCTR);
		writeSpiData(0x0f);   // referance voltage regulator on, circuit voltage follower on, BOOST ON

		// Data control
		writeSpiCommand(nokia::NOKIA_GE8_DATCTL);
		writeSpiData(0x04); // page scan
		writeSpiData(0x00); // RGB sequence
		writeSpiData(0x02); // 12bit per pixel mode A others may not work

		// Page Address set
		writeSpiCommand(nokia::NOKIA_GE8_PASET);
		writeSpiData(2); // start at 2 others corrupt display settings in a unpredictable way
		writeSpiData(2 + this->getHeight()-1 + 2); // 2 more for filling, but not handled by buffered display

		// Page Column set
		writeSpiCommand(nokia::NOKIA_GE8_CASET);
		writeSpiData(0);
		writeSpiData(0 + this->getWidth()-1);

		// Display On
		writeSpiCommand(nokia::NOKIA_GE8_DISON);

	}

	writeSpi9BitFlush();
	CS::set();
}

template <typename SPI, typename CS, typename Reset, bool GE12>
void
xpcc::Nokia6610<SPI, CS, Reset, GE12>::update() {
	CS::reset();

	if (GE12)
	{
		return;
	}
	else
	{
		// Display OFF
		//writeSpiCommand(nokia::NOKIA_GE8_DISOFF);

		// WRITE MEMORY
		writeSpiCommand(nokia::NOKIA_GE8_RAMWR);
	}

	static const uint32_t mask1Blank  = 0x00f000; // RGB000
	static const uint32_t mask1Filled = 0xff0000; // RGB000

	static const uint32_t mask2Blank = mask1Blank>>12; // 000RGB
	static const uint32_t mask2Filled = mask1Filled>>12; // 000RGB

	for(uint8_t x = 0; x < this->getWidth(); ++x)
	{
		for(uint8_t y = 0; y < this->getHeight()/8; ++y) {
			uint8_t group = this->buffer[x][y];
			for (uint8_t pix = 0; pix < 8; pix+=2, group>>=2){
				uint32_t data =
						((group&1)?mask1Filled:mask1Blank)|
						((group&2)?mask2Filled:mask2Blank);

				writeSpiData(data>>16);
				writeSpiData(data>>8);
				writeSpiData(data);
			}
		}
		// fill pixel not handled by buffered display
		uint32_t data = mask1Blank | mask2Blank;
		writeSpiData(data>>16);
		writeSpiData(data>>8);
		writeSpiData(data);
	}

	if (GE12)
	{
	}
	else
	{
		// wait approximately 100ms
		xpcc::delayMilliseconds(100);

		// Display On
		writeSpiCommand(nokia::NOKIA_GE8_DISON);
	}

	writeSpi9BitFlush();
	CS::reset();
}
