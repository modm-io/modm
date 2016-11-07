/*
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2014, Kevin Laeufer
 * Copyright (c) 2014, Niklas Hauser
 * Copyright (c) 2015, None
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
	xpcc::glcd::Color::signalViolet(), //SIGNALVIOLET
	xpcc::glcd::Color::emeraldGreen(), //EMERALDGREEN
	xpcc::glcd::Color::blue(),		// BORDER
	xpcc::glcd::Color::yellow(),	// TEXT
	xpcc::glcd::Color::black(),		// BACKGROUND
	xpcc::glcd::Color::red(),		// ACTIVATED
	xpcc::glcd::Color::blue(),		// DEACTIVATED
};

xpcc::gui::ColorPalette xpcc::gui::DefaultColorPalette(defaultColors);
