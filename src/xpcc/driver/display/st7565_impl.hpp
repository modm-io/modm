/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2012-2014, Niklas Hauser
 * Copyright (c) 2012, 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
		spi.transferBlocking(ST7565_PAGE_ADDRESS | y);		// Row select
		spi.transferBlocking(ST7565_COL_ADDRESS_MSB);		// Column select high

		if (TopView) {
			spi.transferBlocking(ST7565_COL_ADDRESS_LSB | 4);	// Column select low
		}
		else {
			spi.transferBlocking(ST7565_COL_ADDRESS_LSB);	// Column select low
		}

		// switch to data mode
		a0.set();
		for(uint8_t x = 0; x < Width; ++x) {
			spi.transferBlocking(this->buffer[x][y]);
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
		spi.transferBlocking(ST7565_REVERSE);
	}
	else {
		spi.transferBlocking(ST7565_NORMAL);
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
	xpcc::delayMilliseconds(50);
	reset.set();

	cs.reset();
	a0.reset();

	// View direction
	if (TopView) {
		spi.transferBlocking(ST7565_ADC_NORMAL);		// ADC normal
		spi.transferBlocking(ST7565_SCAN_DIR_REVERSE);	// reverse COM0~COM63
	}
	else {
		spi.transferBlocking(ST7565_ADC_REVERSE);
		spi.transferBlocking(ST7565_SCAN_DIR_NORMAL);
	}

	for (uint8_t i = 0; i < size; ++i) {
		spi.transferBlocking(configuration[i]);
	}

	cs.set();

	this->clear();
	this->update();
}
