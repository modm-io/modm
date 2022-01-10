/*
 * Copyright (c) 2022, Nikolay Semenov
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <modm/ui/display/color_graphic_display.hpp>

#include "st7789/st7789_driver.hpp"

namespace modm
{

/// @ingroup modm_driver_st7789
template<typename Interface, uint16_t Width = 240, uint16_t Height = 320>
class St7789 : public ColorGraphicDisplay, public St7789Driver<Interface, Width, Height>
{
public:
	using Driver = St7789Driver<Interface, Width, Height>;

	using Driver::initialize;

	uint16_t
	getWidth() const final
	{
		switch (Driver::orientation_)
		{
			case Driver::Orientation::Portrait90:
				[[fallthrough]];
			case Driver::Orientation::Portrait270:
				return Height;
			default:
				return Width;
		}
	}

	uint16_t
	getHeight() const final
	{
		switch (Driver::orientation_)
		{
			case Driver::Orientation::Portrait90:
				[[fallthrough]];
			case Driver::Orientation::Portrait270:
				return Width;
			default:
				return Height;
		}
	}

	size_t
	getBufferWidth() const final
	{
		return Width;
	}

	size_t
	getBufferHeight() const final
	{
		return Height;
	}

	void
	setPixel(int16_t x, int16_t y) final
	{
		setPixel(x, y, foregroundColor);
	}

	color::Rgb565
	getPixel(int16_t, int16_t) const final
	{
		return color::html::Black;
	}

	void
	clearPixel(int16_t x, int16_t y) final
	{
		setPixel(x, y, backgroundColor.value());
	}

	using Driver::clear;
	void
	clear() final
	{
		Driver::clear(backgroundColor.value());
	}

	void
	update() final
	{ /* noop */
	}

private:
	void
	setPixel(int16_t x, int16_t y, const color::Rgb565 &color)
	{
		Driver::setClipping(x, y, 1, 1);
		Driver::writeData({reinterpret_cast<const uint8_t *>(&color.color), sizeof(color.color)});
	}
};

}  // namespace modm
