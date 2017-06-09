/*
 * Copyright (c) 2010-2013, Fabian Greif
 * Copyright (c) 2012-2013, 2017, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/architecture/platform.hpp>

using namespace modm::platform;
typedef modm::platform::SystemClock clock;

// Create a new UART object and configure it to a baudrate of 115200
typedef Uart0 uart;

int
main()
{
    GpioOutputD1::connect(Uart0::Tx);
    GpioInputD0::connect(Uart0::Rx);
    uart::initialize<clock, 115200>();

    // Enable interrupts, this is needed for every buffered UART
	sei();
	
	// Write some characters
    uart::write('H');
    uart::write('e');
    uart::write('l');
    uart::write('l');
    uart::write('o');
    uart::write('\n');
	
	while (1)
	{
	}
}
