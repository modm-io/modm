/*
 * Copyright (c) 2016-2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/debug/logger.hpp>

/* This example showcases the hard fault handler LED blinking and UART logging.
 * You can trigger the hard fault by pressing the blue button, which will execute an undefined instruction.
 * The hard fault handler will report failure reason over PA2 @ 115.2kBaud and flash the blue LED.
 *
 * Connecting a debugger changes this behavior: the hardfault will now trigger a breakpoint.
 * This allows normal debugging of the failure reason.
 * You can try this yourself, open two shell in this folder, then:
 *  - in one terminal execute `scons openocd-debug`. This starts the openocd server.
 *  - then execute `scons debug`, which starts the GDB debugger in TUI mode.
 *  - type `monitor reset halt`, then `c` into GDB to restart the controller.
 * The red LED should now be on, signaling that the debugger has been recognized.
 * Pressing the blue button now, will make GDB jump to the breakpoint.
 *
 * You need to power cycle the board to reset the debugger recognition.
 */


// ----------------------------------------------------------------------------
// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::INFO

// Create an IODeviceWrapper around the Uart Peripheral we want to use
modm::IODeviceWrapper< Usart2, modm::IOBuffer::BlockIfFull > loggerDevice;

// Set all four logger streams to use the UART
modm::log::Logger modm::log::debug(loggerDevice);
modm::log::Logger modm::log::info(loggerDevice);
modm::log::Logger modm::log::warning(loggerDevice);
modm::log::Logger modm::log::error(loggerDevice);

using namespace Board;

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	LedOrange::set();

	// initialize Uart2 for MODM_LOG_
	Usart2::connect<GpioA2::Tx>();
	Usart2::initialize<Board::systemClock, 115200>();

	MODM_LOG_INFO << "Press Button to cause a Hardfault!" << modm::endl;

	if (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk) {
		// if this LED is on, the debugger is connected
		LedRed::set();
		MODM_LOG_INFO << "Debugger connected!" << modm::endl;
	}

	while (1)
	{
		LedGreen::toggle();
		LedOrange::toggle();

		if (Button::read()) {
			// execute undefined instructed
			// the hard fault handler will blink the blue LED
			// or, if the debugger is connected, will trigger a breakpoint
			asm volatile (".short 0xde00");
		}

		modm::delayMilliseconds(500);

	}

	return 0;
}
