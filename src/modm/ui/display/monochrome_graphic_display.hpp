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

#ifndef MODM_MONOCHROME_GRAPHIC_DISPLAY_HPP
#define MODM_MONOCHROME_GRAPHIC_DISPLAY_HPP

#include <stdlib.h>

#include "graphic_display.hpp"

namespace modm
{
/**
 * Base class for monochrome graphical displays with a RAM buffer.
 *
 * Every operation works on the internal RAM buffer, therefore the content
 * of the real display is not changed until a call of update().
 *
 * \tparam	Width			Horizontal number of Pixels
 * \tparam	Height			Vertical number of Pixels
 * \tparam	BufferWidth		Horizontal (first) dimension of Buffer
 * \tparam	BufferHeight	Vertical (first) dimension of Buffer
 *
 * \author	Thomas Sommer
 * \ingroup	modm_ui_display
 */
template<int16_t Width, int16_t Height, std::size_t BufferWidth, std::size_t BufferHeight>
class MonochromeGraphicDisplay : public GraphicDisplay
{
	static_assert(Width > 0, "width must be greater than 0");
	static_assert(Height > 0, "height must be greater than 0");

public:
	virtual ~MonochromeGraphicDisplay() = default;

	inline uint16_t
	getWidth() const final
	{
		return Width;
	}

	inline uint16_t
	getHeight() const final
	{
		return Height;
	}

	inline std::size_t
	getBufferWidth() const final
	{
		return BufferWidth;
	}

	inline std::size_t
	getBufferHeight() const final
	{
		return BufferHeight;
	}

	virtual bool
	getPixel(int16_t x, int16_t y) const = 0;

	void
	clear() final;

protected:
	uint8_t buffer[BufferHeight][BufferWidth];
};
}  // namespace modm

#include "monochrome_graphic_display_impl.hpp"

#endif  // MODM_MONOCHROME_GRAPHIC_DISPLAY_HPP