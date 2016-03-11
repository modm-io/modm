// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

// Inspired by: http://arduino.cc/en/Tutorial/ReadAnalogVoltage

#include <xpcc/architecture/platform.hpp>

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
		serialStream << voltage << "V" << xpcc::endl;
	}
}
