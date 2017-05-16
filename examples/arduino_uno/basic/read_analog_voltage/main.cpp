/*
 * Copyright (c) 2014, Kevin Läufer
 * Copyright (c) 2014, Sascha Schade
 * Copyright (c) 2014-2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

// Inspired by: http://arduino.cc/en/Tutorial/ReadAnalogVoltage

#include <modm/architecture/platform.hpp>

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
		// convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
		float voltage = sensorValue * (5.0 / 1023.0);
		// print voltage
		serialStream << voltage << "V" << modm::endl;
	}
}
