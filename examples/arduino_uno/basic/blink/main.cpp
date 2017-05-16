/*
 * Copyright (c) 2013-2014, Kevin Läufer
 * Copyright (c) 2013, 2016, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

// Inspired by: http://arduino.cc/en/Tutorial/Blink

#include <modm/architecture/platform.hpp>

int
main()
{
	Board::initialize();
	LedD13::setOutput();

	while (1)
	{
		LedD13::set();
		modm::delayMilliseconds(1000);
		LedD13::reset();
		modm::delayMilliseconds(1000);
	}
}
