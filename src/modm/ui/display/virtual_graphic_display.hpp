/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2012, 2014, Niklas Hauser
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2013, Thorsten Lajewski
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_VIRTUAL_GRAPHIC_DISPLAY
#define MODM_VIRTUAL_GRAPHIC_DISPLAY

#include <modm/ui/display/color_graphic_display.hpp>

namespace modm
{
/// @ingroup modm_ui_display
class VirtualGraphicDisplay : public modm::ColorGraphicDisplay
{
public:
	VirtualGraphicDisplay(modm::ColorGraphicDisplay* display, modm::glcd::Point leftUpper,
						  modm::glcd::Point rightLower);

	void
	setDisplay(modm::ColorGraphicDisplay* display);

	virtual inline uint16_t
	getWidth() const
	{
		return this->width;
	}

	virtual inline uint16_t
	getHeight() const
	{
		return this->height;
	}

	virtual void
	clear();

	virtual void
	update();

protected:
	void
	setPixel(int16_t x, int16_t y) final;

	void
	clearPixel(int16_t x, int16_t y) final;

	color::Rgb565
	getPixel(int16_t x, int16_t y) const final;

private:
	modm::ColorGraphicDisplay* display;
	modm::glcd::Point leftUpper;
	modm::glcd::Point rightLower;
	const uint16_t width;
	const uint16_t height;
};

}  // namespace modm

#endif  // MODM_VIRTUAL_GRAPHIC_DISPLAY
