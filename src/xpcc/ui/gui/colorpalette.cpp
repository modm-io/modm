// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include "colorpalette.hpp"

xpcc::glcd::Color defaultColors[] = {
	xpcc::glcd::Color::black(),		// BLACK
	xpcc::glcd::Color::white(),		// WHITE
	xpcc::glcd::Color::gray(),		// GRAY
	xpcc::glcd::Color::red(),		// RED
	xpcc::glcd::Color::green(),		// GREEN
	xpcc::glcd::Color::blue(),		// BLUE
	xpcc::glcd::Color::yellow(),	// YELLOW
	xpcc::glcd::Color::blue(),		// BORDER
	xpcc::glcd::Color::yellow(),	// TEXT
	xpcc::glcd::Color::black(),		// BACKGROUND
	xpcc::glcd::Color::red(),		// ACTIVATED
	xpcc::glcd::Color::blue(),		// DEACTIVATED
};

xpcc::gui::ColorPalette xpcc::gui::DEFAULT_COLORPALETTE(defaultColors);
