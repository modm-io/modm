/*
 * Copyright (c) 2012, 2014, Sascha Schade
 * Copyright (c) 2013-2014, Kevin Läufer
 * Copyright (c) 2014-2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

/**
 * \brief	Minimal example for LPC11C24 on LPCXpresso board.
 *
 *
 */

#include <modm/architecture/architecture.hpp>

#include <modm/debug/logger.hpp>

using namespace modm::platform;

typedef GpioOutput0_7 Led;
typedef GpioOutput3_1 WriteInd;

typedef SystemClock<Pll<ExternalCrystal<MHz12>, MHz48>> systemClock;

// ----------------------------------------------------------------------------
// Logging
typedef Uart1 Uart;
modm::IODeviceWrapper< Uart, modm::IOBuffer::BlockIfFull > loggerDevice;

modm::log::Logger modm::log::debug(loggerDevice);
modm::log::Logger modm::log::info(loggerDevice);
modm::log::Logger modm::log::warning(loggerDevice);
modm::log::Logger modm::log::error(loggerDevice);
// ----------------------------------------------------------------------------


void
testWriteSingle(void);

void
testWriteBuffer(void);

void
testModmLogger(void);

int
main()
{
	systemClock::enable();

	// Initialize 32-bit timer 0. TIME_INTERVAL is defined as 10mS
	// You may also want to use the Cortex SysTick timer to do this
	// init_timer32(0, TIME_INTERVAL);

	// Enable timer 0. Our interrupt handler will begin incrementing
	// the TimeTick global each time timer 0 matches and resets.
	// enable_timer32(0);

	// Set LED port pin to output
	Led::setOutput();

	Uart::initialize<systemClock, 115200>();

	modm::delayMilliseconds(100); // glitch ?

	Uart::write('X');
	Uart::write('I');
	Uart::write('X');

//	testWriteSingle();
//	testWriteBuffer();

	testModmLogger();

	while(1);

		// Single byte reading and writing, loopback.
//		uint8_t c;
//		if (Uart::read(c))
//		{
//			Uart::write(c);
//		}

		// Buffer reading and writing
//		uint8_t buf[16];
//		uint8_t rr = Uart::read(buf, 16);
//		Uart::write(buf, rr);
}

void
testWriteSingle(void)
{
	while (1)
	{
		Led::toggle();

		// Multiple single byte writes in blocking mode
		Uart::write('\n');
		for (uint8_t ii = 'a'; ii < 't'; ++ii) {
			WriteInd::setOutput(true);
			Uart::write(ii);
			WriteInd::setOutput(false);
		}

		modm::delayMilliseconds(2);

		// Some pause between burst otherwise USB is overloaded.
		static uint8_t burst = 0;
		if (burst++ > 5)
		{
			burst = 0;
			modm::delayMilliseconds(500);
		}
	} // while (1)
}

void
testWriteBuffer(void)
{
	uint8_t buf[40];
	for (uint8_t ii = 0; ii < 40; ++ii)
	{
		buf[ii] = ii + 'A';
	}

	while (1)
	{
		Led::toggle();

		WriteInd::setOutput(true);
		Uart::write(buf, 40);
		WriteInd::setOutput(false);
		WriteInd::setOutput(true);
		Uart::write('\n');
		WriteInd::setOutput(false);

		modm::delayMilliseconds(2);

		// Some pause between burst otherwise USB is overloaded.
		static uint8_t burst = 0;
		if (burst++ > 5) {
			burst = 0;
			modm::delayMilliseconds(5000);
		}
	} // while (1)
}

void
testModmLogger(void)
{
	while (1)
	{
		MODM_LOG_DEBUG << "Hello world" << modm::endl;
	}
}
