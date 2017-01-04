/*
 * Copyright (c) 2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

// AVR AT90CAN128 breakout board AL-AVREB_CAN
// https://www.alvidi.de/products/DE/AVR_Entwicklungsboards/avr_modul_avreb_can.php
// Tested with 16MHz external crystal

#ifndef XPCC_AL_AVREB_CAN_HPP
#define XPCC_AL_AVREB_CAN_HPP

#include <modm/architecture/platform.hpp>
#include <modm/debug/logger.hpp>

using namespace xpcc::at90;

namespace Board
{

using systemClock = xpcc::avr::SystemClock;

// Arduino Footprint
using Led0 = xpcc::GpioInverted<GpioOutputF0>;
using Led1 = xpcc::GpioInverted<GpioOutputF1>;
using Led2 = xpcc::GpioInverted<GpioOutputF2>;
using Led3 = xpcc::GpioInverted<GpioOutputF3>;

using Button = xpcc::GpioUnused;
using Leds = xpcc::SoftwareGpioPort< Led3, Led2, Led1, Led0 >;


inline void
initialize()
{
	systemClock::enable();

	GpioD2::connect(Uart1::Rx);
	GpioD3::connect(Uart1::Tx);
	Uart1::initialize<systemClock, 38400>();

	// xpcc::Clock initialization
	// Clear Timer on Compare Match Mode
	TCCR0A = (1 << WGM01);
	TIMSK0 = (1 << OCIE0A);
#if F_CPU > 16000000
	// Set and enable output compare A
	OCR0A = F_CPU / (1000ul * 256);
	// Set prescaler 256 and enable timer
	TCCR0A = (1 << CS02);
#elif F_CPU > 2000000
	// Set and enable output compare A
	OCR0A = F_CPU / (1000ul * 64);
	// Set prescaler 64 and enable timer
	TCCR0A = (1 << CS01) | (1 << CS00);
#elif F_CPU > 1000000
	// Set and enable output compare A
	OCR0A = F_CPU / (1000ul * 8);
	// Set prescaler 8 and enable timer
	TCCR0A = (1 << CS01);
#endif

	enableInterrupts();
}

}

using namespace Board;
extern xpcc::IOStream serialStream;

#endif	// XPCC_ARDUINO_UNO_HPP
