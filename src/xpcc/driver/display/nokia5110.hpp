// coding: utf-8
/* Copyright (c) 2016, strongly-typed
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_NOKIA5110_HPP
#define XPCC_NOKIA5110_HPP

#include <xpcc/ui/display/buffered_graphic_display.hpp>

namespace xpcc
{

/**
 * Driver for Nokia 5110 displays
 *
 * Adopted from https://github.com/carlosefr/pcd8544/blob/master/PCD8544.cpp
 *
 * Cheap, monochrome graphical display with SPI interface.
 * Maximum SPI frequency is 4 MHz.
 *
 * \ingroup driver_display
 */
template < typename Spi, typename Ce, typename Dc, typename Reset >
class Nokia5110 : public BufferedGraphicDisplay< 84, 48 >
{
public:
	void
	initialize();

	virtual void
	update();

protected:
	void writeCommand(uint8_t data);
};

} // xpcc namespace

#include "nokia5110_impl.hpp"

#endif // XPCC_NOKIA5110_HPP
