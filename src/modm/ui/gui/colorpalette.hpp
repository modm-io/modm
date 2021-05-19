/*
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2014, Kevin Läufer
 * Copyright (c) 2014, Niklas Hauser
 * Copyright (c) 2015, Niclas Rohrer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_GUI_COLORPALETTE_HPP
#define MODM_GUI_COLORPALETTE_HPP

#include <modm/ui/display/color_graphic_display.hpp>


namespace modm
{

namespace gui
{

class ColorPalette;
/// @ingroup modm_ui_gui
extern ColorPalette DefaultColorPalette;

/*
 * TODO: find a way so that color options can be defined in user program
 */
/// @ingroup modm_ui_gui
enum
Color {
	BLACK,
	WHITE,
	GRAY,
	RED,
	GREEN,
	BLUE,
	YELLOW,
	SIGNALVIOLET,
	EMERALDGREEN,
	BORDER,
	TEXT,
	BACKGROUND,
	ACTIVATED,
	DEACTIVATED,
	PALETTE_SIZE
};

/// @author	Niklas Hauser
/// @ingroup modm_ui_gui
class ColorPalette
{
public:
	ColorPalette(modm::color::Rgb565 colors[Color::PALETTE_SIZE]) :
		colors(colors)
	{
	}

	ColorPalette(ColorPalette &rhs = DefaultColorPalette) :
		colors(rhs.colors)
	{
	}

	ColorPalette&
	operator=(ColorPalette &rhs)
	{
		colors = rhs.colors;
		return *this;
	}

	void
	setColor(Color name, modm::color::Rgb565 color)
	{
		if (name < Color::PALETTE_SIZE)
		{
			colors[name] = color;
		}
	}

	const modm::color::Rgb565
	getColor(Color name) const
	{
		if (name >= Color::PALETTE_SIZE)
			return modm::color::Rgb565(0xffff);
		return colors[name];
	}

	const modm::color::Rgb565
	operator[](Color name)
	{
		return getColor(name);
	}

	const modm::color::Rgb565*
	getPointer() const
	{
		return colors;
	}

private:
	modm::color::Rgb565 *colors;
};

}	// namespace gui

}	// namespace modm

#endif  // MODM_GUI_COLORPALETTE_HPP
