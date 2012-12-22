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
template <typename MEMORY, typename RESET>
void
xpcc::SiemensS75Portrait<MEMORY, RESET>::initialize()
{
	MEMORY::initialize();

	// Reset pin
	RESET::setOutput(false);

	SiemensS75Common<MEMORY, RESET>::lcdSettings(false);

	this->clear();
}

// ----------------------------------------------------------------------------

template <typename MEMORY, typename RESET>
void
xpcc::SiemensS75Landscape<MEMORY, RESET>::initialize()
{
	MEMORY::initialize();

	// Reset pin
	RESET::setOutput(false);

	SiemensS75Common<MEMORY, RESET>::lcdSettings(true);

	this->clear();
}

// ----------------------------------------------------------------------------

template <typename MEMORY, typename RESET>
void
xpcc::SiemensS75Common<MEMORY, RESET>::lcdSettings(bool landscape) {
	// Hardware reset is low from initialize
	xpcc::delay_ms(50);
	RESET::set();
	xpcc::delay_ms(50);

	MEMORY::writeCommand(0x00, 0x0001); // R00: Start oscillation
	xpcc::delay_ms(10);

	//power on sequence
	MEMORY::writeCommand(0x10, 0x1f92);	// R10: Power Control 1
	MEMORY::writeCommand(0x11, 0x0014);	// R11: Power Control 2
	MEMORY::writeCommand(0x00, 0x0001);	// R00: Start oscillation
	MEMORY::writeCommand(0x10, 0x1f92);	// R10: Power Control 1
	MEMORY::writeCommand(0x11, 0x001c); // R11: Power Control 2
	MEMORY::writeCommand(0x28, 0x0006);	// R28: VCOM OTP (1)
	MEMORY::writeCommand(0x02, 0x0000);	// R02: LCD drive AC control
	MEMORY::writeCommand(0x12, 0x040f); // R12: Power Control 2

	xpcc::delay_ms(100);

	if (landscape) {
		MEMORY::writeCommand(0x03, 0x7820);
	}
	else {
		MEMORY::writeCommand(0x03, 0x7838); 	// R03: Entry mode
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

	MEMORY::writeCommand(0x01, 0x31af);	// R01: Driver output control
	MEMORY::writeCommand(0x07, 0x0033);	// R07: Display Control
	xpcc::delay_ms(10);

	// colourful test
	lcdCls(0x0000); // black
}

template <typename MEMORY, typename RESET>
void
xpcc::SiemensS75Common<MEMORY, RESET>::lcdCls(uint16_t colour) {
	// Set CGRAM Address to 0 = upper left corner
	MEMORY::writeCommand(0x21, 0x0000);

	// Set instruction register to "RAM Data write"
	MEMORY::writeRegister(0x22);

	// Write all pixels to the same colour
	MEMORY::writeDataMult(colour, 132 * 176);
}

template <typename MEMORY, typename RESET>
void
xpcc::SiemensS75Portrait<MEMORY, RESET>::update() {
	// Set CGRAM Address to 0 = upper left corner
	MEMORY::writeCommand(0x21, 0x0000);

	// Set instruction register to "RAM Data write"
	MEMORY::writeRegister(0x22);

	const uint16_t maskBlank  = 0x0000; // RRRR RGGG GGGB BBBB
	const uint16_t maskFilled = 0x37e0; // RRRR RGGG GGGB BBBB

	const uint8_t width = 132;
	const uint8_t height = 176 / 8;

	const uint8_t fill_h = maskFilled >> 8;
	const uint8_t fill_l = maskFilled & 0xff;

	const uint8_t blank_h = maskBlank >> 8;
	const uint8_t blank_l = maskBlank & 0xff;

	for (uint_fast8_t x = 0; x < width; ++x)
	{
		for (uint_fast8_t y = 0; y < height; ++y)
		{
			// group of 8 black-and-white pixels
			uint_fast8_t group = this->buffer[x][y];
			uint8_t PortBuffer[16];
			uint_fast8_t PortIdx = 0;

			for (uint_fast8_t pix = 0; pix < 8; ++pix, group >>= 1) {
				if (group & 1)
				{
					PortBuffer[PortIdx++] = fill_h;
					PortBuffer[PortIdx++] = fill_l;
				}
				else
				{
					PortBuffer[PortIdx++] = blank_h;
					PortBuffer[PortIdx++] = blank_l;
				}
			} // pix

			MEMORY::writeRam(PortBuffer, PortIdx);
		} // y
	} // x
}

template <typename MEMORY, typename RESET>
void
xpcc::SiemensS75Landscape<MEMORY, RESET>::update() {
	// Set CGRAM Address to height-1 = upper left corner
	MEMORY::writeCommand(0x21, 131);

	// Set instruction register to "RAM Data write"
	MEMORY::writeRegister(0x22);

	const uint16_t maskBlank  = 0x0000; // RRRR RGGG GGGB BBBB
	const uint16_t maskFilled = 0x37e0; // RRRR RGGG GGGB BBBB

	// size of the XPCC Display buffer, not the hardware pixels
	const uint8_t width = 176;
	const uint8_t height = 136 / 8; // Display is only 132 pixels high.

	const uint8_t fill_h = maskFilled >> 8;
	const uint8_t fill_l = maskFilled & 0xff;

	const uint8_t blank_h = maskBlank >> 8;
	const uint8_t blank_l = maskBlank & 0xff;

	for (uint_fast8_t x = 0; x < width; ++x)
	{
		for (uint_fast8_t y = 0; y < height; ++y)
		{
			// group of 8 black-and-white pixels
			uint_fast8_t group = this->buffer[x][y];
			uint8_t PortBuffer[16];
			uint_fast8_t PortIdx = 0;

			// Only 4 pixels at the lower end of the display in landscape mode
			uint_fast8_t pixels;
			if (y == (height - 1)) {
				// The last pixels
				pixels = 4;
			}
			else {
				pixels = 8;
			}

			for (uint_fast8_t pix = 0; pix < pixels; ++pix, group >>= 1) {
				if (group & 1)
				{
					PortBuffer[PortIdx++] = fill_h;
					PortBuffer[PortIdx++] = fill_l;
				}
				else
				{
					PortBuffer[PortIdx++] = blank_h;
					PortBuffer[PortIdx++] = blank_l;
				}
			} // pix

			MEMORY::writeRam(PortBuffer, PortIdx);
		} // y
	} // x
}
