/*
 * Copyright (c) 2021, Tomasz Wasilczyk
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>

int
main()
{
	Board::initialize();

	auto& display = Board::display;

	display.setCursor(115, 54);
	display.setFont(modm::font::Ubuntu_36);
	display << "Hello World";
	display.drawRoundedRectangle({100, 48}, 184, 40, 10);

	display.update();
}
