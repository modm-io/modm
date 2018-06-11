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

#include <modm/platform.hpp>

using namespace modm::platform;
using systemClock = SystemClock;

int
main()
{
	Uart0::connect<GpioOutputD1::Txd, GpioInputD0::Rxd>();
	Uart0::initialize<systemClock, 115200>();

	// Enable interrupts, this is needed for every buffered UART
	enableInterrupts();

	// Write some characters
	Uart0::write('H');
	Uart0::write('e');
	Uart0::write('l');
	Uart0::write('l');
	Uart0::write('o');
	Uart0::write('\n');

	while (1)
	{
	}
}
