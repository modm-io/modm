/*
 * Copyright (c) 2019, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
using namespace Board;

__attribute__((noinline))
void function1(uint32_t bla)
{
	static_cast<void>(bla);

	if (Button::read()) {
		// execute undefined instructed
		// the hard fault handler will blink the blue LED
		// or, if the debugger is connected, will trigger a breakpoint
		asm volatile (".short 0xde00");
	}
}

__attribute__((noinline))
void function2(uint32_t bla, uint8_t blub)
{
	static_cast<void>(blub);
	function1(bla);
}

void modm_hardfault_entry()
{
	// Put hardware in safe mode here
	Board::Leds::set();
	// But do not wait forever
	modm::delayMilliseconds(100);
	// Do not depend on interrupts in this function (buffered UART etc!)
}

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	Board::Leds::setOutput(modm::Gpio::High);

	uint32_t *const ptr = new uint32_t[2*1024];
	MODM_LOG_INFO << "Can I allocate 2kB? answer: " << ptr << modm::endl;

	if (FaultReporter::hasReport())
	{
		MODM_LOG_ERROR << "\n\nHardFault! Copy the data into a 'coredump.txt' file, ";
		MODM_LOG_ERROR << "then execute 'scons postmortem firmware=";
		MODM_LOG_ERROR << modm::hex << FaultReporter::firmware() << "'.\n\n";
		for (const uint8_t data : FaultReporter())
		{
			MODM_LOG_ERROR << modm::hex << data << modm::flush;
		}
		MODM_LOG_ERROR << "\n\n\n" << modm::flush;
		FaultReporter::clearAndReboot();
	}

	MODM_LOG_INFO << "Hold Button to cause a Hardfault!" << modm::endl;

	if (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk) {
		MODM_LOG_INFO << "Debugger connected!" << modm::endl;
	}

	while (1)
	{
		Board::Leds::toggle();

		function2(23, 43);

		modm::delayMilliseconds(250);
	}

	return 0;
}
