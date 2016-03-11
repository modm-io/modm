// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

// Inspired by: http://arduino.cc/en/Tutorial/Blink

#include <xpcc/architecture/platform.hpp>

int
main()
{
	Board::initialize();
	LedD13::setOutput();

	while (1)
	{
		LedD13::set();
		xpcc::delayMilliseconds(1000);
		LedD13::reset();
		xpcc::delayMilliseconds(1000);
	}
}
