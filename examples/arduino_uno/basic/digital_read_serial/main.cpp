/*
 * Copyright (c) 2014, Kevin Läufer
 * Copyright (c) 2014, Sascha Schade
 * Copyright (c) 2014, 2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

// Inspired by: http://arduino.cc/en/Tutorial/DigitalReadSerial

#include <modm/architecture/platform.hpp>

// name pin2
using PushButton = D2;

int
main()
{
	Board::initialize();
	PushButton::setInput();

	while (1)
	{
		// read the input pin
		bool buttonState = PushButton::read();
		// print button state
		serialStream << buttonState << modm::endl;
	}
}
