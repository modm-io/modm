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

#include "colorpalette.hpp"

using namespace modm::color::html;

modm::color::Rgb565 defaultColors[] = {
	Black,
	White,
	Gray,
	Red,
	Green,
	Blue,
	Yellow,
	Purple,
	MediumSeaGreen,
	Blue,			// BORDER
	Yellow,			// TEXT
	Black,			// BACKGROUND
	Red,			// ACTIVATED
	Blue,			// DEACTIVATED
};

modm::gui::ColorPalette modm::gui::DefaultColorPalette(defaultColors);
