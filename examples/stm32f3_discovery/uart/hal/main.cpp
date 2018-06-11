/*
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2014, Sascha Schade
 * Copyright (c) 2015-2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>

int
main()
{
	Board::initialize();

	// Initialize Usart Hal
	GpioInputA3::configure(Gpio::InputType::PullUp);
	GpioConnector<Peripheral::Usart2, GpioOutputA2::Tx, GpioInputA3::Rx>::connect();
//	UsartHal2::initialize<Board::systemClock, 115200>(UsartHal2::Parity::Odd);
	UsartHal2::enableInterruptVector(true, 14);
	UsartHal2::enableInterrupt(UsartHal2::Interrupt::TxEmpty);
	UsartHal2::setTransmitterEnable(true);

	while (1)
	{
	}

	return 0;
}

/* FIXME: Since --whole-archive option, the interrupt defined in libmodm.a conflicts
// Interrupt Handler
MODM_ISR(USART2)
{
	UsartHal2::getInterruptFlags();
	if(UsartHal2::isTransmitRegisterEmpty()) {
		UsartHal2::write('A');
	}
}
*/