/*
 * Copyright (c) 2014, Kevin Läufer
 * Copyright (c) 2014, Sascha Schade
 * Copyright (c) 2014-2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

// Inspired by: http://arduino.cc/en/Tutorial/AnalogReadSerial

#include <modm/board/board.hpp>

int
main()
{
	Board::initialize();

	// Initialize the analog to digital converter
	// With the AVR running at 16Mhz and a prescaler of 128 the
	// ADC is running at 125kHz.
	Adc::initialize<Board::systemClock, 125000>();
	Adc::setReference(Adc::Reference::InternalVcc);

	while (1)
	{
		// read the input on analog pin 0
		int sensorValue = Adc::readChannel(0);
		// print analog readout
		serialStream << sensorValue << modm::endl;
	}
}
