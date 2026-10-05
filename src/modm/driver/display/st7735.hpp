/*
 * Copyright (c) 2026, Niklas Hauser
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

#include "st7735/st7735_driver.hpp"

namespace modm
{

/// @ingroup modm_driver_st7735
template<typename Interface>
class St7735 : public ColorGraphicDisplay, public St7735Driver<Interface>
{
public:
	using Driver = St7735Driver<Interface>;

	using Driver::Driver;
	using Driver::initialize;

	uint16_t
	getWidth() const final
	{ return Driver::getWidth(); }

	uint16_t
	getHeight() const final
	{ return Driver::getHeight(); }

	size_t
	getBufferWidth() const final
	{ return getWidth(); }

	size_t
	getBufferHeight() const final
	{ return getHeight(); }

	void
	setPixel(int16_t x, int16_t y) final
	{ setPixel(x, y, foregroundColor); }

	color::Rgb565
	getPixel(int16_t, int16_t) const final
	{ return color::html::Black; }

	void
	clearPixel(int16_t x, int16_t y) final
	{ setPixel(x, y, backgroundColor); }

	using Driver::clear;
	void
	clear() final
	{ Driver::clear(backgroundColor.color); }

	void
	update() final
	{ /* noop */
	}

private:
	void
	setPixel(int16_t x, int16_t y, const color::Rgb565 &color)
	{
		if (x < 0 || y < 0 || x >= getWidth() || y >= getHeight()) { return; }

		Driver::setClipping(static_cast<uint16_t>(x), static_cast<uint16_t>(y), 1, 1);
		Driver::writeColor(color.color);
	}
};

}  // namespace modm
