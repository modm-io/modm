/*
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2014, Kevin Läufer
 * Copyright (c) 2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "homeview.hpp"

HomeView::HomeView(modm::gui::GuiViewStack* stack, uint8_t identifier) :
	View(stack, identifier, modm::gui::Dimension(320, 240)),
	toggleLedButton((char*)"Win Eurobot", modm::gui::Dimension(100, 50)),
	doNothingButton((char*)"Delete MODM", modm::gui::Dimension(100, 50)),
	rocker1(100, 50, modm::gui::Dimension(200, 30)),
	checkbox1(true, modm::gui::Dimension(30, 30))
{
	this->colorpalette.setColor(modm::gui::Color::BACKGROUND, modm::glcd::Color::red());

	this->pack(&toggleLedButton, modm::glcd::Point(110, 10));
	this->pack(&doNothingButton, modm::glcd::Point(110, 80));
	this->pack(&checkbox1, modm::glcd::Point(60, 140));
	this->pack(&rocker1, modm::glcd::Point(60, 200));
}

bool
HomeView::hasChanged()
{
	return true;
}
