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

	/*
	 * TODO: find a way so that color options can be defined in user program
	 */

	enum Color {
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


	typedef xpcc::glcd::Color ColorPalette [Color::PALETTE_SIZE];

//	typedef xpcc::glcd::Color ColorPalette;
//	typedef std::array<xpcc::glcd::Color, Color::PALETTE_SIZE> ColorPalette;


	/*
	 * TODO: make DEFAULT_COLORPALETTE const or even better constexpr
	 */
	extern ColorPalette DEFAULT_COLORPALETTE; //[xpcc::gui::Color::PALETTE_SIZE];

	static inline void
	copyColorPalette(ColorPalette& src, ColorPalette& dest)
	{
		for(int ii = 0; ii < Color::PALETTE_SIZE; ++ii){
			dest[ii] = src[ii];
		}
	}

}	// namespace gui

}	// namespace xpcc

#endif  // XPCC_GUI_COLORPALETTE_HPP
