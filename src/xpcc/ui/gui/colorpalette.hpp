// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_GUI_COLORPALETTE_HPP
#define XPCC_GUI_COLORPALETTE_HPP

#include <xpcc/ui/display/graphic_display.hpp>


namespace xpcc
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
	BORDER,
	TEXT,
	BACKGROUND,
	ACTIVATED,
	DEACTIVATED,
	PALETTE_SIZE
};

class ColorPalette
{
public:
	ColorPalette(xpcc::glcd::Color colors[Color::PALETTE_SIZE]) :
		colors(colors)
	{
	}

	ColorPalette(ColorPalette &palette) :
		colors(palette.colors)
	{
	}

	ColorPalette&
	operator=(ColorPalette rhs)
	{
		for(int ii = 0; ii < Color::PALETTE_SIZE; ++ii)
		{
			colors[ii] = rhs.colors[ii];
		}
		return *this;
	}

	void
	setColor(xpcc::glcd::Color color, Color name)
	{
		if (name < Color::PALETTE_SIZE)
		{
			colors[name] = color;
		}
	}

	const xpcc::glcd::Color
	getColor(Color name) const
	{
		if (name >= Color::PALETTE_SIZE)
			return xpcc::glcd::Color(0xffff);
		return colors[name];
	}

	const xpcc::glcd::Color
	operator[](Color name)
	{
		if (name >= Color::PALETTE_SIZE)
			return xpcc::glcd::Color(0xffff);
		return colors[name];
	}

	const xpcc::glcd::Color*
	getPointer() const
	{
		return colors;
	}

private:
	xpcc::glcd::Color *colors;
};

}	// namespace gui

}	// namespace xpcc

#endif  // XPCC_GUI_COLORPALETTE_HPP
