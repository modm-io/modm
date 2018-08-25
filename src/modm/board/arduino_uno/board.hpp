/*
 * Copyright (c) 2016-2018, Niklas Hauser
 * Copyright (c) 2017, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

//
// Arduino UNO
//

#ifndef MODM_ARDUINO_UNO_HPP
#define MODM_ARDUINO_UNO_HPP

#include <modm/platform.hpp>
#include <modm/debug/logger.hpp>
#define MODM_BOARD_HAS_LOGGER

using namespace modm::platform;
#define MODM_BOARD_ARDUINO_UNO


namespace Board
{

using systemClock = modm::platform::SystemClock;

// Arduino Footprint
using A0 = GpioC0;
using A1 = GpioC1;
using A2 = GpioC2;
using A3 = GpioC3;
using A4 = GpioC4;
using A5 = GpioC5;

using D0  = GpioD0;
using D1  = GpioD1;
using D2  = GpioD2;
using D3  = GpioD3;
using D4  = GpioD4;
using D5  = GpioD5;
using D6  = GpioD6;
using D7  = GpioD7;
using D8  = GpioB0;
using D9  = GpioB1;
using D10 = GpioB2;
using D11 = GpioB3;
using D12 = GpioB4;
using D13 = GpioB5;

using Button = GpioUnused;
using LedD13 = D13;

using Leds = SoftwareGpioPort< LedD13 >;

using LoggerDevice = modm::IODeviceWrapper< Uart0, modm::IOBuffer::BlockIfFull >;

inline void
initialize()
{
	systemClock::enable();

	// Uart0 is connected to onboard USB bridge
	Uart0::connect<D1::Txd, D0::Rxd>();
	Uart0::initialize<systemClock, 9600>();

	// modm::Clock initialization
	// Clear Timer on Compare Match Mode
	TCCR0A = (1 << WGM01);
	TIMSK0 = (1 << OCIE0A);
#if F_CPU > 16000000
	// Set and enable output compare A
	OCR0A = F_CPU / (1000ul * 256);
	// Set prescaler 256 and enable timer
	TCCR0B = (1 << CS02);
#elif F_CPU > 2000000
	// Set and enable output compare A
	OCR0A = F_CPU / (1000ul * 64);
	// Set prescaler 64 and enable timer
	TCCR0B = (1 << CS01) | (1 << CS00);
#elif F_CPU > 1000000
	// Set and enable output compare A
	OCR0A = F_CPU / (1000ul * 8);
	// Set prescaler 8 and enable timer
	TCCR0B = (1 << CS01);
#endif

	enableInterrupts();
}

}

using namespace Board;
extern modm::IOStream serialStream;

#endif	// MODM_ARDUINO_UNO_HPP
