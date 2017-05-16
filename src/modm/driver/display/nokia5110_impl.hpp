/*
 * Copyright (c) 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_NOKIA5110_HPP
#error "Do not include this file directly. Include nokia5110.hpp instead."
#endif

namespace modm
{

template< typename Spi, typename Ce, typename Dc, typename Reset >
void
Nokia5110< Spi, Ce, Dc, Reset >::initialize()
{
	Reset::set();

	// Function set: PD = 0 (on), V = 0 (horizontal addressing), H = 1 (extended instruction set)
	writeCommand(0x21);
	writeCommand(0x13); // Bias System:

	writeCommand(0xc2); // Set Vop

	// Basic instruction set, vertical addressing
	// Matches well with modm's BufferedGraphicDisplay memory layout
	writeCommand(0x20 | (1 << 1));

	// writeCommand(0x08); // display blank
	// writeCommand(0x09); // all on
	writeCommand(0x0c); // normal
	// writeCommand(0x0d); // inverse
}

template< typename Spi, typename Ce, typename Dc, typename Reset >
void
Nokia5110< Spi, Ce, Dc, Reset >::update()
{
	// goto 0, 0
	writeCommand(0x80); // Column
	writeCommand(0x40); // Row

	Ce::reset();
	Dc::set(); // high = data
	for (uint8_t xx = 0; xx < this->getWidth(); ++xx) {
		for (uint8_t yy = 0; yy < this->getHeight() / 8; ++yy) {
			Spi::transferBlocking(this->display_buffer[xx][yy]);
		}
	}
	Ce::set();
}

template< typename Spi, typename Ce, typename Dc, typename Reset >
void
Nokia5110< Spi, Ce, Dc, Reset >::writeCommand(uint8_t data)
{
	Dc::reset(); // low = command
	Ce::reset();
	Spi::transferBlocking(data);
	Ce::set();
}

} // modm namespace
