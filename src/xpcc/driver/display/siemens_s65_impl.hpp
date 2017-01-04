/*
 * Copyright (c) 2012, 2014, Sascha Schade
 * Copyright (c) 2012-2013, Fabian Greif
 * Copyright (c) 2013-2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

/*
 * Code from:
 * http://www.watterott.net/projects/s65-shield
 * http://www.superkranz.de/christian/S65_Display/DisplaySoftware.html
 *
 * Pinout from:
 * http://www.juras-projects.org/eng/lcd.php
 */

#ifndef XPCC_SIEMENS_S65_HPP
#	error	"Don't include this file directly, use 'siemens_s65.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename SPI, typename CS, typename RS, typename Reset>
void
xpcc::SiemensS65Portrait<SPI, CS, RS, Reset>::initialize()
{
	// CS pin
	CS::setOutput(true);

	// RS pin
	RS::setOutput(false);

	// Reset pin
	Reset::setOutput(false);

	SiemensS65Common<SPI, CS, RS, Reset>::lcdSettings(false);

	this->clear();
}

// ----------------------------------------------------------------------------

template <typename SPI, typename CS, typename RS, typename Reset>
void
xpcc::SiemensS65Landscape<SPI, CS, RS, Reset>::initialize()
{
	// CS pin
	CS::setOutput(true);

	// RS pin
	RS::setOutput(false);

	// Reset pin
	Reset::setOutput(false);

	SiemensS65Common<SPI, CS, RS, Reset>::lcdSettings(true);

	this->clear();
}

// ----------------------------------------------------------------------------

template <typename SPI, typename CS, typename RS, typename Reset>
void
xpcc::SiemensS65Common<SPI, CS, RS, Reset>::writeCmd(uint8_t reg, uint16_t param)
{
	writeReg(reg);
	writeData(param);
}

template <typename SPI, typename CS, typename RS, typename Reset>
void
xpcc::SiemensS65Common<SPI, CS, RS, Reset>::writeReg(uint8_t reg)
{
	CS::reset();
	SPI::transferBlocking(0x74); // start byte, RS = 0, R/W = 0, write index register
	SPI::transferBlocking(0x00);
	SPI::transferBlocking(reg);
	CS::set();
}

template <typename SPI, typename CS, typename RS, typename Reset>
void
xpcc::SiemensS65Common<SPI, CS, RS, Reset>::writeData(uint16_t data)
{
	CS::reset();
	SPI::transferBlocking(0x76);	// start byte, RS = 1, R/W = 0, write instruction or RAM data
	SPI::transferBlocking(data>>8);
	SPI::transferBlocking(data);
	CS::set();
}

template <typename SPI, typename CS, typename RS, typename Reset>
void
xpcc::SiemensS65Common<SPI, CS, RS, Reset>::lcdSettings(bool landscape) {
	// Hardware reset is low from initialize
	xpcc::delayMilliseconds(50);
	Reset::set();
	xpcc::delayMilliseconds(50);

	writeCmd(0x07, 0x0000); //display off
	xpcc::delayMilliseconds(10);

	//power on sequence
	writeCmd(0x02, 0x0400); //lcd drive control
	writeCmd(0x0C, 0x0001); //power control 3: VC        //step 1
	writeCmd(0x0D, 0x0006); //power control 4: VRH
	writeCmd(0x04, 0x0000); //power control 2: CAD
	writeCmd(0x0D, 0x0616); //power control 4: VRL
	writeCmd(0x0E, 0x0010); //power control 5: VCM
	writeCmd(0x0E, 0x1010); //power control 5: VDV
	writeCmd(0x03, 0x0000); //power control 1: BT        //step 2
	writeCmd(0x03, 0x0000); //power control 1: DC
	writeCmd(0x03, 0x000C); //power control 1: AP
	xpcc::delayMilliseconds(40);
	writeCmd(0x0E, 0x2D1F); //power control 5: VCOMG     //step 3
	xpcc::delayMilliseconds(40);
	writeCmd(0x0D, 0x0616); //power control 4: PON       //step 4
	xpcc::delayMilliseconds(100);

	//display options
	if (landscape) {
		writeCmd(0x05, 0x0030); //Entry mode AM=0, ID0=1, ID1=1
	}
	else {
		// portrait mode
		writeCmd(0x05, 0x0038); //Entry mode AM=1, ID0=1, ID1=1
	}

	//setArea(0, 0, (S65_WIDTH-1), (S65_HEIGHT-1));
	//  writeCmd(0x16, 176<<8); //set y
	//  writeCmd(0x17, 132<<8); //set x

	//display on sequence (bit2 = reversed colors)
	writeCmd(0x07, 0x0005); //display control: D0
	writeCmd(0x07, 0x0025); //display control: GON
	writeCmd(0x07, 0x0027); //display control: D1
	writeCmd(0x07, 0x0037); //display control: DTE
	xpcc::delayMilliseconds(10);
	lcdCls(0x03e0);
}

template <typename SPI, typename CS, typename RS, typename Reset>
void
xpcc::SiemensS65Common<SPI, CS, RS, Reset>::lcdCls(uint16_t colour) {
	// Set CGRAM Address to 0 = upper left corner
	writeCmd(0x21, 0x0000);

	// Set instruction register to "RAM Data write"
	writeReg(0x22);

	CS::reset();
	SPI::transferBlocking(0x76);	// start byte

	// start data transmission

#if S65_LPC_ACCELERATED > 0
	// switch to 16 bit mode, wait for empty FIFO before
	while (!(LPC_SSP0->SR & SPI_SRn_TFE));
	LPC_SSP0->CR0 |= 0x0f;

	for (uint_fast16_t ii = 0; ii < (132 * 176); ++ii)
	{
		// 8 pixels of 16 bits fit in the Tx FIFO if it is empty.
		while (!(LPC_SSP0->SR & SPI_SRn_TFE));

		for (uint_fast8_t jj = 0; jj < 8; ++jj) {
			LPC_SSP0->DR = colour;
		}
	}
	// switch back to 8 bit transfer, wait for empty FIFO before.
	while (!(LPC_SSP0->SR & SPI_SRn_TFE));
	LPC_SSP0->CR0 &= ~0xff;
	LPC_SSP0->CR0 |=  0x07;
#else
	// generic implementation
	uint8_t c1 = colour >> 8;
	uint8_t c2 = colour & 0xff;
	for (uint16_t i = 0; i < (132 * 176); ++i) {
		SPI::transferBlocking(c1);
		SPI::transferBlocking(c2);
	}
#endif

	CS::set();
}

template <typename SPI, typename CS, typename RS, typename Reset>
void
xpcc::SiemensS65Portrait<SPI, CS, RS, Reset>::update() {
	// Set CGRAM Address to 0 = upper left corner
	SiemensS65Common<SPI, CS, RS, Reset>::writeCmd(0x21, 0x0000);

	// Set instruction register to "RAM Data write"
	SiemensS65Common<SPI, CS, RS, Reset>::writeReg(0x22);

	// WRITE MEMORY
	CS::reset();
	SPI::transferBlocking(0x76);	// start byte

	const uint16_t maskBlank  = 0x0000; // RRRR RGGG GGGB BBBB
	const uint16_t maskFilled = 0x37e0; // RRRR RGGG GGGB BBBB

	const uint8_t width = 132; // this->getWidth();
	const uint8_t height = 176 / 8; // this->getHeight()/8;

#if S65_LPC_ACCELERATED > 0
	/**
	 * This is an extremely optimised version of the copy routine for LPC
	 * microcontrollers. It will work with any SPI block that supports 16-bit
	 * transfer and a FIFO with a size of at least 8 frames.
	 *
	 * The SPI is switched to 16-bit mode to reduce the
	 * numbers of bus accesses and use the trick that then 8 pixels of each
	 * 16 bits fit into the FIFO.
	 *
	 * This reduces the copy time to 17.6 msec when running at 24 MHz SPI clock.
	 * This is a peak transfer rate of 2.6 MiByte/sec
	 */

	// switch to 16 bit mode, wait for empty FIFO before
	while (!(LPC_SSP0->SR & SPI_SRn_TFE));
	LPC_SSP0->CR0 |= 0x0f;

	for (uint8_t x = width - 1; x != 0xff; --x)
	{
		for (uint8_t y = 0; y < height; ++y)
		{
			// group of 8 black-and-white pixels
			uint8_t group = this->buffer[x][y];

			// 8 pixels of 16 bits fit in the Tx FIFO if it is empty.
			while (!(LPC_SSP0->SR & SPI_SRn_TFE));

			for (uint8_t pix = 0; pix < 8; ++pix, group >>= 1) {
				LPC_SSP0->DR = (group & 1) ? maskFilled : maskBlank;
			} // pix
		} // y
	} // x

	// switch back to 8 bit transfer, wait for empty FIFO before.
	while (!(LPC_SSP0->SR & SPI_SRn_TFE));
	LPC_SSP0->CR0 &= ~0xff;
	LPC_SSP0->CR0 |=  0x07;

#else
	// ----- Normal version with SPI buffer
	const uint8_t fill_h = maskFilled >> 8;
	const uint8_t fill_l = maskFilled & 0xff;

	const uint8_t blank_h = maskBlank >> 8;
	const uint8_t blank_l = maskBlank & 0xff;

	for (uint8_t x = width - 1; x != 0xff; --x)
	{
		for (uint8_t y = 0; y < height; ++y)
		{
			// group of 8 black-and-white pixels
			uint8_t group = this->buffer[x][y];
			uint8_t spiBuffer[16];
			uint8_t spiIdx = 0;

			for (uint8_t pix = 0; pix < 8; ++pix, group >>= 1) {
				if (group & 1)
				{
					spiBuffer[spiIdx++] = fill_h;
					spiBuffer[spiIdx++] = fill_l;
				}
				else
				{
					spiBuffer[spiIdx++] = blank_h;
					spiBuffer[spiIdx++] = blank_l;
				}
			} // pix

			// use transfer() of SPI to transfer spiBuffer
			SPI::transferBlocking(spiBuffer, nullptr, 16);
		} // y
	} // x
#endif

	CS::set();
}

template <typename SPI, typename CS, typename RS, typename Reset>
void
xpcc::SiemensS65Landscape<SPI, CS, RS, Reset>::update() {
	// Set CGRAM Address to 0 = upper left corner
	SiemensS65Common<SPI, CS, RS, Reset>::writeCmd(0x21, 0x0000);

	// Set instruction register to "RAM Data write"
	SiemensS65Common<SPI, CS, RS, Reset>::writeReg(0x22);

	// WRITE MEMORY
	CS::reset();
	SPI::transferBlocking(0x76);	// start byte

	const uint16_t maskBlank  = 0x0000; // RRRR RGGG GGGB BBBB
	const uint16_t maskFilled = 0x37e0; // RRRR RGGG GGGB BBBB

	// size of the XPCC Display buffer, not the hardware pixels
	const uint8_t width = 176;
	const uint8_t height = 136 / 8; // Display is only 132 pixels high.

#if S65_LPC_ACCELERATED > 0
	// See SiemensS65Portrait for a description

	// switch to 16 bit mode, wait for empty FIFO before
	while (!(LPC_SSP0->SR & SPI_SRn_TFE));
	LPC_SSP0->CR0 |= 0x0f;

	for (uint8_t x = 0; x < width; ++x)
	{
		for (uint8_t y = 0; y < height; ++y)
		{
			// group of 8 black-and-white pixels
			uint8_t group = this->buffer[x][y];

			// 8 pixels of 16 bits fit in the Tx FIFO if it is empty.
			while (!(LPC_SSP0->SR & SPI_SRn_TFE));

			// Only 4 pixels at the lower end of the display in landscape mode
			uint8_t pixels = (y == (height - 1)) ? 4 : 8;

			for (uint8_t pix = 0; pix < pixels; ++pix, group >>= 1) {
				LPC_SSP0->DR = (group & 1) ? maskFilled : maskBlank;
			} // pix
		} // y
	} // x

	// switch back to 8 bit transfer, wait for empty FIFO before.
	while (!(LPC_SSP0->SR & SPI_SRn_TFE));
	LPC_SSP0->CR0 &= ~0xff;
	LPC_SSP0->CR0 |=  0x07;

#else
	// ----- Normal version with SPI buffer
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

			uint8_t spiBuffer[16];
			uint8_t bufSize = 16;
			uint_fast8_t spiIdx = 0;

			// Only 4 pixels at the lower end of the display in landscape mode
			uint8_t pixels;
			if (y == (height - 1)) {
				// The last pixels
				pixels = 4;
				bufSize = 8;
			}
			else {
				pixels = 8;
			}

			for (uint8_t pix = 0; pix < pixels; ++pix, group >>= 1) {
				if (group & 1)
				{
					spiBuffer[spiIdx++] = fill_h;
					spiBuffer[spiIdx++] = fill_l;
				}
				else
				{
					spiBuffer[spiIdx++] = blank_h;
					spiBuffer[spiIdx++] = blank_l;
				}
			} // pix

			// use transfer() of SPI to transfer spiBuffer
			SPI::transferBlocking(spiBuffer, nullptr, bufSize);
		} // y
	} // x
#endif

	CS::set();
}
