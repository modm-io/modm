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

modm::glcd::Color defaultColors[] = {
	modm::glcd::Color::black(),		// BLACK
	modm::glcd::Color::white(),		// WHITE
	modm::glcd::Color::gray(),		// GRAY
	modm::glcd::Color::red(),		// RED
	modm::glcd::Color::green(),		// GREEN
	modm::glcd::Color::blue(),		// BLUE
	modm::glcd::Color::yellow(),	// YELLOW
	modm::glcd::Color::signalViolet(), //SIGNALVIOLET
	modm::glcd::Color::emeraldGreen(), //EMERALDGREEN
	modm::glcd::Color::blue(),		// BORDER
	modm::glcd::Color::yellow(),	// TEXT
	modm::glcd::Color::black(),		// BACKGROUND
	modm::glcd::Color::red(),		// ACTIVATED
	modm::glcd::Color::blue(),		// DEACTIVATED
};

modm::gui::ColorPalette modm::gui::DefaultColorPalette(defaultColors);
