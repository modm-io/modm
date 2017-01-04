/*
 * Copyright (c) 2010-2011, Fabian Greif
 * Copyright (c) 2012-2014, Niklas Hauser
 * Copyright (c) 2013, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/architecture/platform.hpp>
#include <modm/io/iostream.hpp>

using namespace xpcc::atmega;
typedef xpcc::avr::SystemClock systemClock;

// Create a new UART object and configure it to a baudrate of 115200
Uart0 uart;

int
main()
{
	uart.initialize<systemClock, 115200>();

	// Enable interrupts, this is needed for every buffered UART
	sei();

	// Create a IOStream for complex formatting tasks
	xpcc::IODeviceWrapper< Uart0, xpcc::IOBuffer::BlockIfFull > device;
	xpcc::IOStream stream(device);

	// Now we can print numbers and other objects to the stream
	// The following will result in the string "24 is a nice number!\n" with
	// the number formatted as ASCII text.
	stream << 24 << " is a nice number!" << xpcc::endl;

	while (1)
	{
	}
}
