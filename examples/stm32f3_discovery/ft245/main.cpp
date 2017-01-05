/*
 * Copyright (c) 2015, Sascha Schade
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
#include <modm/driver/usb/ft245.hpp>

/**
 * Example of FT245 usage.
 *
 * Connect D0 to D7 of FT245 to PD0 to PD7.
 * Connect Rxf, Txe, Rd and Wr to their corresponding pins.
 * Start a terminal and connect to the virtual serial port of the FT245.
 * The microcontroller echos the received character increased by one,
 * e.g. send 'a' and receive 'b' from the microcontroller.
 * For every character received the red LED toggles once.
 *
 */
int
main()
{
	Board::initialize();

	typedef modm::stm32::GpioPort<modm::stm32::GpioD0, 8> myPort;
	typedef modm::stm32::GpioInputC11  Rxf;
	typedef modm::stm32::GpioInputC10  Txe;
	typedef modm::stm32::GpioOutputA15 Rd;
	typedef modm::stm32::GpioOutputA9  Wr;

	Rd::setOutput(modm::Gpio::High);
	Wr::setOutput(modm::Gpio::Low);

	typedef modm::Ft245<myPort, Rd, Wr, Rxf, Txe> myFt;

	while (1)
	{
		uint8_t c;
		if (myFt::read(c)) {
			myFt::write(c + 1);
			Board::LedNorth::toggle();
		}
	}

	return 0;
}
