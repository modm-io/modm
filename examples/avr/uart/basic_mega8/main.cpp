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

using namespace modm::literals;
using namespace modm::platform;

int
main()
{
	Uart::connect<GpioOutputD1::Txd, GpioInputD0::Rxd>();
	Uart::initialize<SystemClock, 4800_Bd>();

	// Enable interrupts, this is needed for every buffered UART
	enableInterrupts();

	// Write some characters
	Uart::write('H');
	Uart::write('e');
	Uart::write('l');
	Uart::write('l');
	Uart::write('o');
	Uart::write('\n');

	while (1)
	{
	}
}
