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
#define MODM_NOKIA5110_HPP

#include <modm/ui/display/monochrome_graphic_display_buffered_vertical.hpp>

namespace modm
{

/**
 * Driver for Nokia 5110 displays
 *
 * Adopted from https://github.com/carlosefr/pcd8544/blob/master/PCD8544.cpp
 *
 * Cheap, monochrome graphical display with SPI interface.
 * Maximum SPI frequency is 4 MHz.
 *
 * \ingroup modm_driver_nokia5110
 */
template < typename Spi, typename Ce, typename Dc, typename Reset >
class Nokia5110 : public MonochromeGraphicDisplayBufferedVertical< 84, 48 >
{
public:
	void
	initialize();

	virtual void
	update();

protected:
	void writeCommand(uint8_t data);
};

} // modm namespace

#include "nokia5110_impl.hpp"

#endif // MODM_NOKIA5110_HPP
