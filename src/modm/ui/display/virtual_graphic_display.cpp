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
		modm::glcd::Point leftUpper, modm::glcd::Point rightLower):
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
	this->display->setColor(color::Rgb(0, 0, 0));
	this->display->fillRectangle(this->leftUpper, width, height);
	this->display->setColor(color::Rgb(255, 255, 255));
}

void
modm::VirtualGraphicDisplay::update()
{
	this->display->update();
	return;
}

void
modm::VirtualGraphicDisplay::setPixel(int16_t x, int16_t y)
{
	this->display->setPixel(x + this->leftUpper[0], y + this->leftUpper[1]);
}

void
modm::VirtualGraphicDisplay::clearPixel(int16_t x, int16_t y)
{
	this->display->clearPixel(x + this->leftUpper[0], y + this->leftUpper[1] );
}

modm::color::Rgb565
modm::VirtualGraphicDisplay::getPixel(int16_t x, int16_t y) const
{
	return this->display->getPixel(x + this->leftUpper[0], y + this->leftUpper[1] );
}