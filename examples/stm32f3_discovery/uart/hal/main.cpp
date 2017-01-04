/*
 * Copyright (c) 2013, Kevin Laeufer
 * Copyright (c) 2014, Sascha Schade
 * Copyright (c) 2015-2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/architecture/platform.hpp>

int
main()
{
	Board::initialize();

	// Initialize Usart Hal
	GpioOutputA2::connect(UsartHal2::Tx);
	GpioInputA3::connect(UsartHal2::Rx, Gpio::InputType::PullUp);
//	UsartHal2::initialize<Board::systemClock, 115200>(UsartHal2::Parity::Odd);
	UsartHal2::enableInterruptVector(true, 14);
	UsartHal2::enableInterrupt(UsartHal2::Interrupt::TxEmpty);
	UsartHal2::setTransmitterEnable(true);

	while (1)
	{
	}

	return 0;
}

// Interrupt Handler
XPCC_ISR(USART2)
{
	UsartHal2::getInterruptFlags();
	if(UsartHal2::isTransmitRegisterEmpty()) {
		UsartHal2::write('A');
	}
}
