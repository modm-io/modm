/*
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

#include "virtual_graphic_display.hpp"

modm::VirtualGraphicDisplay::VirtualGraphicDisplay(modm::ColorGraphicDisplay* display,
		modm::display::Point leftUpper, modm::display::Point rightLower):
		display(display), leftUpper(leftUpper), rightLower(rightLower),
		width(static_cast<uint16_t>(this->rightLower[0] - this->leftUpper[0])),
		height(static_cast<uint16_t>(this->rightLower[1] - this->leftUpper[1]))
{
}

void
modm::VirtualGraphicDisplay::setDisplay(modm::ColorGraphicDisplay* display)
{
	this->display = display;
	return;
}

void
modm::VirtualGraphicDisplay::clear()
{
	//TODO switch black , white
	this->display->color = color::html::Black;
	this->display->fillRectangle(this->leftUpper, width, height);
	this->display->color = color::html::White;
}

void
modm::VirtualGraphicDisplay::update()
{
	this->display->update();
	return;
}

void
modm::VirtualGraphicDisplay::setPixel(display::Point pos)
{
	this->display->setPixel(pos + display::Point(this->leftUpper[0], this->leftUpper[1]));
}

modm::color::Rgb565<true>
modm::VirtualGraphicDisplay::getPixel(display::Point pos) const
{
	return this->display->getPixel(pos + display::Point(this->leftUpper[0], this->leftUpper[1]));
}