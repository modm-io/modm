// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

// Inspired by: http://arduino.cc/en/Tutorial/DigitalReadSerial

#include "../../arduino_uno.hpp"

// create a device wrapper and a stream in oder to be able to send strings
xpcc::IODeviceWrapper< Uart0, xpcc::IOBuffer::BlockIfFull > serialDevice;
xpcc::IOStream serialStream(serialDevice);

// name pin2
typedef DigitalInput2 PushButton;

int
main()
{
	// initialize the serial communication module
	DigitalInput0::connect(Uart0::Rx);
	DigitalOutput1::connect(Uart0::Tx);
	Uart0::initialize<clock, 9600>();

	PushButton::setInput();

	while (1)
	{
		// read the input pin
		bool buttonState = PushButton::read();
		// print button state
		serialStream << buttonState << xpcc::endl;
	}
}
