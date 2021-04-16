/*
 * Copyright (c) 2019, Fabian Greif
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MODM_MONOCHROME_GRAPHIC_DISPLAY_HORIZONTAL_HPP
#define MODM_MONOCHROME_GRAPHIC_DISPLAY_HORIZONTAL_HPP

#include <stdlib.h>

#include "monochrome_graphic_display.hpp"

namespace modm
{
/**
 * Base class for monochrome graphical displays with a RAM buffer.
 *
 * The pixel are compressed horizontally, this means that 8 pixel in
 * y-direction are combined into one byte in the RAM buffer.
 *
 * Every operation works on the internal RAM buffer, therefore the content
 * of the real display is not changed until a call of update().
 *
 * \tparam	Width	Width of the display. Must be a multiple of 8!
 * \tparam	Height	Height of the display.
 *
 * \ingroup	modm_ui_display
 */
template<int16_t Width, int16_t Height>
class MonochromeGraphicDisplayHorizontal
	: public MonochromeGraphicDisplay<Width, Height, Width / 8, Height>
{
	// Height must be a multiple of 8
	static_assert((Width % 8) == 0, "width must be a multiple of 8");

public:
	virtual ~MonochromeGraphicDisplayHorizontal() = default;

protected:
	void
	setPixel(int16_t x, int16_t y) final;

	void
	clearPixel(int16_t x, int16_t y) final;

	bool
	getPixel(int16_t x, int16_t y) const final;
};
}  // namespace modm

#include "monochrome_graphic_display_horizontal_impl.hpp"

#endif  // MODM_MONOCHROME_GRAPHIC_DISPLAY_HORIZONTAL_HPP
