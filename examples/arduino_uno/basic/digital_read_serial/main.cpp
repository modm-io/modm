// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

// Inspired by: http://arduino.cc/en/Tutorial/DigitalReadSerial

#include <xpcc/architecture/platform.hpp>

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
		serialStream << buttonState << xpcc::endl;
	}
}
