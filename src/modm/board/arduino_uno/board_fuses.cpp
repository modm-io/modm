/*
 * Copyright (c) 2021, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "board.hpp"

// Defaults taken from Arduino's boards.txt
// see https://github.com/arduino/ArduinoCore-avr/blob/master/boards.txt
modm_weak FUSES =
{
	0xFF, // .low
	0xDE, // .high
	0xFD, // .extended
};
