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
	modm::delay(100ms);
	// Do not depend on interrupts in this function (buffered UART etc!)
}

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	LedOrange::set();

	uint32_t *const ptr = new uint32_t[20*1024];
	MODM_LOG_INFO << "Can I allocate 20kB? answer: " << ptr << modm::endl;

	if (FaultReporter::hasReport())
	{
		MODM_LOG_ERROR << "\n\nHardFault! Copy the data into a 'coredump.txt' file, ";
		MODM_LOG_ERROR << "then execute\n\n\tscons debug-coredump ";
#ifdef MODM_DEBUG_BUILD
		MODM_LOG_ERROR << "profile=debug ";
#endif
		MODM_LOG_ERROR << "firmware=" << modm::hex;
		for (const auto data : FaultReporter::buildId()) MODM_LOG_ERROR << data;
		MODM_LOG_ERROR << "\nor\n\tmake debug-coredump";
#ifdef MODM_DEBUG_BUILD
		MODM_LOG_ERROR << " profile=debug";
#endif
		MODM_LOG_ERROR << "\n\n";
		for (const auto data : FaultReporter())
			MODM_LOG_ERROR << modm::hex << data << modm::flush;
		MODM_LOG_ERROR << "\n\n\n" << modm::flush;
		FaultReporter::clearAndReboot();
	}

	MODM_LOG_INFO << "Hold Button to cause a Hardfault!" << modm::endl;

	if (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk) {
		// if this LED is on, the debugger is connected
		LedRed::set();
		MODM_LOG_INFO << "Debugger connected!" << modm::endl;
	}

	while (true)
	{
		LedGreen::toggle();
		LedOrange::toggle();

		function2(23, 43);

		modm::delay(250ms);
	}

	return 0;
}
