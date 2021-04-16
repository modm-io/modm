/*
 * Copyright (c) 2009, Georgi Grinshpun
 * Copyright (c) 2009-2011, 2013, 2019, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2011, Thorsten Lajewski
 * Copyright (c) 2012-2015, Niklas Hauser
 * Copyright (c) 2021, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MODM_MONOCHROME_GRAPHIC_DISPLAY_VERTICAL_HPP
#define MODM_MONOCHROME_GRAPHIC_DISPLAY_VERTICAL_HPP

#include <stdlib.h>

#include "monochrome_graphic_display.hpp"

namespace modm
{
/**
 * Base class for graphical displays with a RAM buffer.
 *
 * Every operation works on the internal RAM buffer, therefore the content
 * of the real display is not changed until a call of update().
 *
 * \tparam	Width	Width of the display.
 * \tparam	Height	Height of the display. Must be a multiple of 8!
 *
 * \author	Fabian Greif
 * \ingroup	modm_ui_display
 */
template<int16_t Width, int16_t Height>
class MonochromeGraphicDisplayVertical
	: public MonochromeGraphicDisplay<Width, Height, Width, Height / 8>
{
	static_assert((Height % 8) == 0, "height must be a multiple of 8");

public:
	virtual ~MonochromeGraphicDisplayVertical() = default;

	// Faster version adapted for the RAM buffer
	void
	drawImageRaw(glcd::Point start, uint16_t width, uint16_t height,
				 modm::accessor::Flash<uint8_t> data) final;

	void
	setPixel(int16_t x, int16_t y) final;

	void
	clearPixel(int16_t x, int16_t y) final;

	bool
	getPixel(int16_t x, int16_t y) const final;

protected:
	// Faster version adapted for the RAM buffer
	void
	drawHorizontalLine(glcd::Point start, uint16_t length) final;

	// Faster version adapted for the RAM buffer
	void
	drawVerticalLine(glcd::Point start, uint16_t length) final;
};
}  // namespace modm

#include "monochrome_graphic_display_vertical_impl.hpp"

#endif  // MODM_MONOCHROME_GRAPHIC_DISPLAY_VERTICAL_HPP
