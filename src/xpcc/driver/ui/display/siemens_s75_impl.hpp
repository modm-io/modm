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

#ifndef XPCC__SIEMENS_S75_HPP
#error	"Don't include this file directly, use 'siemens_s75.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename PORT, typename CS, typename RS, typename WR, typename Reset>
void
xpcc::SiemensS75Portrait<PORT, CS, RS, WR, Reset>::initialize()
{
	// CS pin
	CS::setOutput(true);

	// RS pin
	RS::setOutput(false);

	// WR Pin
	WR::setOutput(false);

	// Reset pin
	Reset::setOutput(false);

	SiemensS75Common<PORT, CS, RS, WR, Reset>::lcdSettings(false);

	this->clear();
}

// ----------------------------------------------------------------------------

template <typename PORT, typename CS, typename RS, typename WR, typename Reset>
void
xpcc::SiemensS75Landscape<PORT, CS, RS, WR, Reset>::initialize()
{
	// CS pin
	CS::setOutput(true);

	// RS pin
	RS::setOutput(false);

	// WR Pin
	WR::setOutput(false);

	// Reset pin
	Reset::setOutput(false);

	SiemensS75Common<PORT, CS, RS, WR, Reset>::lcdSettings(true);

	this->clear();
}

// ----------------------------------------------------------------------------

template <typename PORT, typename CS, typename RS, typename WR, typename Reset>
void
xpcc::SiemensS75Common<PORT, CS, RS, WR, Reset>::writeCmd(uint8_t reg, uint16_t param)
{
	writeReg(reg);
	writeData(param);
}

template <typename PORT, typename CS, typename RS, typename WR, typename Reset>
void
xpcc::SiemensS75Common<PORT, CS, RS, WR, Reset>::writeReg(uint8_t reg)
{
	RS::reset();
	xpcc::delay_us(100);

	CS::reset();

	PORT::write(0);
	xpcc::delay_us(100);
	WR::reset();	// High-to-low strobe
	xpcc::delay_us(100);
	WR::set();
	xpcc::delay_us(100);

	PORT::write(reg);
	xpcc::delay_us(100);
	WR::reset();	// High-to-low strobe
	xpcc::delay_us(100);
	WR::set();
	xpcc::delay_us(100);

	CS::set();
}

template <typename PORT, typename CS, typename RS, typename WR, typename Reset>
void
xpcc::SiemensS75Common<PORT, CS, RS, WR, Reset>::writeData(uint16_t data)
{
	RS::set();		// RS = 1, R/W = 0, write instruction or RAM data
	xpcc::delay_us(100);

	CS::reset();

	PORT::write(data>>8);
	xpcc::delay_us(100);
	WR::reset();	// High-to-low strobe
	xpcc::delay_us(100);
	WR::set();
	xpcc::delay_us(100);

	PORT::write(data);
	xpcc::delay_us(100);
	WR::reset();	// High-to-low strobe
	xpcc::delay_us(100);
	WR::set();
	xpcc::delay_us(100);

	CS::set();
}

template <typename PORT, typename CS, typename RS, typename WR, typename Reset>
void
xpcc::SiemensS75Common<PORT, CS, RS, WR, Reset>::lcdSettings(bool landscape) {
	// Hardware reset is low from initialize
	xpcc::delay_ms(50);
	Reset::set();
	xpcc::delay_ms(50);

	writeCmd(0x00, 0x0001); // R00: Start oscillation
	xpcc::delay_ms(10);

	//power on sequence
	writeCmd(0x10, 0x1f92);	// R10: Power Control 1
	writeCmd(0x11, 0x0014);	// R11: Power Control 2
	writeCmd(0x00, 0x0001);	// R00: Start oscillation
	writeCmd(0x10, 0x1f92);	// R10: Power Control 1
	writeCmd(0x11, 0x001c); // R11: Power Control 2
	writeCmd(0x28, 0x0006);	// R28: VCOM OTP (1)
	writeCmd(0x02, 0x0000);	// R02: LCD drive AC control
	writeCmd(0x12, 0x040f); // R12: Power Control 2

	xpcc::delay_ms(100);

	if (landscape) {
		writeCmd(0x03, 0x7820);
	}
	else {
		writeCmd(0x03, 0x7838);	// R03: Entry mode
	}
	/**
	 * Bit 0 set: stopped working
	 * Bit 1 set: no change
	 * 0x7830 | 0x0003: no change
	 * 0x7830 | 0x0004: stoppend working
	 * 0x7830 | 0x0008:	landscape
	 *
	 * 0x7838 | 0x0001:	stopped working
	 * 0x7838 | 0x0002:	no change
	 * 0x7838 | 0x0003:	colour inverted
	 * 0x7838 | 0x0004:	no change
	 * 0x7800
	 */

	writeCmd(0x01, 0x31af);	// R01: Driver output control
	writeCmd(0x07, 0x0033);	// R07: Display Control
	xpcc::delay_ms(10);

	// colourful test
	lcdCls(0x0000); // black
}

template <typename PORT, typename CS, typename RS, typename WR, typename Reset>
void
xpcc::SiemensS75Common<PORT, CS, RS, WR, Reset>::lcdCls(uint16_t colour) {
	// Set CGRAM Address to 0 = upper left corner
	writeCmd(0x21, 0x0000);

	// Set instruction register to "RAM Data write"
	writeReg(0x22);

	RS::set();
	CS::reset();

	// start data transmission

	// generic implementation
	uint8_t c1 = colour >> 8;
	uint8_t c2 = colour & 0xff;
	for (uint16_t i = 0; i < (132 * 176); ++i) {
		WR::reset();
		PORT::write(c1);
		WR::set();

		WR::reset();
		PORT::write(c2);
		WR::set();
	}

	CS::set();
}

template <typename PORT, typename CS, typename RS, typename WR, typename Reset>
void
xpcc::SiemensS75Portrait<PORT, CS, RS, WR, Reset>::update() {
	// Set CGRAM Address to 0 = upper left corner
	SiemensS75Common<PORT, CS, RS, WR, Reset>::writeCmd(0x21, 0x0000);

	// Set instruction register to "RAM Data write"
	SiemensS75Common<PORT, CS, RS, WR, Reset>::writeReg(0x22);

	// WRITE MEMORY
	RS::set();
	CS::reset();

	const uint16_t maskBlank  = 0x0000; // RRRR RGGG GGGB BBBB
	const uint16_t maskFilled = 0x37e0; // RRRR RGGG GGGB BBBB

	const uint8_t width = 132;
	const uint8_t height = 176 / 8;

	const uint8_t fill_h = maskFilled >> 8;
	const uint8_t fill_l = maskFilled & 0xff;

	const uint8_t blank_h = maskBlank >> 8;
	const uint8_t blank_l = maskBlank & 0xff;

	for (uint8_t x = 0; x < width; ++x)
	{
		for (uint8_t y = 0; y < height; ++y)
		{
			// group of 8 black-and-white pixels
			uint8_t group = this->buffer[x][y];
			uint8_t PORTBuffer[16];
			uint8_t PORTIdx = 0;

			for (uint8_t pix = 0; pix < 8; ++pix, group >>= 1) {
				if (group & 1)
				{
					PORTBuffer[PORTIdx++] = fill_h;
					PORTBuffer[PORTIdx++] = fill_l;
				}
				else
				{
					PORTBuffer[PORTIdx++] = blank_h;
					PORTBuffer[PORTIdx++] = blank_l;
				}
			} // pix

			for (uint8_t ii = 0; ii < PORTIdx; ++ii) {
				PORT::write(PORTBuffer[ii]);
				WR::reset();	// High-to-Low strobe
				WR::set();
			}
		} // y
	} // x

	CS::set();
}

template <typename PORT, typename CS, typename RS, typename WR, typename Reset>
void
xpcc::SiemensS75Landscape<PORT, CS, RS, WR, Reset>::update() {
	// Set CGRAM Address to height-1 = upper left corner
	SiemensS75Common<PORT, CS, RS, WR, Reset>::writeCmd(0x21, 131);

	// Set instruction register to "RAM Data write"
	SiemensS75Common<PORT, CS, RS, WR, Reset>::writeReg(0x22);

	// WRITE MEMORY
	RS::set();
	CS::reset();

	const uint16_t maskBlank  = 0x0000; // RRRR RGGG GGGB BBBB
	const uint16_t maskFilled = 0x37e0; // RRRR RGGG GGGB BBBB

	// size of the XPCC Display buffer, not the hardware pixels
	const uint8_t width = 176;
	const uint8_t height = 136 / 8; // Display is only 132 pixels high.

	const uint8_t fill_h = maskFilled >> 8;
	const uint8_t fill_l = maskFilled & 0xff;

	const uint8_t blank_h = maskBlank >> 8;
	const uint8_t blank_l = maskBlank & 0xff;

	for (uint8_t x = 0; x < width; ++x)
	{
		for (uint8_t y = 0; y < height; ++y)
		{
			// group of 8 black-and-white pixels
			uint8_t group = this->buffer[x][y];

			uint8_t PORTBuffer[16];
			uint_fast8_t PORTIdx = 0;

			// Only 4 pixels at the lower end of the display in landscape mode
			uint8_t pixels;
			if (y == (height - 1)) {
				// The last pixels
				pixels = 4;
			}
			else {
				pixels = 8;
			}

			for (uint8_t pix = 0; pix < pixels; ++pix, group >>= 1) {
				if (group & 1)
				{
					PORTBuffer[PORTIdx++] = fill_h;
					PORTBuffer[PORTIdx++] = fill_l;
				}
				else
				{
					PORTBuffer[PORTIdx++] = blank_h;
					PORTBuffer[PORTIdx++] = blank_l;
				}
			} // pix
			for (uint8_t ii = 0; ii < PORTIdx; ++ii) {
				PORT::write(PORTBuffer[ii]);
				WR::reset();	// Low-to-high strobe
				WR::set();
			}
		} // y
	} // x

	CS::set();
}
