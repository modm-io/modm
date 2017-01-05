/*
 * Copyright (c) 2011-2012, Fabian Greif
 * Copyright (c) 2014-2016, Sascha Schade
 * Copyright (c) 2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/architecture/architecture.hpp>

#if defined(__AVR_AT90CAN128__) || defined(__AVR_AT90CAN64__) || defined(__AVR_AT90CAN32__)
using namespace modm::at90;
#else
using namespace modm::atmega;
#endif

typedef modm::avr::SystemClock clock;

typedef Uart0 uart;

typedef GpioOutputB0 Out;
typedef GpioInputB1  In;
typedef GpioB2       Io;

int
main()
{
	Out::setOutput();
	Out::setOutput(true);
	Out::set();
	Out::reset();	
	Out::toggle();
	
	In::setInput();
	In::read();
	
	Io::setOutput();
	Io::set();
	Io::setInput();
	Io::read();
	
#if defined(__AVR_ATmega64__)   || \
	defined(__AVR_ATmega645__)  || \
	defined(__AVR_ATmega6490__) || \
	defined(__AVR_ATmega128__)  || \
	defined(__AVR_ATmega2560__) || \
	defined(__AVR_AT90CAN128__) || \
	defined(__AVR_AT90CAN64__)  || \
	defined(__AVR_AT90CAN32__)
	typedef Uart0 uart;
	GpioOutputE1::connect(Uart0::Tx);
	GpioInputE0::connect(Uart0::Rx);
#elif defined(__AVR_ATmega640__) || \
	defined(__AVR_ATmega1280__)  || \
	defined(__AVR_ATmega1281__)  || \
	defined(__AVR_ATmega2561__)
	typedef Uart1 uart;
	GpioOutputD3::connect(Uart1::Tx);
	GpioInputD2::connect(Uart1::Rx);
#else
	typedef Uart0 uart;
	GpioOutputD1::connect(Uart0::Tx);
	GpioInputD0::connect(Uart0::Rx);
#endif

	uart::initialize<clock, 9600>();
	uart::write('x');
	
	while (1)
	{
	}
}
