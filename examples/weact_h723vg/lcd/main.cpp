/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>

namespace
{

void
drawTestPattern()
{
	Board::display.setOrientation(modm::glcd::Orientation::Landscape0);
	Board::display.setBackgroundColor(modm::color::html::Black);
	Board::display.clear();

	Board::display.setColor(modm::color::html::Red);
	Board::display.fillRectangle(4, 4, 72, 32);
	Board::display.setColor(modm::color::html::Green);
	Board::display.fillRectangle(84, 4, 72, 32);
	Board::display.setColor(modm::color::html::Blue);
	Board::display.fillRectangle(4, 44, 72, 32);
	Board::display.setColor(modm::color::html::White);
	Board::display.fillRectangle(84, 44, 72, 32);

	Board::display.setColor(modm::color::html::Yellow);
	Board::display.drawRectangle(0, 0, Board::display.getWidth(), Board::display.getHeight());
}

}  // namespace

int
main()
{
	Board::initialize();
	Board::initializeDisplay();
	Board::setDisplayBacklight(true);
	drawTestPattern();

	while (true)
	{
		if (Board::Button::read())
		{
			Board::setDisplayBacklight(false);
		} else
		{
			Board::setDisplayBacklight(true);
		}
		modm::delay(20ms);
	}
}
