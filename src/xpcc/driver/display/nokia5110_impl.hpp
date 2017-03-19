// coding: utf-8
/* Copyright (c) 2016, strongly-typed
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_NOKIA5110_HPP
#error "Do not include this file directly. Include nokia5110.hpp instead."
#endif

namespace xpcc
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
	// Matches well with xpcc's BufferedGraphicDisplay memory layout
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

} // xpcc namespace
