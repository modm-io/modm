// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

// Inspired by: http://arduino.cc/en/Tutorial/Blink

#include "../../arduino_uno.hpp"

MAIN_FUNCTION
{
	DigitalOutput13::setOutput();

	while (1)
	{
		DigitalOutput13::set();
		xpcc::delay_ms(1000);
		DigitalOutput13::reset();
		xpcc::delay_ms(1000);
	}
}
