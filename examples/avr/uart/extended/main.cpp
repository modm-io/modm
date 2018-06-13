/*
 * Copyright (c) 2010-2011, Fabian Greif
 * Copyright (c) 2012-2014, 2017, Niklas Hauser
 * Copyright (c) 2013, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/platform.hpp>
#include <modm/io/iostream.hpp>

using namespace modm::platform;
using systemClock = SystemClock;

int
main()
{
	Uart0::connect<GpioOutputD1::Txd, GpioInputD0::Rxd>();
	Uart0::initialize<systemClock, 115200>();

	// Enable interrupts, this is needed for every buffered UART
	enableInterrupts();

	// Create a IOStream for complex formatting tasks
	modm::IODeviceWrapper< Uart0, modm::IOBuffer::BlockIfFull > device;
	modm::IOStream stream(device);

	// Now we can print numbers and other objects to the stream
	// The following will result in the string "24 is a nice number!\n" with
	// the number formatted as ASCII text.
	stream << 24 << " is a nice number!" << modm::endl;

	while (1)
	{
	}
}
