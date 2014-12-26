// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

// Inspired by: http://arduino.cc/en/Tutorial/AnalogReadSerial

#include "../../arduino_uno.hpp"

// create a device wrapper and a stream in oder to be able to send strings
xpcc::IODeviceWrapper< Uart0, xpcc::IOBuffer::BlockIfFull > serialDevice;
xpcc::IOStream serialStream(serialDevice);

MAIN_FUNCTION
{
	// initialize the serial communication module
	DigitalInput0::connect(Uart0::Rx);
	DigitalOutput1::connect(Uart0::Tx);
	Uart0::initialize<clock, 9600>();

	// Initialize the analog to digital converter
	// With the AVR running at 16Mhz and a prescaler of 128 the
	// ADC is running at 125kHz.
	Adc::initialize(Adc::Reference::InternalAVcc, Adc::Prescaler::Div128);

	while (1)
	{
		// read the input on analog pin 0
		int sensorValue = Adc::readChannel(0);
		// print analog readout
		serialStream << sensorValue << xpcc::endl;
	}
}
