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

#include <modm/ui/display/graphic_display.hpp>


namespace modm
{

namespace gui
{

class ColorPalette;
extern ColorPalette DefaultColorPalette;

/*
 * TODO: find a way so that color options can be defined in user program
 */
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
/// @ingroup	gui
class ColorPalette
{
public:
	ColorPalette(modm::glcd::Color colors[Color::PALETTE_SIZE]) :
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
	setColor(Color name, modm::glcd::Color color)
	{
		if (name < Color::PALETTE_SIZE)
		{
			colors[name] = color;
		}
	}

	const modm::glcd::Color
	getColor(Color name) const
	{
		if (name >= Color::PALETTE_SIZE)
			return modm::glcd::Color(0xffff);
		return colors[name];
	}

	const modm::glcd::Color
	operator[](Color name)
	{
		return getColor(name);
	}

	const modm::glcd::Color*
	getPointer() const
	{
		return colors;
	}

private:
	modm::glcd::Color *colors;
};

}	// namespace gui

}	// namespace modm

#endif  // MODM_GUI_COLORPALETTE_HPP
